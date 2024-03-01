/**
 * THIS FILE IS ONLY TEMPLATE FOR OTHER UX NODE
 * THIS FILE IS NOT SUPPOSED TO BE BUILT
*/
#include "ux.h"
#include "console.h"
#include "ui.h"
#include "ux_timer.h"
#include "ux_utility.h"
#include "sys_config.h"
#include "scheduler.h"
#include "scheduler_output.h"
#define NODE_NAME "ADJUST TIMER"
#define NODE_SELF() (&ux_adjust_timer_node)
#define NUM_EDIT_TIMER  (6)
#define IS_ENTER_CLICK(param) (param->button.button == BUTTON_ENTER && param->button.evt == BUTTON_EVT_CLICK)
#define IS_ENTER_LONG_PRESS(param) (param->button.button == BUTTON_ENTER && param->button.evt == BUTTON_EVT_LONG_PRESS)
#define IS_UP_CLICK(param) (param->button.button == BUTTON_UP && param->button.evt == BUTTON_EVT_CLICK)
#define IS_DOWN_CLICK(param) (param->button.button == BUTTON_DOWN && param->button.evt == BUTTON_EVT_CLICK)

typedef enum 
{
  SETTING_TRIG_HOUR,
  SETTING_TRIG_MINUTE,
  SETTING_TRIG_SECOND,
  SETTING_DOW_MASK,
  SETTING_MONTH_MASK,
  SETTING_OUTPUT_VALUE,
  NUM_SETTING_TIMER,
}timer_setting_t;
// Private function prototype
static void        node_draw_scr();
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param);
static void        remap_scheduler_output_value(scheduler_t * const p_sch, bool map_sys_to_ui);
static ux_node_t * node_sm_process(ux_event_t evt, const ux_evt_param_t * p_evt_param);
static void        single_setting_timer(scheduler_t * const p_timer, timer_setting_t setting, int32_t *p_value, bool is_get);
// Node variable, will be extern in UX and other node
ux_node_t ux_adjust_timer_node = 
{
  .node_name = NODE_NAME,
  .screen_update = node_draw_scr,
  .node_process = node_process
};
// Relating node
extern ux_node_t ux_menu_node;
extern ux_node_t ux_mainscreen_node;
// Private variable
static enum 
{
  CHOOSING_OUTPUT,
  LIST_TIMER,
  EDIT_TIMER,
} current_state;
static ux_utility_adjust_var_wrap_t wrap_config;
static bool force_clear_screen = false;
static ui_screen_info_t screen_info;
static ux_node_t * p_previous_node;
static scheduler_t edit_timers[NUM_EDIT_TIMER];
static scheduler_t editing_timer;
static uint32_t choosing_output = 0;
static int32_t  choosing_timer  = 0; // -1: New timer
static uint32_t num_timer_queried = NUM_EDIT_TIMER;
static uint32_t *p_highlight_idx = 0;
static uint32_t list_timer_highlight_idx;
static uint32_t edit_timer_highlight_idx;
static uint32_t query_idx = 0;
static bool     is_editing_timer = false;
static bool     timer_modified   = false;
// Private function

static void node_draw_scr()
{
  CONSOLE_LOG_VERBOSE("Request node %s draw screen", NODE_NAME);
  screen_info.edit_timer.ux_current_state = current_state;
  switch(current_state)
  {
    case CHOOSING_OUTPUT:
      screen_info.edit_timer.chosen_output = choosing_output;
      break;
    case LIST_TIMER:
      p_highlight_idx = &list_timer_highlight_idx;
      screen_info.edit_timer.chosen_output = choosing_output;
      screen_info.edit_timer.num_timers_listed = num_timer_queried;
      for(int i = 0; i < num_timer_queried; i++)
      {
        remap_scheduler_output_value(&edit_timers[i], true);
      }
      screen_info.edit_timer.p_timer_list = edit_timers;
      screen_info.edit_timer.highlight_idx = (*p_highlight_idx) % NUM_EDIT_TIMER;
      break;
    case EDIT_TIMER:
      p_highlight_idx = &edit_timer_highlight_idx;
      screen_info.edit_timer.p_edit_timer  = &editing_timer;
      screen_info.edit_timer.highlight_idx = (*p_highlight_idx);
      screen_info.edit_timer.is_editing_timer_param    = is_editing_timer;
      screen_info.edit_timer.is_adding_timer = (choosing_timer == -1) ? true : false;
      break;
    default:
      break;
  }
  ui_update_screen(UI_SCREEN_ADJUST_TIMER, &screen_info, force_clear_screen);
  force_clear_screen = false;
}
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param)
{
  ux_node_t * p_node_ret = NULL;
  switch(evt)
  {
    case UX_EVENT_NODE_SWITCH_INTO:
    {
      CONSOLE_LOG_INFO("Switch into node %s", NODE_NAME);
      p_previous_node = p_evt_param->switch_into.p_previous_node;
      if(p_previous_node == &ux_menu_node)
      {
        current_state = CHOOSING_OUTPUT;
      }
      force_clear_screen = true;
      break;
    }
    case UX_EVENT_BUTTON:
      p_node_ret = node_sm_process(evt, p_evt_param);
      break;
    default:
      break;
  }
  return p_node_ret;
}

