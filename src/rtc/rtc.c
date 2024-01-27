#include "rtc.h"
#include "hal_i2c.h"
#include "console.h"
#include "sys_config.h"
#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"
#include <stdbool.h>
#if CONFIG_RTC_CHIP_USING == CONFIG_RTC_CHIP_DS3231
#include "ds3231/rtc_ds3231.h"
#endif
// I hate this hack
uint32_t __get_IPSR(void)
{
  uint32_t result;
  asm volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}
#define CHECK_VALID_CONTROLLER(cont, func) ((cont != NULL) && (cont->func) != NULL)
#define IS_IN_ISR()    ((__get_IPSR() & 0x3f) != 0)
static rtc_controller_t * p_rtc_controller = NULL;
static rtc_cb_t cb_table[CONFIG_RTC_MAX_CALLBACK_NUM] = {0};
static uint16_t num_cb_registered = 0;
static SemaphoreHandle_t new_time_semphr;
static TaskHandle_t rtc_thread_handle;
static rtc_t now;
// Private function prototype
static void rtc_thread_entry(void * p_arg);
static void rtc_driver_new_time_cb();
static void rtc_dispatch(const rtc_t * p_new_time);
// Public function
void rtc_init()
{
  #if CONFIG_RTC_CHIP_USING == CONFIG_RTC_CHIP_DS3231
    p_rtc_controller = rtc_ds3231_get_controller();
  #else 
  #error Please choose your RTC chip
  #endif
    if(!p_rtc_controller)
    {
      CONSOLE_LOG_ERROR("Fail to get RTC controller");
      return;
    }
    // Check the valid of controller
    if(!(p_rtc_controller->init && 
         p_rtc_controller->get_datetime && 
         p_rtc_controller->reg_new_time_cb))  
    { 
      CONSOLE_LOG_ERROR("RTC Controller not having enough API");
      return;
    }
    p_rtc_controller->init();
    // Get current date time
    rtc_t now = p_rtc_controller->get_datetime();
    CONSOLE_LOG_INFO("Current datetime: %s, %d-%d-%d - %d:%d:%d",rtc_get_DoW_string(now.date.DoW, true),
                                                                now.date.year, now.date.month, now.date.day,
                                                                now.time.hour, now.time.minute, now.time.second);
    // Register cb
    p_rtc_controller->reg_new_time_cb(rtc_driver_new_time_cb);
    xTaskCreate(rtc_thread_entry, 
                "RTC", 
                (RTC_THREAD_STACK_SIZE/sizeof(configSTACK_DEPTH_TYPE)), 
                NULL, 
                RTC_THREAD_PRIORITY, 
                &rtc_thread_handle);
    new_time_semphr = xSemaphoreCreateBinary();
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

void rtc_register_cb(rtc_cb_t cb)
{
  ASSERT_LOG_ERROR_RETURN(cb, "Invalid input");
  ASSERT_LOG_ERROR_RETURN(num_cb_registered < CONFIG_RTC_MAX_CALLBACK_NUM, "Can not register new callback, out of mem");
  for(int i = 0; i < CONFIG_RTC_MAX_CALLBACK_NUM; i++)
  {
    if(cb_table[i] == NULL)
    {
      CONSOLE_LOG_VERBOSE("Register new cb, slot %d", i);
      cb_table[i] = cb;
      num_cb_registered++;
      return;
    }
  }
}
void rtc_get_current(rtc_t * const p_rtc)
{
  ASSERT_LOG_ERROR_RETURN(p_rtc, "Invalid param");
  memcpy(p_rtc, &now, sizeof(rtc_t));
}
// Private function
static void rtc_driver_new_time_cb()
{
  if(IS_IN_ISR())
  {
    BaseType_t HigherPriorityTaskWoken;
    vTaskNotifyGiveFromISR(rtc_thread_handle, &HigherPriorityTaskWoken);
    portYIELD_FROM_ISR(HigherPriorityTaskWoken);
  }
  else
  {
    xTaskNotifyGive(rtc_thread_handle);
  }
  //CONSOLE_LOG_DEBUG("New time from RTC");
}

static void rtc_dispatch(const rtc_t * p_new_time)
{
  for(int i = 0; i < CONFIG_RTC_MAX_CALLBACK_NUM; i++)
  {
    if(cb_table[i] != NULL)
    {
      cb_table[i](p_new_time);
    }
  }
}
static void rtc_thread_entry(void * p_arg)
{
  while(1)
  {
    ulTaskNotifyTake(true, portMAX_DELAY);
    now = p_rtc_controller->get_datetime();
    rtc_dispatch(&now);
  }
}