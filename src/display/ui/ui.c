#include "ui.h"
#include "console.h"
#include "u8g2.h"
#include "display.h"
#include "bitmap_icons/icAlarm.h"
#include "bitmap_icons/icon24_24.h"
#include <string.h>
#include "sys_config.h"
#include "ui_draw_list.h"
// Typedef
typedef void (*ui_screen_draw_t)(u8g2_t * const, const ui_screen_info_t *);
// List of all using screen
static void ui_draw_startup_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info);
static void ui_draw_mainscreen_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info);
static void ui_draw_menu_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info);
static void ui_draw_adjust_time_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info);
const ui_screen_draw_t ui_screen_draw[NUM_OF_UI_SCREEN] = 
{
  [UI_SCREEN_STARTUP]     = ui_draw_startup_screen,
  [UI_SCREEN_MAINSCREEN]  = ui_draw_mainscreen_screen,
  [UI_SCREEN_MENU]        = ui_draw_menu_screen,
  [UI_SCREEN_ADJUST_TIME] = ui_draw_adjust_time_screen,
};
// Public functions

/**
 * This function is called from UX to update the screen
*/
void ui_update_screen(ui_screen_t screen, const ui_screen_info_t * p_screen_info, bool force_clear)
{
  u8g2_t * p_u8g2 = display_get_u8g2_handle();
  if(!p_u8g2)
  {
    CONSOLE_LOG_ERROR("Fail to get u8g2 handle");
    return;
  }
  if(force_clear)
  {
    u8g2_ClearDisplay(p_u8g2);
  }
  if(screen > NUM_OF_UI_SCREEN)
  {
    CONSOLE_LOG_ERROR("Invalid screen request");
    return;
  }
  CONSOLE_LOG_DEBUG("Request update ui screen:%d", screen);
  // Draw the screen
  if(ui_screen_draw[screen])
  {
    ui_screen_draw[screen](p_u8g2, p_screen_info);
  }
  else
  {
    CONSOLE_LOG_WARN("Request draw screen not exist in UI:%d", screen);
  }
  u8g2_SendBuffer(p_u8g2);
}




// Private functions

static void ui_draw_startup_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info)
{
  CONSOLE_LOG_VERBOSE("Draw startup screen");
  char text_buffer[64];
  const char FONT_HEIGHT = 10;
  const char TEXT_X = 44;
  const char TEXT_Y = 25;

  u8g2_DrawXBM(p_u8g2, 5, 17, 32, 32, icAlarm_bits);
  u8g2_SetFont(p_u8g2, u8g2_font_spleen5x8_mf );
  sniprintf(text_buffer, sizeof(text_buffer), "%s", "Timer Controller");
  u8g2_DrawStr(p_u8g2, TEXT_X, 25, text_buffer);
  sniprintf(text_buffer, sizeof(text_buffer), "Version:%d.%d", CONFIG_MAJOR_VER, CONFIG_MINOR_VER);
  u8g2_DrawStr(p_u8g2, TEXT_X, TEXT_Y + FONT_HEIGHT, text_buffer);
  sniprintf(text_buffer, sizeof(text_buffer), "Project by PHB");
  u8g2_DrawStr(p_u8g2, TEXT_X, TEXT_Y + 2*FONT_HEIGHT, text_buffer);  
}

static void ui_draw_mainscreen_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info)
{
  if(!p_screen_info)
  {
    CONSOLE_LOG_ERROR("Can not draw mainscreen without info");
    return;
  }
  char clock_str[16];
  const rtc_t * p_rtc = &(p_screen_info->mainscreen.current_time);
  snprintf(clock_str, sizeof(clock_str), "%02d%c%02d", p_rtc->time.hour, (p_rtc->time.second % 2) ? ':' : ' ', 
                                                      p_rtc->time.minute);
  u8g2_ClearBuffer(p_u8g2);
  // Draw the main clock
  u8g2_SetFont(p_u8g2, u8g2_font_VCR_OSD_tf);
  u8g2_DrawStr(p_u8g2, 32, 39, clock_str);
}

