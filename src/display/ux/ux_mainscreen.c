#include "ux.h"
#include "console.h"
#include "ui.h"
#include "ux_timer.h"
#include "rtc.h"
#include <string.h>
#define NODE_NAME "MAIN SCREEN"

// Private function prototype
static void        node_draw_scr();
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param);
// Node variable, will be extern in UX and other node
ux_node_t ux_mainscreen_node = 
{
  .node_name = NODE_NAME,
  .screen_update = node_draw_scr,
  .node_process = node_process
};
// Private variable
static rtc_t current_time;
// Private function
static void node_draw_scr()
{
  CONSOLE_LOG_VERBOSE("Request node %s draw screen", NODE_NAME);
  ui_screen_info_t mainscreen_info =
  {
    .mainscreen = {
      .hour   = current_time.time.hour,
      .minute = current_time.time.second
    }
  };
  ui_update_screen(UI_SCREEN_MAINSCREEN, &mainscreen_info);
}
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param)
{
  ux_node_t * p_node_ret = NULL;
  switch(evt)
  {
    case UX_EVENT_NODE_SWITCH_INTO:
      CONSOLE_LOG_DEBUG("Switch into %s node", NODE_NAME);
      break;
    case UX_EVENT_TIME_UPDATE:
      memcpy(&current_time, &(p_evt_param->time_update.new_time), sizeof(rtc_t));
      p_node_ret = &ux_mainscreen_node;
      break;
    default:
      break;
  }
  return p_node_ret;
}