#ifndef _UI_H_
#define _UI_H_
#include <stdint.h>
#include <stdbool.h>
#include "rtc.h"
#include "scheduler.h"
typedef enum
{
  UI_SCREEN_STARTUP,
  UI_SCREEN_MAINSCREEN,
  UI_SCREEN_MENU,
  UI_SCREEN_ADJUST_TIME,
  UI_SCREEN_ADJUST_TIMER,
  NUM_OF_UI_SCREEN
} ui_screen_t;
typedef union
{
  struct
  {
    rtc_t current_time;
  } mainscreen;
  struct
  {
    uint8_t highlight_idx;
  } menu;
  struct
  {
    rtc_t current_time;
    uint8_t highlight_idx;
    bool is_selected;
  } adjust_time;
  struct
  {
    uint8_t ux_current_state;
    uint32_t chosen_output;
    const scheduler_t * p_timer_list;
    uint32_t num_timers_listed;
    uint32_t highlight_idx;
    const scheduler_t * p_edit_timer;
    bool is_editing_timer_param;
    bool is_adding_timer;
  } edit_timer;
} ui_screen_info_t;
void ui_update_screen(ui_screen_t screen, const ui_screen_info_t * p_screen_info, bool force_clear);
#endif