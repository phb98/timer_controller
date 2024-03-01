#include "output_controller.h"
#include "scheduler_file.h"
#include "scheduler.h"
#include "console.h"
#include "scheduler_output.h"


scheduler_ret_t scheduler_output_get_by_channel(const output_ch_t ch, uint32_t start_idx, scheduler_t *p_out_sch, uint32_t * p_num_out)
{
  ASSERT_LOG_ERROR_RETURN_VAL(p_out_sch && p_num_out, false, "Invalid param");
  scheduler_ret_t ret = SCHEDULER_OK;
  uint32_t num_sch;
  uint32_t num_sch_found = 0;
  scheduler_t *p_writting_sch = p_out_sch;
  ret = scheduler_file_get_num_sch(&num_sch);
  if(ret != SCHEDULER_OK)
  {
    return ret;
  }
  for(uint32_t i = start_idx; (i < num_sch) && (num_sch_found < *p_num_out); i++)
  {
    ret = scheduler_file_get_scheduler(p_writting_sch, i);
    if(ret != SCHEDULER_OK)
    {
      break;
    }
    if(p_writting_sch->act_param.output.channel == ch)
    {
      num_sch_found++;
      p_writting_sch++;
    }
  }
  *p_num_out = num_sch_found;
  return ret;
}

scheduler_ret_t scheduler_output_set_channel(const scheduler_t * const p_sch)
{
  ASSERT_LOG_ERROR_RETURN(p_sch, "Invalid param");
  scheduler_ret_t ret = SCHEDULER_OK;
  if(p_sch->action != SCHEDULER_ACTION_OUTPUT)
  {
    CONSOLE_LOG_ERROR("Invalid action");
    return SCHEDULER_ERR_INVALID_ACTION;
  }
  CONSOLE_LOG_DEBUG("Set output channel %d to %d", p_sch->act_param.output.channel, p_sch->act_param.output.value);
  output_controller_set_val(p_sch->act_param.output.channel, p_sch->act_param.output.value);
  return ret;
}