static void ui_draw_menu_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info)
{
  #define NUM_MENU    (6)
  #define ICON_W      (24)
  #define ICON_H      (24)
  #define START_X     (20)
  #define X_GAP       (8)
  #define START_Y     (13)
  #define Y_GAP       (2)
  #define X_ALIGN_CENTER(t) ((128 - (u8g2_GetUTF8Width(p_u8g2, t))) / 2)
  if(!p_screen_info)
  {
    CONSOLE_LOG_ERROR("Can not draw menuscreen without info");
    return;
  }
  
  uint8_t x_pos = START_X;
  uint8_t y_pos = START_Y;
  const uint8_t * p_icon_menu[] = {icHome_24,     icAlarm_24,       icClock_24,    icList_24,    icManual_24,     icSetting_24};
  const char *    p_text_menu[] = {"Main Screen", "Add/Edit Timer", "Adjust Time", "Timer List", "Manual Output", "Setting"};
  uint8_t temp = 0;
  uint8_t highlight_idx = p_screen_info->menu.highlight_idx;
  u8g2_ClearBuffer(p_u8g2);
  u8g2_SetDrawColor(p_u8g2, 1); // Normal color
  // Draw an icon grid of 2 row x 3 col
  for(int row = 0; row < 2; row++)
  {
    for(int col = 0; col < 3; col++)
    {
      if(temp == highlight_idx)
      {
        u8g2_SetDrawColor(p_u8g2, 0);
        u8g2_DrawXBM(p_u8g2, x_pos, y_pos, ICON_W, ICON_H, p_icon_menu[temp++]);
        u8g2_SetDrawColor(p_u8g2, 1);
      }
      else
      {
        u8g2_DrawXBM(p_u8g2, x_pos, y_pos, ICON_W, ICON_H, p_icon_menu[temp++]);
      }
      x_pos += ICON_W + X_GAP;
    }
    x_pos = START_X; // New row, Reset to begin of X position
    y_pos += ICON_H + Y_GAP; // Advance to next row Y position
  }
  // Draw menu text
  u8g2_SetFont(p_u8g2, u8g2_font_6x12_tf);
  u8g2_DrawStr(p_u8g2, X_ALIGN_CENTER(p_text_menu[highlight_idx]), 11, p_text_menu[highlight_idx]);
}

static void ui_draw_adjust_time_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info)
{
  #define VAL_STR_LENGTH (32)
  ASSERT_LOG_ERROR_RETURN(p_screen_info, "Invalid Param");
  const char * time_name_str[] = {"Year", "Month", "Day", "DoW", "Hour", "Minute", "Second"};
  char * p_val_str[7];
  void * p_val_str_mem = display_mem_malloc(7 * VAL_STR_LENGTH); // 7 item 
  const rtc_t * p_rtc = &(p_screen_info->adjust_time.current_time);
  ASSERT_LOG_ERROR_RETURN(p_val_str_mem, "Can not allocate mem");
  for(int i = 0; i < 7; i++)
  {
    (p_val_str)[i] = (uint8_t*)p_val_str_mem + VAL_STR_LENGTH * i;
    memset(p_val_str[i], 0x0, VAL_STR_LENGTH);
  }
  // Print string to buffer to display
  snprintf(p_val_str[0], VAL_STR_LENGTH, "%d", p_rtc->date.year);
  snprintf(p_val_str[1], VAL_STR_LENGTH, "%d", p_rtc->date.month);
  snprintf(p_val_str[2], VAL_STR_LENGTH, "%d", p_rtc->date.day);
  snprintf(p_val_str[3], VAL_STR_LENGTH, "%s", rtc_get_DoW_string(p_rtc->date.DoW, true));
  snprintf(p_val_str[4], VAL_STR_LENGTH, "%d", p_rtc->time.hour);
  snprintf(p_val_str[5], VAL_STR_LENGTH, "%d", p_rtc->time.minute);
  snprintf(p_val_str[6], VAL_STR_LENGTH, "%d", p_rtc->time.second);
  ui_draw_list_t list_config = 
  {
    .num_item = 7,
    .highlight_idx = p_screen_info->adjust_time.highlight_idx,
    .item_names = time_name_str,
    .item_values = (const char **)p_val_str,
    .is_selected = p_screen_info->adjust_time.is_selected,
  };
  ui_draw_list_screen(p_u8g2, &list_config);
  display_mem_free(p_val_str_mem);
}