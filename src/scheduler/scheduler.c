#include "scheduler.h"
#include <stdint.h>
#include "rtc.h"
#include "console.h"
typedef struct
{
  rtc_time_t trigger_time;
  uint8_t    output_ch;
  uint32_t   output_val;
  uint8_t    dow_repeat;  // bit mask from sunday to saturday, bit 0 is sunday
                          // not implemented for now. set it to 0xFF when create
  uint16_t   month_repeat; // bit mask from Jan to Dec, bit 0 is Jan
                          // not implemented for now. set it to 0xFFFF when create
} scheduler_t;

typedef struct
{
  uint32_t signature;
  scheduler_t scheduler;
} scheduler_file_t;

void sheduler_init()
{

}