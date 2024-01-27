#include "ux.h"
#include "console.h"
#include "ui.h"
#include "ux_timer.h"
#define NODE_NAME               "MENU"
#define NUM_MENU_ITEM           (6)
#define SCREEN_TIMEOUT_MS       (30000)
#define SCREEN_TIMEOUT_TIMER    "Menu Screen Timeout"
// Private function prototype
static void        node_draw_scr();
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param);
// Node variable, will be extern in UX and other node
ux_node_t ux_menu_node = 
{
  .node_name = NODE_NAME,
  .screen_update = node_draw_scr,
  .node_process = node_process
};
// Related node
extern ux_node_t ux_mainscreen_node;
// Private variable
static uint8_t highlight_idx;
static bool force_clear_screen = false;
static ux_node_t * p_node_list[NUM_MENU_ITEM] = 
{
  &ux_mainscreen_node,
  NULL, // Not implement right now
  NULL, // Not implement right now
  NULL, // Not implement right now
  NULL, // Not implement right now
  NULL, // Not implement right now
};
// Private function
static void node_draw_scr()
{
  CONSOLE_LOG_VERBOSE("Request node %s draw screen", NODE_NAME);
  static ui_screen_info_t screen_info;
  screen_info.menu.highlight_idx = highlight_idx;
  ui_update_screen(UI_SCREEN_MENU, &screen_info, force_clear_screen);
  force_clear_screen = false;
  // Restart timeout timer when refresh screen
  ux_timer_start(SCREEN_TIMEOUT_TIMER, SCREEN_TIMEOUT_MS);
}
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param)
{
  ux_node_t * p_node_ret = NULL;
  switch(evt)
  {
    case UX_EVENT_NODE_SWITCH_INTO:
    {
      CONSOLE_LOG_INFO("Switch into node %s", NODE_NAME);
      force_clear_screen = true;
      // If we come from mainscreen, always start at home icon
      if(p_evt_param->switch_into.p_previous_node == &ux_mainscreen_node)
      {
        highlight_idx = 0;
      }
      break;
    }
    case UX_EVENT_BUTTON:
    {
      if(p_evt_param->button.button == BUTTON_ENTER)
      {
        if(p_evt_param->button.evt == BUTTON_EVT_CLICK)
        {
          p_node_ret = p_node_list[highlight_idx];
        }
        else if(p_evt_param->button.evt  == BUTTON_EVT_LONG_PRESS)
        {
          // Long press enter, back to main screen
          p_node_ret = &ux_mainscreen_node;
        }
      }
      else if(p_evt_param->button.button == BUTTON_UP && p_evt_param->button.evt == BUTTON_EVT_CLICK)
      {
        highlight_idx = (highlight_idx + 1) % NUM_MENU_ITEM;
        p_node_ret = &ux_menu_node;
      }
      else if(p_evt_param->button.button == BUTTON_DOWN && p_evt_param->button.evt == BUTTON_EVT_CLICK)
      {
        highlight_idx = (highlight_idx == 0 ? NUM_MENU_ITEM - 1 : highlight_idx - 1);
        p_node_ret = &ux_menu_node;
      }
      break;
    }
    case UX_EVENT_TIMER_FIRED:
    {
      if(strcmp(p_evt_param->evt_timer.timer_name, SCREEN_TIMEOUT_TIMER) == 0)
      {
        // Switch back to main screen node
        p_node_ret = &ux_mainscreen_node;
      }
    }
    default:
      break;
  }
  return p_node_ret;
}