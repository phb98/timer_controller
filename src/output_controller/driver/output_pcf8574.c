#include "output_pcf8574.h"
#include <stdint.h>
#include <stddef.h>
#include "console.h"
#include "sys_config.h"
#include "hal_i2c.h"
#define PCF8574_DEFAULT_OUTPUT (0x0)
#define PCF8574_NUM_CHANNEL    (8)
#define BIT_SET(a, n)             ((a) |= (0x1 << n))
#define BIT_CLEAR(a, n)           ((a) &= (~(0x1 << n)))
#define BIT_READ(a, n)            (((a) >> n) & 0x1)
static void pcf8574_init();
static void pcf8574_set(const output_ch_t ch, const output_val_t val);
static output_val_t pcf8574_get(const output_ch_t ch);
static bool pcf8574_i2c_write(const uint8_t val);

static hal_i2c_handle_t i2c_handle;
static uint8_t          ch_state;
static output_driver_t pcf8574_driver = 
{
  .get = pcf8574_get,
  .set = pcf8574_set,
  .init = pcf8574_init
};
output_driver_t * pcf8574_get_driver()
{
  return &pcf8574_driver;
}


static void pcf8574_init()
{
  CONSOLE_LOG_INFO("Init output PCF8574");
  hal_i2c_get_handle(&i2c_handle, CONFIG_PCF8574_I2C, CONFIG_PCF8574_I2C_SPEED);
  // Check if communication with the chip ok
  uint8_t temp = PCF8574_DEFAULT_OUTPUT;
  if(!pcf8574_i2c_write(temp))
  {
    CONSOLE_LOG_ERROR("Fail to communicate to pcf8574");
    return;
  }
  CONSOLE_LOG_INFO("PCF8574 OK");
  ch_state = PCF8574_DEFAULT_OUTPUT;
}
static void pcf8574_set(const output_ch_t ch, const output_val_t val)
{
  ASSERT_LOG_ERROR_RETURN(ch < PCF8574_NUM_CHANNEL, "Input ch is over max ch");
  if(val) BIT_SET(ch_state, ch);
  else BIT_CLEAR(ch_state, ch);
  pcf8574_i2c_write(ch_state);
}
static output_val_t pcf8574_get(const output_ch_t ch)
{
  if(ch >= PCF8574_NUM_CHANNEL)
  {
    CONSOLE_LOG_ERROR("Input ch is over max ch");
    return 0;
  }
  return BIT_READ(ch_state, ch);
}

static bool pcf8574_i2c_write(const uint8_t val)
{
  return (hal_i2c_send_blocking(&i2c_handle, CONFIG_PCF8574_I2C_ADDR, &val, 1) == 1);
}