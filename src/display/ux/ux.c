#include "ux.h"
#include "console.h"
#include "ux_timer.h"
#include "display.h"
#include "ux_time.h"
#include "ux_timer.h"

// List of all node
extern ux_node_t ux_startup_node;

typedef struct
{
  ux_event_t       evt;
  ux_evt_param_t * p_param;
} ux_msg_t;
// Private variable
static ux_node_t * p_current_node = &ux_startup_node;

static void         ux_process_event(void * p_display_msg);
static const char * get_ux_event_name(const ux_event_t evt);
// Public functions
void ux_init()
{
  ux_timer_init();
  ux_time_init();
  CONSOLE_LOG_INFO("UX init done");
}

void ux_send_event(ux_event_t evt, const ux_evt_param_t * p_evt_param)
{
  CONSOLE_LOG_DEBUG("Received ux event:%s", get_ux_event_name(evt));
  if(evt >= NUM_OF_UX_EVENT)
  {
    CONSOLE_LOG_ERROR("Invalid UX event:%d", evt);
    return;
  }
  // Send new disp message to switch context to display thread
  ux_msg_t ux_msg = 
  {
    .evt = evt,
    .p_param = NULL,
  };
  if(p_evt_param)
  {
    ux_msg.p_param = display_mem_malloc(sizeof(ux_evt_param_t));
    if(ux_msg.p_param)
    {
      memcpy(ux_msg.p_param, p_evt_param, sizeof(ux_evt_param_t));
    }
    else CONSOLE_LOG_ERROR("Fail allocate memory");
  }

  display_msg_t disp_msg = 
  {
    .func = ux_process_event,
    .param_size = sizeof(ux_msg_t),
    .p_param = &ux_msg
  };
  if(false == display_thread_post_msg(disp_msg))
  {
    display_mem_free(ux_msg.p_param);
  }
}

static void ux_process_event(void * p_display_msg)
{
  ux_msg_t * p_ux_msg = (ux_msg_t *)p_display_msg;
  ux_node_t * p_next_node = p_current_node->node_process(p_ux_msg->evt, (p_ux_msg->p_param));
  // Free the allocated memory
  if(p_ux_msg->p_param) display_mem_free(p_ux_msg->p_param);
  if(p_next_node != NULL)
  {
    if(p_current_node != p_next_node)
    {
      ux_evt_param_t * p_temp_event = display_mem_malloc(sizeof(ux_evt_param_t));
      if(p_temp_event) p_temp_event->switch_into.p_previous_node = p_current_node;
      else
      {
        CONSOLE_LOG_ERROR("Fail to allocate mem");
      }
      p_current_node = p_next_node;
      // Inform the next node we will switch into it
      p_current_node->node_process(UX_EVENT_NODE_SWITCH_INTO, p_temp_event);
      if(p_temp_event) display_mem_free(p_temp_event);
    }
    // Update the screen
    p_current_node->screen_update();
  }
}

static const char * get_ux_event_name(const ux_event_t evt)
{
  static const char * evt_name[NUM_OF_UX_EVENT] = 
  {
    "UX_EVENT_STARTUP",
    "UX_EVENT_TIMER_FIRED",
    "UX_EVENT_NODE_SWITCH_INTO",
    "UX_EVENT_BUTTON",
    "UX_EVENT_TIME_UPDATE",
  };
  static const char * unknown_evt_name = "UX_EVENT_UNKNOWN";
  if(evt >= NUM_OF_UX_EVENT) return unknown_evt_name;
  return evt_name[evt];
}