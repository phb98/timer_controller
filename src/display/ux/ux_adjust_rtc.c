/**
 * THIS FILE IS ONLY TEMPLATE FOR OTHER UX NODE
 * THIS FILE IS NOT SUPPOSED TO BE BUILT
*/
#include "ux.h"
#include "console.h"
#include "ui.h"
#include "ux_timer.h"
#include "ux_utility.h"
#include "ux_time.h"
#define NODE_NAME "NODE_ADJUST_RTC"
#define SCREEN_TIMEOUT_TIMER  "Adjust_rtc_timeout"
#define SCREEN_TIMEOUT_MS     (30000)
// Private function prototype
static void        node_draw_scr();
static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param);
// Node variable, will be extern in UX and other node
ux_node_t ux_adjust_rtc_node = 
{
  .node_name = NODE_NAME,
  .screen_update = node_draw_scr,
  .node_process = node_process
};
// Relating node
extern ux_node_t ux_menu_node;
extern ux_node_t ux_mainscreen_node;
// Private variable
typedef enum 
{
  RTC_SETTING_YEAR = 0,
  RTC_SETTING_MONTH,
  RTC_SETTING_DAY,
  RTC_SETTING_DOW,
  RTC_SETTING_HOUR,
  RTC_SETTING_MINUTE,
  RTC_SETTING_SECOND,
  NUM_OF_RTC_SETTING,
} rtc_setting_t;
static ui_screen_info_t screen_info;
static bool is_changing_val = false;
static ux_rtc_t changing_rtc;
static ux_node_t * p_previous_node = NULL;
static bool force_clear_screen = false;
static uint8_t changing_idx = 0;
// Private function
static void chaning_single_rtc_setting(rtc_t * p_rtc, const rtc_setting_t setting, const int32_t new_val);
static int32_t get_single_rtc_val(const rtc_t * p_rtc, rtc_setting_t setting);
static ux_utility_adjust_var_wrap_t get_wrap_config_from_rtc_setting(rtc_setting_t setting);
static void node_draw_scr()
{
  CONSOLE_LOG_VERBOSE("Request node %s draw screen", NODE_NAME);
  screen_info.adjust_time.is_selected = is_changing_val;
  screen_info.adjust_time.highlight_idx = changing_idx;
  memcpy(&(screen_info.adjust_time.current_time), &changing_rtc, sizeof(rtc_t));
  ui_update_screen(UI_SCREEN_ADJUST_TIME, &screen_info, force_clear_screen);
  ux_timer_start(SCREEN_TIMEOUT_TIMER, SCREEN_TIMEOUT_MS);
  force_clear_screen = false;
}

static ux_node_t * node_process(ux_event_t evt, const ux_evt_param_t * p_evt_param)
{
  ux_node_t * p_node_ret = NULL;
  ux_node_t * const node_self = &ux_adjust_rtc_node;
  switch(evt)
  {
    case UX_EVENT_NODE_SWITCH_INTO:
    {
      CONSOLE_LOG_INFO("Switch into node %s", NODE_NAME);
      p_previous_node = p_evt_param->switch_into.p_previous_node;
      if(p_previous_node == &ux_menu_node)
      {
        // Normal flow, we will get system current time
        ux_time_get_current(&changing_rtc);
        changing_idx = 0;
      }
      // Reset variable
      is_changing_val = false;
      force_clear_screen = true;
      break;
    }
    case UX_EVENT_BUTTON:
    {
      if(p_evt_param->button.button == BUTTON_ENTER && p_evt_param->button.evt == BUTTON_EVT_LONG_PRESS)
      {
        // Save RTC and
        // Back to previous screen
        rtc_set_current(&changing_rtc);
        p_node_ret = p_previous_node;
      }
      else if(p_evt_param->button.evt == BUTTON_EVT_CLICK)
      {
        if(p_evt_param->button.button == BUTTON_ENTER)
        {
          is_changing_val = !is_changing_val;
        }
        else if(p_evt_param->button.button == BUTTON_UP)
        {
          if(!is_changing_val) changing_idx = (changing_idx + 1) % NUM_OF_RTC_SETTING;
          else
          {
            int32_t temp = get_single_rtc_val(&changing_rtc, changing_idx);
            ux_utility_adjust_var_wrap_t wrap_config = get_wrap_config_from_rtc_setting(changing_idx);
            temp = ux_utility_adjust_var_wrapping(&wrap_config, temp, 1);
            chaning_single_rtc_setting(&changing_rtc, changing_idx, temp);
          }
        }
        else if(p_evt_param->button.button == BUTTON_DOWN)
        {
          if(!is_changing_val) changing_idx = (changing_idx == 0 ? NUM_OF_RTC_SETTING - 1 : changing_idx - 1); 
          else
          {
            int32_t temp = get_single_rtc_val(&changing_rtc, changing_idx);
            ux_utility_adjust_var_wrap_t wrap_config = get_wrap_config_from_rtc_setting(changing_idx);
            temp = ux_utility_adjust_var_wrapping(&wrap_config, temp, -1);
            chaning_single_rtc_setting(&changing_rtc, changing_idx, temp);
          }
        }
        p_node_ret = node_self;
      }
    case UX_EVENT_TIMER_FIRED:
    {
      if(strcmp(p_evt_param->evt_timer.timer_name, SCREEN_TIMEOUT_TIMER) == 0)
      {
        // Switch back to main screen node
        p_node_ret = &ux_mainscreen_node;
      }
    }
      break;
    }
    default:
      break;
  }
  if(p_node_ret != NULL && p_node_ret != node_self)
  {
    // Stop the ux timer if we swtich to other node
    ux_timer_stop(SCREEN_TIMEOUT_TIMER);
  }
  return p_node_ret;
}

