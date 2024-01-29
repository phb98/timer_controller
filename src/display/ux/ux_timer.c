#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "ux_timer.h"
#include <string.h>
#include "console.h"
#include "ux.h"
#define MS_TO_RTOS_TICK(ms) (ms/portTICK_PERIOD_MS)
// Private variable
// List of timer
typedef struct 
{
  char timer_name[UX_TIMER_NAME_LENGTH_MAX];
  char rtos_timer_name[32];
  TimerHandle_t timer_handle;
  // uint16_t      timer_id;
}ux_timer_t; 
static ux_timer_t ux_timer[NUM_OF_UX_TIMER];

static void ux_timer_callback(TimerHandle_t fired_timer);
void ux_timer_init()
{
  // Create all timer in RTOS
  for(uint16_t i = 0; i < NUM_OF_UX_TIMER; i++)
  {
    ux_timer_t * p_set_timer = &ux_timer[i];
    snprintf(p_set_timer->rtos_timer_name, sizeof(p_set_timer->rtos_timer_name), "ux_timer_%d", i);
    p_set_timer->timer_handle = NULL; // This to determine if create timer success or not
    p_set_timer->timer_handle = xTimerCreate(p_set_timer->rtos_timer_name, 1000, false, // ALL ux timer will be one shot, if ux node want to reload, it will do it manually
                                             p_set_timer,     // Since the next time it reload, it can use different timer, not the same one
                                             ux_timer_callback); // 
    if(p_set_timer->timer_handle == NULL)
    {
      CONSOLE_LOG_ERROR("Fail to create %s", p_set_timer->rtos_timer_name);
    }
    else CONSOLE_LOG_VERBOSE("Create timer:%s Success", p_set_timer->rtos_timer_name);
  }
}
void ux_timer_start(const char * timer_name, const uint32_t timer_interval_ms)
{
  if(!timer_name || !timer_interval_ms)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return;
  }
  if((strlen(timer_name) + 1) > UX_TIMER_NAME_LENGTH_MAX)
  {
    CONSOLE_LOG_WARN("Timer name too long, will be truncated");
  }
  ux_timer_t * p_timer = NULL;
  // Loop through timer list to
  // 1:Find idle timer
  // 2:Check if there is time with similar name running, if yes then restart it
  for(int16_t i = 0; i < NUM_OF_UX_TIMER; i++)
  {
    if(strcmp(ux_timer[i].timer_name, timer_name) == 0)
    {
      xTimerStop(ux_timer[i].timer_handle, portMAX_DELAY);
      p_timer = &ux_timer[i];
      CONSOLE_LOG_DEBUG("ux timer %s already running, restart it", timer_name);
      break;
    }
    if(xTimerIsTimerActive(ux_timer[i].timer_handle) == false && p_timer == NULL)
    {
      p_timer = &ux_timer[i];
    }
  }
  // Check if we find success
  if(p_timer == NULL)
  {
    CONSOLE_LOG_WARN("Tried to start ux timer:%s but no empty timer left", timer_name);
    return;
  }
  // Change to request timer interval
  if(pdPASS == xTimerChangePeriod(p_timer->timer_handle, MS_TO_RTOS_TICK(timer_interval_ms), portMAX_DELAY)) // Should not wait here since this timer is not active
  {
    // Copy timer name
    snprintf(p_timer->timer_name, UX_TIMER_NAME_LENGTH_MAX, "%s", timer_name);
    // Start the timer
    xTimerStart(p_timer->timer_handle, portMAX_DELAY); // Should start immediately
    const char * time_name = pcTimerGetName(p_timer->timer_handle);
    CONSOLE_LOG_DEBUG("Started ux timer:%s, rtos_timer_name:%s", timer_name, pcTimerGetName(p_timer->timer_handle));
  }
  else CONSOLE_LOG_ERROR("Fail to start ux timer:%s, rtos_timer_name:%s", timer_name, pcTimerGetName(p_timer->timer_handle));
}
void ux_timer_stop(const char * timer_name)
{
  ASSERT_LOG_ERROR_RETURN(timer_name, "Invalid param");
  ux_timer_t * p_timer = NULL;
  for(int16_t i = 0; i < NUM_OF_UX_TIMER; i++)
  {
    if(strcmp(ux_timer[i].timer_name, timer_name) == 0)
    {
      p_timer = &(ux_timer[i]);
      if(xTimerIsTimerActive(p_timer->timer_handle) == false)
      {
        CONSOLE_LOG_WARN("UX timer %s already stop", timer_name);
        return;
      }
      break;
    }
  }
  if(!p_timer)
  {
    CONSOLE_LOG_WARN("Can not find timer %s to stop", timer_name);
    return;
  }
  CONSOLE_LOG_DEBUG("Stop ux timer %s", timer_name);
  xTimerStop(p_timer->timer_handle, portMAX_DELAY);
  memset(p_timer->timer_name, 0x0, UX_TIMER_NAME_LENGTH_MAX);
}

// Private function
static void ux_timer_callback(TimerHandle_t fired_timer)
{
  // Make sure RTOS not passing garbage
  if(!fired_timer)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return;
  }
  // Get timer handle
  ux_timer_t *p_timer = (ux_timer_t*)(pvTimerGetTimerID(fired_timer));
  // Send to ux to pass to other
  ux_evt_param_t ux_timer_evt_param;
  memcpy(ux_timer_evt_param.evt_timer.timer_name, p_timer->timer_name, UX_TIMER_NAME_LENGTH_MAX);
  memset(p_timer->timer_name, 0x0, UX_TIMER_NAME_LENGTH_MAX);
  CONSOLE_LOG_DEBUG("Timer %s fired", p_timer->timer_name);
  ux_send_event(UX_EVENT_TIMER_FIRED, &ux_timer_evt_param);
}