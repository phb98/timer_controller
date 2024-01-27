#include "output_mcu_pwm.h"
#include <stdint.h>
#include <stddef.h>
#include "console.h"
#include "hardware/pwm.h"
#include "pico/stdlib.h"
#include "output_controller.h"


typedef struct 
{
  const uint16_t pin;
  output_val_t current_duty;
}pwm_pin_t;
static void mcu_pwm_init();
static void mcu_pwm_set(const output_ch_t ch, const output_val_t val);
static output_val_t mcu_pwm_get(const output_ch_t ch);
static void pwm_pin_setup(uint16_t pin);
static void pwm_pin_set_duty(uint16_t pin, output_val_t val);
static output_driver_t mcu_pwm_driver = 
{
  .get = mcu_pwm_get,
  .set = mcu_pwm_set,
  .init = mcu_pwm_init
};
static pwm_pin_t pwm_pins[CONFIG_MCU_PWM_NUM_CH] = {
  {CONFIG_MCU_PWM_CH0_PIN, 0},
  {CONFIG_MCU_PWM_CH1_PIN, 0},
  {CONFIG_MCU_PWM_CH2_PIN, 0},
  {CONFIG_MCU_PWM_CH3_PIN, 0},
};
output_driver_t * mcu_pwm_get_driver()
{
  return &mcu_pwm_driver;
}


static void mcu_pwm_init()
{
  CONSOLE_LOG_INFO("Init output MCU PWM");
  for(int i = 0; i < CONFIG_MCU_PWM_NUM_CH; i++)
  {
    pwm_pin_setup(pwm_pins[i].pin);
    //mcu_pwm_set(i, 512); // Set default duty to 0
  }
  // gpio_set_function(pwm_pins[2].pin, GPIO_FUNC_SIO);
  // gpio_set_dir(pwm_pins[2].pin, true);
  // gpio_put(pwm_pins[2].pin, 1);
  // gpio_set_function(pwm_pins[0].pin, GPIO_FUNC_SIO);
  // gpio_set_dir(pwm_pins[0].pin, true);
  // gpio_put(pwm_pins[0].pin, 1);
}
static void mcu_pwm_set(const output_ch_t ch, const output_val_t val)
{
  ASSERT_LOG_ERROR_RETURN(ch < CONFIG_MCU_PWM_NUM_CH, "Invalid param");
  pwm_pin_set_duty(pwm_pins[ch].pin, val);
  pwm_pins[ch].current_duty = val;
}

static output_val_t mcu_pwm_get(const output_ch_t ch)
{
  if(ch >= CONFIG_MCU_PWM_NUM_CH)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return 0;
  }
  return pwm_pins[ch].current_duty;
}



static void pwm_pin_setup(uint16_t pin)
{
  #define MCU_CLOCK (125000000.0) //125Mhz
  gpio_set_function(pin, GPIO_FUNC_PWM);
  // Find out which PWM slice is connected
  uint16_t slice_num = pwm_gpio_to_slice_num(pin);
  uint16_t wrap_val = OUTPUT_MCU_PWM_MAX_VAL;
  float clk_div = (MCU_CLOCK/(float)CONFIG_MCU_PWM_FREQ) / (float)(OUTPUT_MCU_PWM_MAX_VAL + 1);
  if(clk_div > 256.0 || clk_div < 1.0)
  {
    CONSOLE_LOG_ERROR("Can not calculate pwm clock divider, set default clock to 1khz");
    clk_div = (MCU_CLOCK/1000.0) / (float)(OUTPUT_MCU_PWM_MAX_VAL + 1);
  }
  CONSOLE_LOG_VERBOSE("Pin:%d, pwm slice:%d, pwm ch:%d", pin, pwm_gpio_to_slice_num(pin), pwm_gpio_to_channel(pin));
  CONSOLE_LOG_VERBOSE("PWM clock div:%f, wrap val:%d", clk_div, wrap_val);
  pwm_set_clkdiv(slice_num, clk_div);
  pwm_set_wrap(slice_num, wrap_val);
  pwm_set_gpio_level(pin, wrap_val / 2);
  // Set the PWM running
  pwm_set_enabled(slice_num, true);
}

static void pwm_pin_set_duty(uint16_t pin, output_val_t val)
{
  #define PWM_VAL_MAPPING(in_val) ((in_val * OUTPUT_MCU_PWM_MAX_VAL) / OUTPUT_CONTROLLER_MAX_VAL);
  // remap
  val = PWM_VAL_MAPPING(val);
  pwm_set_gpio_level(pin, val);
}