#ifndef _UI_H_
#define _UI_H_
#include <stdint.h>
#include <stdbool.h>
typedef enum
{
  UI_SCREEN_STARTUP,
  UI_SCREEN_MAINSCREEN,
  NUM_OF_UI_SCREEN
} ui_screen_t;
typedef union
{
  struct
  {
    uint8_t hour;
    uint8_t minute;
    uint8_t second;
  } mainscreen;
} ui_screen_info_t;
void ui_update_screen(ui_screen_t screen, const ui_screen_info_t * p_screen_info, bool force_clear);
#endif