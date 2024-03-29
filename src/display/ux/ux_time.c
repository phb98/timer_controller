#include "ux.h"
#include "rtc.h"
#include "console.h"
#include <string.h>
#include "ux_time.h"
static void rtc_time_cb(const rtc_t * p_new_time);
void ux_time_init()
{
  rtc_register_cb(rtc_time_cb);
}

void ux_time_get_current(ux_rtc_t * const p_rtc)
{
  ASSERT_LOG_ERROR_RETURN(p_rtc, "Invalid param");
  rtc_get_current(p_rtc);
}

void ux_time_set_current(const ux_rtc_t * const p_rtc)
{
  ASSERT_LOG_ERROR_RETURN(p_rtc, "Invalid param");
  rtc_set_current(p_rtc);
}

static void rtc_time_cb(const rtc_t * p_new_time)
{
  ASSERT_LOG_ERROR_RETURN(p_new_time, "Invalid input");
  CONSOLE_LOG_VERBOSE("New time %02d:%02d:%02d", p_new_time->time.hour, p_new_time->time.minute, p_new_time->time.second);
  ux_evt_param_t evt_param;
  memcpy(&(evt_param.time_update.new_time), p_new_time, sizeof(rtc_t));
  ux_send_event(UX_EVENT_TIME_UPDATE, &evt_param);
}