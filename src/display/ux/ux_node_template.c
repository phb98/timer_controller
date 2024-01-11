/**
 * THIS FILE IS ONLY TEMPLATE FOR OTHER UX NODE
 * THIS FILE IS NOT SUPPOSED TO BE BUILT
*/
#include "ux.h"
#include "console.h"
#include "ui.h"
#include "ux_timer.h"
#define NODE_NAME "NODE_TEMPLATE"

// Private function prototype
static void        node_draw_scr();
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param);
// Node variable, will be extern in UX and other node
ux_node_t ux_template_node = 
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
}
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param)
{
  ux_node_t * p_node_ret = NULL;
  switch(evt)
  {
    default:
      break;
  }
  return p_node_ret;
}