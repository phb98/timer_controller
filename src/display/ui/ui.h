#ifndef _UI_H_
#define _UI_H_
#include <stdint.h>
#include <stdbool.h>
#include "rtc.h"
typedef enum
{
  UI_SCREEN_STARTUP,
  UI_SCREEN_MAINSCREEN,
  UI_SCREEN_MENU,
  UI_SCREEN_ADJUST_TIME,
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
} ui_screen_info_t;
void ui_update_screen(ui_screen_t screen, const ui_screen_info_t * p_screen_info, bool force_clear);
#endif