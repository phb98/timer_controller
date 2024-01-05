#include "ux.h"
#include "console.h"
#include "ui.h"
#include "ux_timer.h"
#define NODE_NAME "START UP"

// List of other node this node will interract with
extern ux_node_t ux_mainscreen_node;
// Private function prototype
static void node_draw_scr();
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param);
// Node variable, will be extern in UX and other node
ux_node_t ux_startup_node = 
{
  .node_name = NODE_NAME,
  .screen_update = node_draw_scr,
  .node_process = node_process
};
// Private variable
// Private function
static void node_draw_scr()
{
  CONSOLE_LOG_VERBOSE("Request node %s draw screen", NODE_NAME);
  ui_update_screen(UI_SCREEN_STARTUP, NULL);
}
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param)
{
  ux_node_t * p_node_ret = NULL;
  switch(evt)
  {
    case UX_EVENT_STARTUP:
      // Return our node to force screen update
      p_node_ret = &ux_startup_node;
      ux_timer_start("Boot_load_timer", 5000);
      break;
    case UX_EVENT_TIMER_FIRED:
      CONSOLE_LOG_VERBOSE("Receive ux timer fired:%s", p_evt_param->evt_timer.timer_name);
      // Check if this is our timer
      if(strcmp(p_evt_param->evt_timer.timer_name, "Boot_load_timer") == 0)
      {
        // Switch to main screen node
        CONSOLE_LOG_DEBUG("Switch from startup to main screen");
        p_node_ret = &ux_mainscreen_node;
      }
      break;
    default:
      break;
  }
  return p_node_ret;
}