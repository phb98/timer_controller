#include "output_pcf8574.h"
#include <stdint.h>
#include <stddef.h>
#include "console.h"
static void pcf8574_init();
static void pcf8574_set(const output_ch_t ch, const output_val_t val);
static output_val_t pcf9574_get(const output_ch_t ch);
static output_driver_t pcf8574_driver = 
{
  .get = pcf9574_get,
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
}
static void pcf8574_set(const output_ch_t ch, const output_val_t val)
{

}
static output_val_t pcf9574_get(const output_ch_t ch)
{
  return 0;
}