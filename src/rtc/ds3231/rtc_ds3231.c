#include "rtc_ds3231.h"
#include "ds3231_regs.h"
#include "hal_i2c.h"
#include "console.h"
#include "sys_config.h"
#include <stdbool.h>
// Private functions prototype
static void     ds3231_init();
static rtc_t    ds3231_get_datetime();
static bool     ds3231_is_stopped();
static void     ds3231_set_datetime(const rtc_t * const p_datetime);
static void     ds3231_start();
static bool     write_reg(uint8_t reg, uint8_t * const p_data);
static bool     read_reg(uint8_t reg, uint8_t * const p_data);
static bool     read_multi_regs(const uint8_t start_reg, const uint8_t num_reg, uint8_t * const p_data);
static bool     write_multi_regs(const uint8_t start_reg, const uint8_t num_reg, const uint8_t * const p_data);
static uint8_t  bcd2bin(uint8_t val) { return val - 6 * (val >> 4); }
static uint8_t  bin2bcd(uint8_t val) { return val + 6 * (val / 10); }

// Private variable
static rtc_controller_t rtc_controller =
{
  .init = ds3231_init,
  .get_datetime = ds3231_get_datetime,
  .adjust_datetime = ds3231_set_datetime,
};

static hal_i2c_handle_t rtc_i2c_handle;

static const rtc_t default_time = 
{
  .date = 
  {
    .day    = 1,
    .month  = 1,
    .year   = 2024,
    .DoW    = 1 // Monday
  },
  .time = 
  {
    .hour   = 00,
    .minute = 00,
    .second = 0
  }
};
// Public functions
rtc_controller_t * rtc_ds3231_get_controller()
{
  return &rtc_controller;
}

// Private functions
static void ds3231_init()
{
  CONSOLE_LOG_INFO("Init RTC DS3231");
  hal_i2c_get_handle(&rtc_i2c_handle, CONFIG_RTC_I2C, CONFIG_RTC_I2C_SPEED);
  // Read the first register to check if DS3231 is available
  uint8_t temp;
  if(read_reg(0x0, &temp))
  {
    CONSOLE_LOG_INFO("Found the DS3231 RTC");
  }
  else
  {
    CONSOLE_LOG_ERROR("Did not find the DS3231 RTC");
  }
  // Check if ds3231 is still running
  if(ds3231_is_stopped())
  {
    CONSOLE_LOG_INFO("DS3231 has lost time, attemp to set default time");
    ds3231_set_datetime(&default_time);
    ds3231_start();
  }
  
}
static rtc_t ds3231_get_datetime()
{
  rtc_t ret ={0};
  uint8_t raw_time[7]; // 7 Registers from 0x00 to 0x06
  if(read_multi_regs(DS3231_TIMEREG, sizeof(raw_time), raw_time))
  {
    ret.date.year   = bcd2bin(raw_time[6]) + 2000U;
    ret.date.month  = bcd2bin(raw_time[5] & 0x7F);
    ret.date.day    = bcd2bin(raw_time[4]);
    ret.date.DoW    = bcd2bin(raw_time[3]);
    ret.time.hour   = bcd2bin(raw_time[2]);
    ret.time.minute = bcd2bin(raw_time[1]);
    ret.time.second = bcd2bin(raw_time[0] & 0x7F);
  }
  else
  {
    CONSOLE_LOG_ERROR("Fail to get datetime from ds3231");
  }
  return ret;
}

static void ds3231_set_datetime(const rtc_t * const p_datetime)
{
  uint8_t buffer[7] = {
                        bin2bcd(p_datetime->time.second),
                        bin2bcd(p_datetime->time.minute),
                        bin2bcd(p_datetime->time.hour),
                        bin2bcd(p_datetime->date.DoW),
                        bin2bcd(p_datetime->date.day),
                        bin2bcd(p_datetime->date.month),
                        bin2bcd(p_datetime->date.year - 2000)
  };
  write_multi_regs(DS3231_TIMEREG, sizeof(buffer), buffer);
}

static void ds3231_start()
{
  uint8_t stat_reg;
  read_reg(DS3231_STATUSREG, &stat_reg);
  stat_reg &= ~(DS3231_STATUSREG_OSC_STOP_BIT); // flip OSF bit
  write_reg(DS3231_STATUSREG, &stat_reg);
}

static bool ds3231_is_stopped()
{
  uint8_t temp;
  // Read status register
  if(read_reg(DS3231_STATUSREG, &temp))
  {
    return (temp & DS3231_STATUSREG_OSC_STOP_BIT);
  }
  else return 0;
}
static bool write_reg(uint8_t reg, uint8_t * const p_data)
{
  if(!p_data)
  {
    CONSOLE_LOG_ERROR("Invalid input");
  }
  uint8_t temp[2] = {reg, *p_data};
  if((2 != hal_i2c_send_blocking(&rtc_i2c_handle, DS3231_ADDRESS, temp, 2)))
  {
    CONSOLE_LOG_ERROR("i2c write fail");
    return false;
  }
  return true;
}

static bool read_reg(uint8_t reg, uint8_t * const p_data)
{
  if(!p_data)
  {
    CONSOLE_LOG_ERROR("Invalid input");
  }
  if((1 != hal_i2c_send_blocking(&rtc_i2c_handle, DS3231_ADDRESS, &reg, 1)) || 
     (1 != hal_i2c_read_blocking(&rtc_i2c_handle, DS3231_ADDRESS, p_data, 1)))
  {
    CONSOLE_LOG_ERROR("i2c read fail");
    return false;
  }
  return true;
}

static bool read_multi_regs(const uint8_t start_reg, const uint8_t num_reg, uint8_t * const p_data)
{
  if(!p_data)
  {
    CONSOLE_LOG_ERROR("Invalid input");
  }
  if((1 != hal_i2c_send_blocking(&rtc_i2c_handle, DS3231_ADDRESS, &start_reg, 1)) || 
     (num_reg != hal_i2c_read_blocking(&rtc_i2c_handle, DS3231_ADDRESS, p_data, num_reg)))
  {
    CONSOLE_LOG_ERROR("i2c read fail");
    return false;
  }
  return true;
}

static bool write_multi_regs(const uint8_t start_reg, const uint8_t num_reg, const uint8_t * const p_data)
{
  uint8_t temp[16]; // To build command
  if(num_reg > sizeof(temp) - 1)
  {
    CONSOLE_LOG_ERROR("Fail, not enough memory");
  }
  if(!p_data)
  {
    CONSOLE_LOG_ERROR("Invalid input");
  }
  temp[0] = start_reg;
  // Crude memcpy
  for(int i = 0; i < num_reg; i++)
  {
    temp[i+1] = p_data[i];
  }
  if((num_reg + 1) != hal_i2c_send_blocking(&rtc_i2c_handle, DS3231_ADDRESS, temp, num_reg+1))
  {
    CONSOLE_LOG_ERROR("i2c write fail");
    return false;
  }
  return true;
}