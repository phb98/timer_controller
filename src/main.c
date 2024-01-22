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
    CONSOLE_LOG_INFO("System Init done");

    // Store the Pico LED state
    uint8_t pico_led_state = 0;
    
    // Configure the Pico's on-board LED
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    
    while (true) {
        // Turn Pico LED on an add the LED state
        // to the FreeRTOS xQUEUE
        pico_led_state = 1;
        gpio_put(PICO_DEFAULT_LED_PIN, pico_led_state);
        vTaskDelay(10000);
        
        // Turn Pico LED off an add the LED state
        // to the FreeRTOS xQUEUE
        pico_led_state = 0;
        gpio_put(PICO_DEFAULT_LED_PIN, pico_led_state);
        vTaskDelay(10000);
        CONSOLE_LOG_DEBUG("A");
        a++;
    }
}

int main() {
  stdio_init_all();
  CONSOLE_LOG_INFO("Begin system init");
  BaseType_t pico_status = xTaskCreate(led_task_pico, 
                                        "PICO_LED_TASK", 
                                        1024, 
                                        NULL, 
                                        1, 
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

