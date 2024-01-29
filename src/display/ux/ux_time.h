#ifndef _UX_TIME_H_
#define _UX_TIME_H_
#include "rtc.h"
typedef rtc_t ux_rtc_t;
void ux_time_init();
void ux_time_get_current(ux_rtc_t * const p_rtc);
void ux_time_set_current(const ux_rtc_t * const p_rtc);
#endif