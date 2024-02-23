// Standard lib
#include <stdint.h>
#include <stdio.h>
// Pico-SDK
#include "pico/stdlib.h"
#include "hardware/i2c.h"
// FreeRTOS
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
// Config
#include "sys_config.h"
// Moudle
#include "console.h"
#include "display.h"
#include "user_input.h"
#include "rtc.h"
#include "littlefs_port.h"
#include "output_controller.h"
#include "scheduler.h"
#include "watchdog.h"

// Private variables
TaskHandle_t house_keeping_task = NULL;
// Private functions prototype
void led_task_pico(void* unused_arg) {
    lfs_port_init();
    display_init();
    user_input_init();
    rtc_init();
    output_controller_init();
    sheduler_init(); // Must after littleFS since it will use file system
    watchdog_start(CONFIG_WATCHDOG_TIMEOUT_MS); // Start watchdog with 5s timeout
    CONSOLE_LOG_INFO("System Init done");
    // Change priority of this task next to lowest in order not to block other task
    vTaskPrioritySet(house_keeping_task, tskIDLE_PRIORITY + 1);
    while (true) {
        vTaskDelay(CONFIG_WATCHDOG_TIMEOUT_MS / 3); // Attempt to kick the watchdog every 1/3 of the timeout
        watchdog_kick();
    }
}

int main() {
  stdio_init_all();
  CONSOLE_LOG_INFO("Begin system init");
  BaseType_t pico_status = xTaskCreate(led_task_pico, 
                                        "Housekeeping Task", 
                                        2048, 
                                        NULL, 
                                        configMAX_PRIORITIES-1, 
                                        &house_keeping_task);
  vTaskStartScheduler();
  // Should not reach below since the RTOS will start execute thread now
  while(1)
  {
    // Should not reach here
  }
  return 0;
}

