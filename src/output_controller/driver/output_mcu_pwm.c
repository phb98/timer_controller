#include "output_mcu_pwm.h"
#include <stdint.h>
#include <stddef.h>
#include "console.h"
static void mcu_pwm_init();
static void mcu_pwm_set(const output_ch_t ch, const output_val_t val);
static output_val_t pcf9574_get(const output_ch_t ch);
static output_driver_t mcu_pwm_driver = 
{
  .get = pcf9574_get,
  .set = mcu_pwm_set,
  .init = mcu_pwm_init
};
output_driver_t * mcu_pwm_get_driver()
{
  return &mcu_pwm_driver;
}


static void mcu_pwm_init()
{
  CONSOLE_LOG_INFO("Init output MCU PWM");
}
static void mcu_pwm_set(const output_ch_t ch, const output_val_t val)
{

}
static output_val_t pcf9574_get(const output_ch_t ch)
{
  return 0;
}