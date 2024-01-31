#include "scheduler.h"
#include <stdint.h>
#include "rtc.h"
#include "console.h"
#include "core_json.h"
#include "scheduler_file.h"
#include "assert.h"
// Private function prototype
void rtc_cb(const rtc_t * p_new_time);
// Public function
void sheduler_init()
{
  CONSOLE_LOG_INFO("Init Scheduler");
  scheduler_file_init();

  rtc_register_cb(rtc_cb);
}

void scheduler_new(const scheduler_t * const p_new_scheduler)
{

}


// Private function
void rtc_cb(const rtc_t * p_new_time)
{
  ASSERT_LOG_ERROR_RETURN(p_new_time, "iNVALID PARAM");

}
