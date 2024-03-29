#include "hal_i2c.h"
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "console.h"
#include <stdbool.h>
typedef i2c_inst_t *i2c_sdk_t;
typedef struct 
{
  const i2c_sdk_t hw_i2c;
  const uint8_t scl_pin;
  const uint8_t sda_pin;
  uint32_t current_speed;
  SemaphoreHandle_t mutex;
  bool is_inited;
} i2c_t; 
static i2c_t i2c_hw_table[NUM_OF_HAL_I2C] =
{
  [HAL_I2C0] = 
  {
    .hw_i2c  = i2c0,
    .scl_pin = CONFIG_I2C0_SCL,
    .sda_pin = CONFIG_I2C0_SDA,
    .current_speed = 100000,
  },
  [HAL_I2C1] = 
  {
    .hw_i2c  = i2c1,
    .scl_pin = CONFIG_I2C1_SCL,
    .sda_pin = CONFIG_I2C1_SDA,
    .current_speed = 100000,
  }
};
static void i2c_hw_init(const hal_i2c_instance_t ins);
// Public functions
void hal_i2c_get_handle(hal_i2c_handle_t * const p_handle, const hal_i2c_instance_t ins, uint32_t speed)
{
  if(ins >= NUM_OF_HAL_I2C || (!p_handle))
  {
    CONSOLE_LOG_ERROR("Invalid input");
    return;
  }
  p_handle->ins = ins;
  p_handle->speed = speed;
  // This function will not reinit the i2c if the i2c already inited, so dont worry
  i2c_hw_init(ins);
  i2c_t * const p_i2c = &i2c_hw_table[ins];
  xSemaphoreTake(p_i2c->mutex, 10000);
  xSemaphoreGive(p_i2c->mutex);
}

void hal_i2c_set_speed(hal_i2c_handle_t * const p_handle, const uint32_t speed)
{
  if(!p_handle) 
  {
    CONSOLE_LOG_ERROR("Invalid input");
    return;
  }
  p_handle->speed = speed;
}
uint32_t hal_i2c_send_blocking(hal_i2c_handle_t * const p_handle, const uint8_t addr, const uint8_t * const p_data, const uint32_t num_data)
{
  uint32_t speed = p_handle->speed;
  uint32_t ins   = p_handle->ins;
  if((!p_handle) || ins >= NUM_OF_HAL_I2C || (!p_data) || (!num_data) || (!speed))
  {
    CONSOLE_LOG_ERROR("Invalid input");
    return 0;
  }
  i2c_t * const p_i2c = &i2c_hw_table[ins];
  if(pdFALSE == xSemaphoreTake(p_i2c->mutex, MS_TO_TICK_RTOS(5000)))
  {
    CONSOLE_LOG_ERROR("i2c%d mutex timeout", ins);
    return 0;
  }
  if(p_i2c->current_speed != speed)
  {
    p_i2c->current_speed = speed;
    i2c_set_baudrate(p_i2c->hw_i2c, speed);
    vTaskDelay(MS_TO_TICK_RTOS(50));
  }
  //int byte_sent = i2c_write_blocking(p_i2c->hw_i2c, addr, p_data, num_data, false);
  int byte_sent = i2c_write_timeout_us(p_i2c->hw_i2c, addr, p_data, num_data, false, (3*1000*1000));
  i2c_hw_t * p_i2c_hw = i2c_get_hw(p_i2c->hw_i2c);
  // Give back the mutex
  xSemaphoreGive(p_i2c->mutex);
  if(byte_sent != num_data)
  {
    return 0;
  }
  return byte_sent;
}

uint32_t hal_i2c_read_blocking(hal_i2c_handle_t * const p_handle, const uint8_t addr, uint8_t * const p_data, const uint32_t num_data)
{
  uint32_t speed = p_handle->speed;
  uint32_t ins   = p_handle->ins;
  if((!p_handle) || ins >= NUM_OF_HAL_I2C || (!p_data) || (!num_data) || (!speed))
  {
    CONSOLE_LOG_ERROR("Invalid input");
    return 0;
  }
  i2c_t * const p_i2c = &i2c_hw_table[ins];
  if(pdFALSE == xSemaphoreTake(p_i2c->mutex, MS_TO_TICK_RTOS(5000)))
  {
    CONSOLE_LOG_ERROR("i2c%d mutex timeout", ins);
    return 0;
  }
  if(p_i2c->current_speed != speed)
  {
    p_i2c->current_speed = speed;
    //i2c_set_baudrate(p_i2c->hw_i2c, speed);
  }
  int byte_read = i2c_read_blocking(p_i2c->hw_i2c, addr, p_data, num_data, false);
  // Give back the mutex
  xSemaphoreGive(p_i2c->mutex);
  if(byte_read != num_data)
  {
    return 0;
  }
  return byte_read;
}
// PRIVATE FUNCTIONS
static void i2c_hw_init(const hal_i2c_instance_t ins)
{
  i2c_t * const p_i2c_hw = &i2c_hw_table[ins];
  if(p_i2c_hw->is_inited)
  {
    CONSOLE_LOG_DEBUG("i2c%d is already inited", ins);
    return;
  }
  p_i2c_hw->is_inited = true;
  p_i2c_hw->mutex = xSemaphoreCreateMutex();
  if(!p_i2c_hw->mutex)
  {
    CONSOLE_LOG_ERROR("Fail to create i2c mutex");
    return;
  }
  xSemaphoreTake(p_i2c_hw->mutex, 5000);
  uint8_t scl_gpio_state, sda_gpio_state;
  scl_gpio_state = gpio_get(p_i2c_hw->scl_pin);
  sda_gpio_state = gpio_get(p_i2c_hw->sda_pin);
  CONSOLE_LOG_DEBUG("i2c_scl state:%d", scl_gpio_state);
  CONSOLE_LOG_DEBUG("i2c_sda state:%d", sda_gpio_state);
  if(!scl_gpio_state || !sda_gpio_state)
  {
    // I2C BUS stuck, try to recovery
    CONSOLE_LOG_WARN("I2C%d bus stuck, try to recovery", ins);
    const uint8_t clock_num = 128;
     // Set SCL to output to manually bit banging
    gpio_set_dir(p_i2c_hw->sda_pin, false); //

    for(int i = 0; i < clock_num; i++)
    {
      if(sda_gpio_state)
      {
        CONSOLE_LOG_WARN("I2C recovery success");
        break;
      }
      gpio_set_dir(p_i2c_hw->scl_pin, true);
      gpio_put(p_i2c_hw->scl_pin, 0);
      for(volatile int temp = 0; temp < 100; temp++);
      gpio_set_dir(p_i2c_hw->scl_pin, false);

      sda_gpio_state = gpio_get(p_i2c_hw->sda_pin);
      for(volatile int temp = 0; temp < 100; temp++);
    }
  }

  // Set GPIO
  gpio_set_function(p_i2c_hw->scl_pin, GPIO_FUNC_I2C);
  gpio_set_function(p_i2c_hw->sda_pin, GPIO_FUNC_I2C);
  i2c_init(p_i2c_hw->hw_i2c, p_i2c_hw->current_speed);
  //gpio_pull_up(p_i2c_hw->scl_pin);
  //gpio_pull_up(p_i2c_hw->sda_pin);
  xSemaphoreGive(p_i2c_hw->mutex);
  CONSOLE_LOG_DEBUG("I2C%d init done", ins);
}


