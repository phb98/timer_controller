#ifndef _RTC_H_
#define _RTC_H_
#include <stdint.h>
#include <stdbool.h>

#define RTC_THREAD_STACK_SIZE (2048)
#define RTC_THREAD_PRIORITY   (3)
typedef struct
{
  uint16_t year;
  uint8_t month;
  uint8_t day;
  uint8_t DoW; // day of week, 0 is Sunday
} rtc_date_t;
typedef struct
{
  uint8_t hour;
  uint8_t minute;
  uint8_t second;
} rtc_time_t;

typedef struct
{
  rtc_date_t date;
  rtc_time_t time;
} rtc_t;

typedef void (*rtc_cb_t)(const rtc_t * p_new_time);
typedef struct
{
  void  (*init)();
  rtc_t (*get_datetime)();
  void  (*adjust_datetime)(const rtc_t * const p_datetime);
  void  (*reg_new_time_cb)(void (*cb)());
} rtc_controller_t;
void rtc_init();
const char* rtc_get_DoW_string(uint8_t day_of_week, bool use_long_string);
void rtc_register_cb(rtc_cb_t cb);

#endif