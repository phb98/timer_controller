#ifndef _SHEDULER_H_
#define _SHEDULER_H_
#include "rtc.h"
#include <stdbool.h>
#include "output_controller.h"
typedef uint8_t scheduler_dow_mask_t;   // 0x00: daily, 0xFF: 1 shot, not implement for now
typedef uint16_t scheduler_month_mask_t; // 0x00: Every month, 0xffff: 1 shot, not immplement for now
typedef enum
{
  SCHEDULER_ACTION_OUTPUT = 1,
  NUM_OF_SCHEDULER_ACTION
} scheduler_action_t;
typedef union
{
  struct {
    output_ch_t   channel;
    output_val_t  val;
  }action;
} scheduler_action_param_t;
typedef struct
{
  rtc_t                      trig_time;
  scheduler_dow_mask_t       dow_mask;
  scheduler_month_mask_t     month_mask;
  scheduler_action_t         action;
  scheduler_action_param_t   act_param;
} scheduler_t;
void sheduler_init();
void scheduler_new(const scheduler_t * const p_new_scheduler);
#endif