#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>
#include "console.h"

uint32_t console_get_time_ms()
{
  return xTaskGetTickCountFromISR();
}