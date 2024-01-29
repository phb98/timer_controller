#ifndef _UI_DRAW_LIST_H_
#define _UI_DRAW_LIST_H_
#include <stdint.h>
#include "u8g2.h"
typedef struct 
{
  uint16_t num_item;
  uint16_t highlight_idx;
  bool     is_selected;
  const char ** item_names;
  const char ** item_values;
} ui_draw_list_t;
void ui_draw_list_screen(u8g2_t * const p_u8g2,  const ui_draw_list_t * p_list_info);
#endif