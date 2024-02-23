#include "scheduler.h"
#include <stdint.h>
#include "rtc.h"
#include "console.h"
#include "core_json.h"
#include "scheduler_file.h"
#include "assert.h"
// Private function prototype
static void scheduler_rtc_cb(const rtc_t * p_new_time);
static void scheduler_recover(const rtc_t * const p_rtc_start, const rtc_t * const p_rtc_end);
// Private variable
const char * scheduler_action_str[NUM_OF_SCHEDULER_ACTION] = 
{
  [SCHEDULER_ACTION_OUTPUT] = "output"
};
// Public function
void sheduler_init()
{
  CONSOLE_LOG_INFO("Init Scheduler");
  scheduler_file_init();
  rtc_t rtc_start = {.time = {.hour = 0, .minute = 0, .second = 0}};
  rtc_t current_time;
  rtc_get_current(&current_time);
  scheduler_recover(&rtc_start, &current_time);
  rtc_register_cb(scheduler_rtc_cb);
  #if 0
  scheduler_t test = 
  {
    .trig_time = 
    {
      .date = {
        .year = 2024,
        .month = 2,
        .day = 1,
        .DoW = 0,
      },
      .time = {
        .hour = 10,
        .minute = 1,
        .second = 52
      },
    },
    .month_mask = 0xFFFF,
    .dow_mask = 0xFF,
    .action = SCHEDULER_ACTION_OUTPUT,
    .act_param = 
    {
      .output = {
        .channel = 0,
        .value = 1234,
      }
    }
  };
  scheduler_file_delete_all();
  for(int i = 0; i < 1000; i++)
  {
    test.act_param.output.value = i;
    test.act_param.output.channel = 2;

    scheduler_new(&test);
  }
  #endif
}

scheduler_ret_t scheduler_new(const scheduler_t * const p_new_scheduler)
{
  if(!p_new_scheduler) return SCHEDULER_ERR_PARAM;
  scheduler_ret_t ret = SCHEDULER_OK;
  ret = scheduler_file_check_exist(p_new_scheduler);
  if(ret != SCHEDULER_OK)
  {
    if(ret == SCHEDULER_ERR_DUPLICATE)
    {
      CONSOLE_LOG_WARN("Scheduler already exist");
    }
    return ret;
  }
  ret = scheduler_file_new_scheduler(p_new_scheduler);
  return ret;
}
/**
 * @note if p_new_scheduler is NULL, it delete the scheduler with id
 */

scheduler_ret_t scheduler_modify(scheduler_t * const p_new_scheduler, uint32_t id)
{
  return scheduler_file_modify(p_new_scheduler, id);
}
// Private function
static void scheduler_rtc_cb(const rtc_t * p_new_time)
{
  ASSERT_LOG_ERROR_RETURN(p_new_time, "iNVALID PARAM");
  uint32_t num_sch = 0;
  if(scheduler_file_get_num_sch(&num_sch) != SCHEDULER_OK)
  {
    CONSOLE_LOG_ERROR("Get num scheduler fail");
    return;
  }
  scheduler_t sch;
  for(int i = 0; i < num_sch; i++)
  {
    if(scheduler_file_get_scheduler(&sch, i) != SCHEDULER_OK)
    {
      CONSOLE_LOG_ERROR("Get scheduler fail");
      return;
    }
    if(rtc_is_equal(&sch.trig_time, p_new_time))
    {
      switch(sch.action)
      {
        case SCHEDULER_ACTION_OUTPUT:
        {
          output_controller_set_val(sch.act_param.output.channel, sch.act_param.output.value);
          break;
        }
        default:
        {
          CONSOLE_LOG_ERROR("Unknow action");
          break;
        }
      }
    }
  }
}

static void scheduler_recover(const rtc_t * const p_rtc_start, const rtc_t * const p_rtc_end)
{
  ASSERT_LOG_ERROR_RETURN(p_rtc_start && p_rtc_end, "Invalid param");
  #define NUM_CACHE_SCH (4)
  scheduler_t sch_cache[NUM_CACHE_SCH] = {0};
  uint32_t num_sch_stored = 0;
  uint32_t num_sch_in_cache = NUM_CACHE_SCH;
  uint32_t sch_query_idx = 0;
  output_val_t val_buffer[CONFIG_OUTPUT_TOTAL_CH] = {0};

  CONSOLE_LOG_INFO("Begin recover scheduler");
  if(scheduler_file_get_num_sch(&num_sch_stored) != SCHEDULER_OK)
  {
    CONSOLE_LOG_ERROR("Get num scheduler fail");
    return;
  }
  while(num_sch_stored)
  {
    if(scheduler_file_get_multi_scheduler(sch_cache, sch_query_idx, &num_sch_in_cache) != SCHEDULER_OK)
    {
      CONSOLE_LOG_ERROR("Get scheduler fail");
      return;
    }
    // Check every second, minute, hour
    // Todo: optimize this
    for(int h = p_rtc_start->time.hour; h <= p_rtc_end->time.hour; h++)
    {
      for(int m = p_rtc_start->time.minute; m <= p_rtc_end->time.minute; m++)
      {
        for(int s = p_rtc_start->time.second; s <= p_rtc_end->time.second; s++)
        {
          for(int i = 0; i < num_sch_in_cache; i++)
          {
            if(sch_cache[i].trig_time.time.hour == h && sch_cache[i].trig_time.time.minute == m && sch_cache[i].trig_time.time.second == s)
            {
              switch(sch_cache[i].action)
              {
                case SCHEDULER_ACTION_OUTPUT:
                {
                  val_buffer[sch_cache[i].act_param.output.channel] = sch_cache[i].act_param.output.value;
                  break;
                }
                default:
                {
                  CONSOLE_LOG_ERROR("Unknow action");
                  break;
                }
              }
            }
          }
        }
      }
    }
    sch_query_idx += num_sch_in_cache;
    num_sch_stored -= num_sch_in_cache;
  }
  // write to output
  for(int i = 0; i < CONFIG_OUTPUT_TOTAL_CH; i++)
  {
    output_controller_set_val(i, val_buffer[i]);
  }
  CONSOLE_LOG_INFO("Finish recover scheduler");
}