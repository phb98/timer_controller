#ifndef _SHEDULER_H_
#define _SHEDULER_H_
#include "rtc.h"
#include <stdbool.h>
#include "output_controller.h"
typedef uint8_t scheduler_dow_mask_t;   // 0x00: daily, 0xFF: 1 shot, not implement for now
typedef uint16_t scheduler_month_mask_t; // 0x00: Every month, 0xffff: 1 shot, not immplement for now
typedef enum
{
  SCHEDULER_ACTION_OUTPUT = 0,
  NUM_OF_SCHEDULER_ACTION
} scheduler_action_t;
extern const char* scheduler_action_str[]; //just declaration

typedef union
{
  struct {
    output_ch_t   channel;
    output_val_t  value;
  }output;
} scheduler_action_param_t;
typedef struct
{
  uint32_t                   id;
  rtc_t                      trig_time;
  scheduler_dow_mask_t       dow_mask;
  scheduler_month_mask_t     month_mask;
  scheduler_action_t         action;
  scheduler_action_param_t   act_param;
} scheduler_t;
typedef enum
{
  SCHEDULER_OK = 0,
  SCHEDULER_ERR_PARAM,
  SCHEDULER_ERR_DUPLICATE,
  SCHEDULER_ERR_INTERNAL,
  SCHEDULER_ERR_NOT_FOUND,
} scheduler_ret_t;
void sheduler_init();
scheduler_ret_t scheduler_new(const scheduler_t * const p_new_scheduler);
scheduler_ret_t scheduler_modify(scheduler_t * const p_new_scheduler, uint32_t id);
#endif