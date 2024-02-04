#include "ui.h"
#include "ui_draw_list.h"
#include "console.h"
#include <string.h>
#define NUM_ITEM_PER_SCREEN (6)
#define TEXT_FONT           (u8g2_font_6x10_tf) // Must use transparent font
#define TEXT_HEIGHT         (7) // detail here https://github.com/olikraus/u8g2/wiki/fntlist8
#define TEXT_START_X        (0)
#define TEXT_START_Y        (TEXT_HEIGHT + 5);
#define TEXT_GAP            (3)
void ui_draw_list_screen(u8g2_t * const p_u8g2,  const ui_draw_list_t * p_list_info)
{
  ASSERT_LOG_ERROR_RETURN((p_u8g2) && (p_list_info) && ((p_list_info->num_item) > (p_list_info->highlight_idx)), "Invalid param");
  uint16_t screen_page    = p_list_info->highlight_idx / NUM_ITEM_PER_SCREEN;
  uint16_t text_pos_y     = TEXT_START_Y;
  uint16_t draw_idx       = screen_page * NUM_ITEM_PER_SCREEN;
  const char * p_item_text = (p_list_info->item_names)[draw_idx];
  const char * p_item_val  = (p_list_info->item_values)[draw_idx];
  char line_buffer[32] = {0};
  u8g2_ClearBuffer(p_u8g2);
  u8g2_SetFont(p_u8g2, TEXT_FONT);
  u8g2_SetFontMode(p_u8g2, 1); // Transparent since it looks better
  u8g2_SetDrawColor(p_u8g2, 1);
  for(int i = 0; i < NUM_ITEM_PER_SCREEN; i++)
  { 
    if(strlen(p_item_text) == 0 || strlen(p_item_val) == 0)
    {
      snprintf(line_buffer, sizeof(line_buffer), "-%s", strlen(p_item_text) == 0 ? p_item_val : p_item_text);
    }
    else
    {
      snprintf(line_buffer, sizeof(line_buffer), "-%s:%s", p_item_text, p_item_val);
    }
    CONSOLE_LOG_VERBOSE("Text List:%s", line_buffer);
    if(draw_idx == p_list_info->highlight_idx)
    {
      if(p_list_info->is_selected)
      {
        uint16_t text_val_width = u8g2_GetStrWidth(p_u8g2, p_item_val) + 1;
        // u8g2_SetDrawColor(p_u8g2, 0);
        // u8g2_DrawBox(p_u8g2, 0, text_pos_y - (TEXT_HEIGHT) - 1, u8g2_GetDisplayWidth(p_u8g2), TEXT_HEIGHT + 2);
        // u8g2_SetDrawColor(p_u8g2, 1);
        // Draw a box from begin of text value string to end of text value string
        u8g2_DrawBox(p_u8g2, u8g2_GetStrWidth(p_u8g2, line_buffer) - text_val_width, text_pos_y - (TEXT_HEIGHT) - 1, text_val_width + 1, TEXT_HEIGHT + 2);
      }
      else
      {
        u8g2_DrawBox(p_u8g2, 0, text_pos_y - (TEXT_HEIGHT) - 1, u8g2_GetDisplayWidth(p_u8g2), TEXT_HEIGHT + 2);
      }
      u8g2_SetDrawColor(p_u8g2, 2);
      u8g2_DrawStr(p_u8g2, TEXT_START_X, text_pos_y, line_buffer);
      u8g2_SetDrawColor(p_u8g2, 1);
    }
    else u8g2_DrawStr(p_u8g2, TEXT_START_X, text_pos_y, line_buffer);
    if(++draw_idx >= p_list_info->num_item) break;
    text_pos_y += TEXT_GAP + TEXT_HEIGHT;
    p_item_text = (p_list_info->item_names)[draw_idx];
    p_item_val  = (p_list_info->item_values)[draw_idx];
  }
  u8g2_SetFontMode(p_u8g2, 0);
  u8g2_SetDrawColor(p_u8g2, 1);
}