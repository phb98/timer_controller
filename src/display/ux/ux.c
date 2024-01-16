#include "ux.h"
#include "console.h"
#include "ux_timer.h"
#include "display.h"
#include "ux_time.h"
// List of all node
extern ux_node_t ux_startup_node;

typedef struct
{
  ux_event_t       evt;
  ux_evt_param_t * p_param;
} ux_msg_t;
// Private variable
static ux_node_t * p_current_node = &ux_startup_node;

static void ux_process_event(void * p_display_msg);
// Public functions
void ux_init()
{
  ux_timer_init();
  ux_time_init();
  CONSOLE_LOG_INFO("UX init done");
}

void ux_send_event(ux_event_t evt, const ux_evt_param_t * p_evt_param)
{
  CONSOLE_LOG_DEBUG("Received ux event:%d", evt);
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
  display_thread_post_msg(disp_msg);

}

static void ux_process_event(void * p_display_msg)
{
  ux_msg_t * p_ux_msg = (ux_msg_t *)p_display_msg;
  ux_node_t * p_next_node = p_current_node->node_process(p_ux_msg->evt, (p_ux_msg->p_param));
  // Free the allocated memory
  if(!p_ux_msg->p_param) display_mem_free(p_ux_msg->p_param);
  if(p_next_node != NULL)
  {
    if(p_current_node != p_next_node)
    {
      p_current_node = p_next_node;
      // Inform the next node we will switch into it
      p_current_node->node_process(UX_EVENT_NODE_SWITCH_INTO, NULL);
    }
    // Update the screen
    p_current_node->screen_update();
  }
}