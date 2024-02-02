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
// Private functions prototype
void low_level_init();
TaskHandle_t pico_task_handle = NULL;
volatile uint8_t a = 0;
void led_task_pico(void* unused_arg) {
    low_level_init();
    lfs_port_init();
    display_init();
    user_input_init();
    rtc_init();
    output_controller_init();
    sheduler_init(); // Must after littleFS since it will use file system
    CONSOLE_LOG_INFO("System Init done");

    // Store the Pico LED state
    uint8_t pico_led_state = 0;
    

    while (true) {
        // Turn Pico LED on an add the LED state
        // to the FreeRTOS xQUEUE
        pico_led_state = 1;
        for(int i =0 ; i < 8; i++)
        //output_controller_set_val(i, pico_led_state);
        vTaskDelay(5000);

        // Turn Pico LED off an add the LED state
        // to the FreeRTOS xQUEUE
        pico_led_state = 0;
        for(int i =0 ; i < 8; i++)
        //output_controller_set_val(i, pico_led_state);
        vTaskDelay(5000);
        CONSOLE_LOG_DEBUG("A");

        a++;
    }
}

int main() {
  stdio_init_all();
  CONSOLE_LOG_INFO("Begin system init");
  BaseType_t pico_status = xTaskCreate(led_task_pico, 
                                        "PICO_LED_TASK", 
                                        2048, 
                                        NULL, 
                                        configMAX_PRIORITIES-1, 
                                        &pico_task_handle);
  vTaskStartScheduler();
  // Should not reach below since the RTOS will start execute thread now
  while(1)
  {
    a++;
  }
  return 0; // SHOULD NOT REACH HERE
}

void low_level_init()
{
  // Init hardware stuff before RTOS
  // hal_i2c_init(HAL_I2C0);
}

