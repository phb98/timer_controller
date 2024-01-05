#ifndef _UX_TIMER_H
#define _UX_TIMER_H
#include <stdint.h>
#define NUM_OF_UX_TIMER           (8)
#define UX_TIMER_NAME_LENGTH_MAX  (32)
void ux_timer_init();
void ux_timer_start(const char * timer_name, const uint32_t timer_interval_ms);
#endif