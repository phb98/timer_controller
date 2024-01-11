#include "rtc.h"
#include "hal_i2c.h"
#include "console.h"
#include "sys_config.h"
#include <stdbool.h>
#if CONFIG_RTC_CHIP_USING == CONFIG_RTC_CHIP_DS3231
#include "ds3231/rtc_ds3231.h"
#endif

#define CHECK_VALID_CONTROLLER(cont, func) ((cont != NULL) && (cont->func) != NULL)
static rtc_controller_t * p_rtc_controller = NULL;
void rtc_init()
{
  #if CONFIG_RTC_CHIP_USING == CONFIG_RTC_CHIP_DS3231
    p_rtc_controller = rtc_ds3231_get_controller();
    if(!p_rtc_controller)
    {
      CONSOLE_LOG_ERROR("Fail to get RTC controller");
      return;
    }
    if(!CHECK_VALID_CONTROLLER(p_rtc_controller, init) || !CHECK_VALID_CONTROLLER(p_rtc_controller, get_datetime))
    {
      CONSOLE_LOG_ERROR("RTC controller not having init function");
      return;
    }
    p_rtc_controller->init();
    // Get current date time
    rtc_t now = p_rtc_controller->get_datetime();
    CONSOLE_LOG_INFO("Current datetime: %s, %d-%d-%d - %d:%d:%d",rtc_get_DoW_string(now.date.DoW, true),
                                                                now.date.year, now.date.month, now.date.day,
                                                                now.time.hour, now.time.minute, now.time.second);
  #else 
  #error Please choose your RTC chip
  #endif
}

const char* rtc_get_DoW_string(uint8_t day_of_week, bool use_long_string) 
{
  const char* days_of_week_long[] = {
    "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
  };

  const char* days_of_week_short[] = {
    "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
  };

  // Check if the day_of_week value is within a valid range
  if (day_of_week >= 0 && day_of_week <= 6) {
      if (use_long_string) {
          return days_of_week_long[day_of_week];
      } else {
          return days_of_week_short[day_of_week];
      }
  } else {
      return NULL;
  }
}