static ux_node_t * node_sm_process(ux_event_t evt, const ux_evt_param_t * p_evt_param)
{
  ux_node_t *p_node_ret = NULL;
  switch(current_state)
  {
    case CHOOSING_OUTPUT:
      if(evt == UX_EVENT_BUTTON)
      {
        if(IS_ENTER_LONG_PRESS(p_evt_param))
        {
          p_node_ret = p_previous_node;
          break;
        }
        if(IS_UP_CLICK(p_evt_param) || IS_DOWN_CLICK(p_evt_param))
        {
          wrap_config.lower_bound = 0;
          wrap_config.upper_bound = CONFIG_OUTPUT_TOTAL_CH - 1;
          wrap_config.low_wrap = false;
          wrap_config.up_wrap = false;
          choosing_output = ux_utility_adjust_var_wrapping(&wrap_config, choosing_output, IS_UP_CLICK(p_evt_param) ? 1 : -1);
          p_node_ret = NODE_SELF();
          break;
        }
        if(IS_ENTER_CLICK(p_evt_param))
        {
          num_timer_queried = NUM_EDIT_TIMER;
          memset(edit_timers, 0x0, sizeof(edit_timers));
          query_idx = 0;
          if(SCHEDULER_OK == scheduler_output_get_by_channel(choosing_output, query_idx, edit_timers, &num_timer_queried))
          {
            list_timer_highlight_idx = 0;
            current_state = LIST_TIMER;
            p_node_ret = NODE_SELF();
            force_clear_screen = true;
          }
          else
          {
            CONSOLE_LOG_ERROR("Fail to query times for channel:%d, return main screen", choosing_output);
            p_node_ret = &ux_mainscreen_node;
          }
          break;
        }
      }
      break;
    case LIST_TIMER:
    {
      p_highlight_idx = &list_timer_highlight_idx;
      if(evt == UX_EVENT_BUTTON)
      {
        if(IS_ENTER_LONG_PRESS(p_evt_param))
        {
          p_node_ret = p_previous_node;
          break;
        }
        if(IS_UP_CLICK(p_evt_param))
        {
          p_node_ret = NODE_SELF();
          list_timer_highlight_idx++;
          if(list_timer_highlight_idx == num_timer_queried)
          {
            if(num_timer_queried == NUM_EDIT_TIMER)
            {
              // there are still timer left to query, query new list
              query_idx += NUM_EDIT_TIMER;
              memset(edit_timers, 0x0, sizeof(edit_timers));
              if(SCHEDULER_OK != scheduler_output_get_by_channel(choosing_output, query_idx, edit_timers, &num_timer_queried))
              {
                CONSOLE_LOG_ERROR("Query fail");
                p_node_ret = &ux_mainscreen_node;
                break;
              }
              list_timer_highlight_idx = 0; // wrap over
            }
          }
          break;
        }
        if(IS_DOWN_CLICK(p_evt_param))
        {
          p_node_ret = NODE_SELF();
          if(list_timer_highlight_idx == 0)
          {
            if(query_idx >= NUM_EDIT_TIMER)
            {
              query_idx -= NUM_EDIT_TIMER;
              num_timer_queried = NUM_EDIT_TIMER;
              if(SCHEDULER_OK != scheduler_output_get_by_channel(choosing_output, query_idx, edit_timers, &num_timer_queried))
              {
                CONSOLE_LOG_ERROR("Query fail");
                p_node_ret = &ux_mainscreen_node;
                break;
              }
              list_timer_highlight_idx = num_timer_queried - 1; // wrap over, but dont wrap if on top of list
            }
          }
          else list_timer_highlight_idx--;
        }
        if(IS_ENTER_CLICK(p_evt_param))
        {
          // Check what timer we want to edit
          if((list_timer_highlight_idx == (num_timer_queried)) && num_timer_queried < NUM_EDIT_TIMER)
          {
            // We want to add new timer
            CONSOLE_LOG_INFO("Begin adding new timer");
            choosing_timer = -1;
            memset(&editing_timer, 0x0, sizeof(scheduler_t));
            editing_timer.act_param.output.channel = choosing_output;
          }
          else
          {
            choosing_timer = edit_timers[list_timer_highlight_idx].id;
            memcpy(&editing_timer, &edit_timers[list_timer_highlight_idx], sizeof(scheduler_t));
            remap_scheduler_output_value(&editing_timer, true); // remap output value for easy edit
            is_editing_timer = false;
            CONSOLE_LOG_INFO("Begin editting timer:%d", choosing_timer);
          }
          is_editing_timer = false;
          edit_timer_highlight_idx = 0;
          timer_modified = false;
          current_state = EDIT_TIMER;
          p_node_ret = NODE_SELF();
          break;
        }
      }
      break;
    }
    case EDIT_TIMER:
    {
      p_highlight_idx = &edit_timer_highlight_idx;
      if(evt == UX_EVENT_BUTTON)
      {
        if(IS_ENTER_LONG_PRESS(p_evt_param))
        {
          if(choosing_timer == -1)
          {
            if(timer_modified) 
            {
              remap_scheduler_output_value(&editing_timer, false); // remap output value for system
              scheduler_new(&editing_timer); // Add new timer
            }
          }
          else if(choosing_timer != -1 && edit_timer_highlight_idx != NUM_SETTING_TIMER)
          {
            if(timer_modified) 
            {
              remap_scheduler_output_value(&editing_timer, false); // remap output value for system
              scheduler_modify(&editing_timer, choosing_timer);
            }
          }
          num_timer_queried = NUM_EDIT_TIMER;
          if(SCHEDULER_OK != scheduler_output_get_by_channel(choosing_output, query_idx, edit_timers, &num_timer_queried))
          {
            CONSOLE_LOG_ERROR("Query fail");
            p_node_ret = &ux_mainscreen_node;
            break;
          }
          current_state = LIST_TIMER;
          timer_modified = false;
          p_node_ret = NODE_SELF();
          break;
        }
        if(IS_ENTER_CLICK(p_evt_param))
        {
          timer_modified = true;
          if((choosing_timer != - 1 && edit_timer_highlight_idx != NUM_SETTING_TIMER) || (choosing_timer == -1))
          {
            is_editing_timer = !is_editing_timer;
          }
          else if(choosing_timer != -1 && edit_timer_highlight_idx == NUM_SETTING_TIMER)
          {
            scheduler_modify(NULL, choosing_timer); // Delete timer
            num_timer_queried = NUM_EDIT_TIMER;
            if(SCHEDULER_OK != scheduler_output_get_by_channel(choosing_output, query_idx, edit_timers, &num_timer_queried))
            {
              CONSOLE_LOG_ERROR("Query fail");
              p_node_ret = &ux_mainscreen_node;
              break;
            }
            current_state = LIST_TIMER;
            p_node_ret = NODE_SELF();
            break;
          }
          p_node_ret = NODE_SELF();
        }
        if(IS_UP_CLICK(p_evt_param) || IS_DOWN_CLICK(p_evt_param)) 
        {
          timer_modified = true;
          if(!is_editing_timer)
          {
            // Change cursor position
            wrap_config.low_wrap = false;
            wrap_config.up_wrap  = false;
            wrap_config.lower_bound = 0;
            wrap_config.upper_bound = choosing_timer == - 1 ? NUM_SETTING_TIMER - 1: NUM_SETTING_TIMER; // EDITING TIMER WILL HAVE 1 more option: Delete
            edit_timer_highlight_idx = ux_utility_adjust_var_wrapping(&wrap_config, edit_timer_highlight_idx, IS_UP_CLICK(p_evt_param) ? 1 : -1);
            p_node_ret = NODE_SELF();
          }
          else
          {
            wrap_config.low_wrap = true;
            wrap_config.up_wrap  = true;
            wrap_config.lower_bound = 0;
            const int up_wrap_config[NUM_SETTING_TIMER] = {
              23, 59, 59, 0xff, 0xfff, 100 // 100%, will be remap when scheduler trigger
            };
            wrap_config.upper_bound = up_wrap_config[edit_timer_highlight_idx % NUM_SETTING_TIMER];
            int32_t temp_set_val;
            int32_t val_step = 1;
            if(edit_timer_highlight_idx % NUM_SETTING_TIMER == SETTING_OUTPUT_VALUE)
            {
              // When changing output value for binary output, there are only 2 possible value: 0 and 100
              val_step = (output_controller_get_type(editing_timer.act_param.output.channel) == OUTPUT_TYPE_BINARY) ? 100 : 1;
            }
            val_step *= IS_UP_CLICK(p_evt_param) ? 1 : -1;
            single_setting_timer(&editing_timer, edit_timer_highlight_idx, &temp_set_val, true); // Get current value
            temp_set_val = ux_utility_adjust_var_wrapping(&wrap_config, temp_set_val, val_step);
            single_setting_timer(&editing_timer, edit_timer_highlight_idx, &temp_set_val, false);// Set new value
            p_node_ret = NODE_SELF();
          }
          break;
        }
      }
      break;
    }
    default:
      CONSOLE_LOG_WARN("Unknown current state:%d", current_state);
      break;
  }
  return p_node_ret;
}


