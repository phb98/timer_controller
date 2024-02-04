#include "scheduler.h"
#include <stdint.h>
#include "rtc.h"
#include "console.h"
#include "core_json.h"
#include "scheduler_file.h"
#include "assert.h"
// Private function prototype
void rtc_cb(const rtc_t * p_new_time);
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
  rtc_register_cb(rtc_cb);
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
  for(int i = 0; i < 2; i++)
  {
    test.act_param.output.value = i*100;
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
void rtc_cb(const rtc_t * p_new_time)
{
  ASSERT_LOG_ERROR_RETURN(p_new_time, "iNVALID PARAM");

}