static void chaning_single_rtc_setting(rtc_t * p_rtc, const rtc_setting_t setting, const int32_t new_val)
{
  ASSERT_LOG_ERROR_RETURN((p_rtc) && (setting < NUM_OF_RTC_SETTING), "Invalid param");
  switch(setting)
  {
    case RTC_SETTING_YEAR:
      p_rtc->date.year = new_val;
      break;
    case RTC_SETTING_MONTH:
      p_rtc->date.month = new_val;
      break;
    case RTC_SETTING_DAY:
      p_rtc->date.day = new_val;
      break;
    case RTC_SETTING_DOW:
      p_rtc->date.DoW = new_val;
      break;
    case RTC_SETTING_HOUR:
      p_rtc->time.hour = new_val;
      break;
    case RTC_SETTING_MINUTE:
      p_rtc->time.minute = new_val;
      break;
    case RTC_SETTING_SECOND:
      p_rtc->time.second = new_val;
      break;
    default:
      break;
  }
}
static ux_utility_adjust_var_wrap_t get_wrap_config_from_rtc_setting(rtc_setting_t setting)
{
  ux_utility_adjust_var_wrap_t wrap_config = 
  {
    .lower_bound = 0,
    .low_wrap = true,
    .up_wrap = true,
  };
  switch(setting)
  {
    case RTC_SETTING_YEAR:
      wrap_config.upper_bound = 2099;
      wrap_config.lower_bound = 2000;
      break;
    case RTC_SETTING_MONTH:
      wrap_config.upper_bound = 12;
      wrap_config.lower_bound = 1;
      break;
    case RTC_SETTING_DAY:
      wrap_config.upper_bound = 31;
      wrap_config.lower_bound = 1;
      break;
    case RTC_SETTING_DOW:
      wrap_config.upper_bound = 6;
      wrap_config.lower_bound = 0;
      break;
    case RTC_SETTING_HOUR:
      wrap_config.upper_bound = 23;
      wrap_config.lower_bound = 0;
      break;
    case RTC_SETTING_MINUTE:
    case RTC_SETTING_SECOND:
      wrap_config.upper_bound = 59;
      wrap_config.lower_bound = 0;
      break;
    default:
      break;
  }
  return wrap_config;
}

static int32_t get_single_rtc_val(const rtc_t * p_rtc, rtc_setting_t setting)
{
  switch(setting)
  {
    case RTC_SETTING_YEAR:
      return p_rtc->date.year;
      break;
    case RTC_SETTING_MONTH:
      return p_rtc->date.month;
      break;
    case RTC_SETTING_DAY:
      return p_rtc->date.day;
      break;
    case RTC_SETTING_DOW:
      return p_rtc->date.DoW;
      break;
    case RTC_SETTING_HOUR:
      return p_rtc->time.hour;
      break;
    case RTC_SETTING_MINUTE:
      return p_rtc->time.minute;
      break;
    case RTC_SETTING_SECOND:
      return p_rtc->time.second;
      break;
    default:
      return 0;
      break;
  }
}