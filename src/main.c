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

// Private functions prototype
void low_level_init();
void ll_i2c_init();
TaskHandle_t pico_task_handle = NULL;
volatile uint8_t a = 0;
void led_task_pico(void* unused_arg) {

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
        vTaskDelay(1000);
        
        // Turn Pico LED off an add the LED state
        // to the FreeRTOS xQUEUE
        pico_led_state = 0;
        gpio_put(PICO_DEFAULT_LED_PIN, pico_led_state);
        vTaskDelay(1000);
        CONSOLE_LOG_DEBUG("A");
        a++;
    }
}

int main() {
  stdio_init_all();
  CONSOLE_LOG_INFO("Begin system init");
  low_level_init();
  display_init();
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
  ll_i2c_init();
}

void ll_i2c_init()
{
  CONSOLE_LOG_DEBUG("Init I2C Display");
  uint32_t i2c_speed = i2c_init(CONFIG_DISPLAY_I2C, CONFIG_DISPLAY_I2C_SPEED);
  CONSOLE_LOG_DEBUG("I2C Display speed:%d", i2c_speed);
  gpio_set_function(CONFIG_DISPLAY_I2C_SCL, GPIO_FUNC_I2C);
  gpio_set_function(CONFIG_DISPLAY_I2C_SDA, GPIO_FUNC_I2C);
  gpio_pull_up(CONFIG_DISPLAY_I2C_SCL);
  gpio_pull_up(CONFIG_DISPLAY_I2C_SDA);

  // Check if display is attached
  uint8_t temp_buf;
  if(i2c_write_blocking(CONFIG_DISPLAY_I2C, CONFIG_DISPLAY_I2C_ADDR, &temp_buf, 1, false) == sizeof(temp_buf))
  {
    CONSOLE_LOG_INFO("Found I2C Display 0x%x", CONFIG_DISPLAY_I2C_ADDR);
  }
  else CONSOLE_LOG_WARN("Can't find I2C Display 0x%x %d", CONFIG_DISPLAY_I2C_ADDR);

  
  CONSOLE_LOG_DEBUG("I2C Display init done");
}