#ifndef _HAL_I2C_H_
#define _HAL_I2C_H_
#include "hardware/i2c.h"
#include "pico/stdlib.h"
typedef enum
{
  HAL_I2C0,
  HAL_I2C1,
  NUM_OF_HAL_I2C
} hal_i2c_instance_t;
typedef struct 
{
  hal_i2c_instance_t ins;
  uint32_t speed;
} hal_i2c_handle_t;
void hal_i2c_get_handle(hal_i2c_handle_t * const p_handle, const hal_i2c_instance_t ins, uint32_t speed);
void hal_i2c_set_speed(hal_i2c_handle_t * const p_handle, const uint32_t speed);
uint32_t hal_i2c_send_blocking(hal_i2c_handle_t * const p_handle, const uint8_t addr, const uint8_t * const p_data, const uint32_t num_data);
uint32_t hal_i2c_read_blocking(hal_i2c_handle_t * const p_handle, const uint8_t addr, uint8_t * const p_data, const uint32_t num_data);
#endif