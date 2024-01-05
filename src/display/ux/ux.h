#ifndef _UX_H_
#define _UX_H_
#include <stdint.h>
#include "user_input.h"


typedef enum
{
  UX_EVENT_STARTUP = 0,
  UX_EVENT_TIMER_FIRED,
  UX_EVENT_NODE_SWITCH_INTO,
  UX_EVENT_BUTTON,
  NUM_OF_UX_EVENT
} ux_event_t;
typedef union
{
  struct{
    const char * timer_name;
  } evt_timer;
  struct{
    user_input_button_t button;
    user_input_button_evt_t evt;
  } button;
} ux_evt_param_t;

typedef struct ux_node_t
{
  const char * node_name;  // Name of ux node
  void (*screen_update)(void); // Function request node to update screen
  struct ux_node_t* (*node_process)(ux_event_t, const ux_evt_param_t *);
} ux_node_t;
void ux_init();
void ux_send_event(ux_event_t evt, const ux_evt_param_t * p_evt_param);
#endif