static void save_timer(const scheduler_t * p_timer, int32_t sch_idx)
{

}

static void single_setting_timer(scheduler_t * const p_timer, timer_setting_t setting, int32_t *p_value, bool is_get)
{
  ASSERT_LOG_ERROR_RETURN(p_timer && p_value, "Invalid Param");
  switch(setting)
  {
    case SETTING_TRIG_HOUR:
      if(is_get) *p_value = p_timer->trig_time.time.hour;
      else p_timer->trig_time.time.hour = *p_value;
      break;
    case SETTING_TRIG_MINUTE:
      if(is_get) *p_value = p_timer->trig_time.time.minute;
      else p_timer->trig_time.time.minute = *p_value;
      break;
    case SETTING_TRIG_SECOND:
      if(is_get) *p_value = p_timer->trig_time.time.second;
      else p_timer->trig_time.time.second = *p_value;
      break;
    case SETTING_DOW_MASK:
      if(is_get) *p_value = p_timer->dow_mask;
      else p_timer->dow_mask = *p_value;
      break;
    case SETTING_MONTH_MASK:
      if(is_get) *p_value = p_timer->month_mask;
      else p_timer->month_mask = *p_value;
      break;
    case SETTING_OUTPUT_VALUE:
      if(is_get) *p_value = p_timer->act_param.output.value;
      else p_timer->act_param.output.value = *p_value;
      break;
    default:
      CONSOLE_LOG_ERROR("Unknown setting:%d", setting);
      break;
  }
}
// @param map_sys_to_ui: true: remap from 0-OUTPUT_CONTROLLER_MAX_VAL to 0-100, false: remap from 0-100 to 0-OUTPUT_CONTROLLER_MAX_VAL
static void remap_scheduler_output_value(scheduler_t * const p_sch, bool map_sys_to_ui)
{
  ASSERT_LOG_ERROR_RETURN(p_sch, "Invalid Param");
  if(output_controller_get_type(p_sch->act_param.output.channel) == OUTPUT_TYPE_BINARY)
  {
    if(p_sch->act_param.output.value > 0)
    {
      p_sch->act_param.output.value = (map_sys_to_ui ? 100 : OUTPUT_CONTROLLER_MAX_VAL);
    }
    return;
  }
  if(map_sys_to_ui)
  {
    p_sch->act_param.output.value = (p_sch->act_param.output.value * 100) / OUTPUT_CONTROLLER_MAX_VAL;
  }
  else
  {
    p_sch->act_param.output.value = (p_sch->act_param.output.value * OUTPUT_CONTROLLER_MAX_VAL) / 100;
  }
}