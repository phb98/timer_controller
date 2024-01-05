#include "ui.h"
#include "console.h"
#include "u8g2.h"
#include "display.h"
#include "bitmap_icons/icAlarm.h"
#include <string.h>
#include "sys_config.h"
// Typedef
typedef void (*ui_screen_draw_t)(u8g2_t * const, const ui_screen_info_t *);
// List of all using screen
static void ui_draw_startup_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info);
static void ui_draw_mainscreen_screen(u8g2_t * const p_u8g2,  const ui_screen_info_t * p_screen_info);

const ui_screen_draw_t ui_screen_draw[NUM_OF_UI_SCREEN] = 
{
  [UI_SCREEN_STARTUP] = ui_draw_startup_screen,
  [UI_SCREEN_MAINSCREEN] = ui_draw_mainscreen_screen,
};
// Public functions

/**
 * This function is called from UX to update the screen
*/
void ui_update_screen(ui_screen_t screen, const ui_screen_info_t * p_screen_info)
{
  u8g2_t * p_u8g2 = display_get_u8g2_handle();
  if(!p_u8g2)
  {
    CONSOLE_LOG_ERROR("Fail to get u8g2 handle");
    return;
  }
  if(screen > NUM_OF_UI_SCREEN)
  {
    CONSOLE_LOG_ERROR("Invalid screen request");
    return;
  }
  CONSOLE_LOG_DEBUG("Request update ui screen:%d", screen);
  // Draw the screen
  ui_screen_draw[screen](p_u8g2, p_screen_info);
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
  u8g2_ClearDisplay(p_u8g2);
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
  snprintf(clock_str, sizeof(clock_str), "%02d:%02d", p_screen_info->mainscreen.hour, 
                                                      p_screen_info->mainscreen.minute);
  u8g2_ClearDisplay(p_u8g2);
  // Draw the main clock
  u8g2_SetFont(p_u8g2, u8g2_font_fub30_tf);
  u8g2_DrawStr(p_u8g2, 12, 42, clock_str);
  u8g2_SendBuffer(p_u8g2);
}