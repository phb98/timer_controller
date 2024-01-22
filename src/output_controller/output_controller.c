#include "output_controller.h"
#include "console.h"
#include "driver/output_mcu_pwm.h"
#include "driver/output_pcf8574.h"
#define OUTPUT_BLOCK_DECLARE(ch_start, ch_end, t, chip_use) \
{\
  .start_ch = ch_start,\
  .end_ch   = ch_end,\
  .type     = t,\
  .chip     = chip_use\
}

typedef struct
{
  output_ch_t     start_ch;
  output_ch_t     end_ch;
  output_type_t   type;
  uint8_t         chip;
  output_driver_t * (*get_driver)();
} output_block_t;

static output_driver_t * (*output_driver_getter[])() =
{
  [CONFIG_OUTPUT_CHIP_PCF8574] = pcf8574_get_driver,
  [CONFIG_OUTPUT_CHIP_MCU_PWM] = mcu_pwm_get_driver,
};

static output_block_t output_block[CONFIG_OUTPUT_NUM_BLOCK] = 
{
  [0] = OUTPUT_BLOCK_DECLARE(CONFIG_OUTPUT_BLOCK_1_START_CH, CONFIG_OUTPUT_BLOCK_1_END_CH,
                             CONFIG_OUTPUT_BLOCK_1_TYPE, CONFIG_OUTPUT_BLOCK_1_CHIP),
  [1] = OUTPUT_BLOCK_DECLARE(CONFIG_OUTPUT_BLOCK_2_START_CH, CONFIG_OUTPUT_BLOCK_2_END_CH,
                             CONFIG_OUTPUT_BLOCK_2_TYPE, CONFIG_OUTPUT_BLOCK_2_CHIP)
};

void get_block_from_ch(const output_ch_t ch, const output_block_t ** pp_block);
void output_controller_init()
{
  for(uint8_t i = 0; i < CONFIG_OUTPUT_NUM_BLOCK; i++)
  {
    CONSOLE_LOG_INFO("Init output block %d", i);
    output_block[i].get_driver = output_driver_getter[output_block[i].chip];
    output_block[i].get_driver()->init();
    CONSOLE_LOG_DEBUG("Output block %d:start ch:%d, end ch:%d, type:%d", 
                        i, output_block[i].start_ch, output_block[i].end_ch, output_block[i].type);
  }
}
void output_controller_set_val(const output_ch_t ch, const output_val_t val)
{
  const output_block_t * p_output_block;
  // Determine which block this output belong to
  get_block_from_ch(ch, &p_output_block);
  ASSERT_LOG_ERROR_RETURN(p_output_block, "Fail to get Output block");
  output_ch_t driver_ch = ch - p_output_block->start_ch;
  p_output_block->get_driver()->set(driver_ch, val);
  CONSOLE_LOG_DEBUG("Set channel %d to %d", ch, val);
}
output_val_t output_controller_get_val(const output_ch_t ch)
{
  const output_block_t * p_output_block;
  output_val_t ret_val;
  // Determine which block this output belong to
  get_block_from_ch(ch, &p_output_block);
  if(!p_output_block)
  {
    CONSOLE_LOG_ERROR("Fail to get Output block");
    return 0;
  }
  output_ch_t driver_ch = ch - p_output_block->start_ch;
  ret_val =  p_output_block->get_driver()->get(driver_ch);
  CONSOLE_LOG_DEBUG("get channel %d, val: %d", ch, ret_val);
}
output_type_t output_controller_get_type(const output_ch_t ch)
{
  const output_block_t * p_output_block;
  // Determine which block this output belong to
  get_block_from_ch(ch, &p_output_block);
  if(!p_output_block)
  {
    CONSOLE_LOG_ERROR("Fail to get Output block");
    return 0xff;
  }
  return p_output_block->type;
}
//PRIVATE FUNCTION
void get_block_from_ch(const output_ch_t ch, const output_block_t ** pp_block)
{
  if(!pp_block)
  {
    ASSERT_LOG_ERROR_RETURN(pp_block, "Invalid param");
  }
  (*pp_block) = NULL;
  const output_block_t * p_chking_block;
  for(uint8_t i = 0; i < CONFIG_OUTPUT_NUM_BLOCK; i++)
  {
    p_chking_block = &output_block[i];
    if(p_chking_block->start_ch >= ch && p_chking_block->end_ch <= ch)
    {
      (*pp_block) = p_chking_block;
      return;
    }
  }
  CONSOLE_LOG_WARN("Could not get output block for ch:%d", ch);
}

output_driver_t * get_driver_from_chip(const uint8_t chip)
{
  output_driver_t * ret= NULL;
  output_driver_t* (*driverGetters[])() = {
    [CONFIG_OUTPUT_CHIP_PCF8574] = pcf8574_get_driver,
    [CONFIG_OUTPUT_CHIP_MCU_PWM] = mcu_pwm_get_driver
    // Add more function pointers for other chips if needed
  };

  // Check if chip value is within the valid range
  if (chip >= 0 && chip < sizeof(driverGetters) / sizeof(driverGetters[0]))
  {
    // Call the corresponding function using the function pointer
    ret = driverGetters[chip]();
  }
  else
  {
    CONSOLE_LOG_ERROR("Can not get driver for chip:%d", chip);
  }
  return ret;
}