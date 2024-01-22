#ifndef _OUTPUT_CONTROLLER_H_
#define _OUTPUT_CONTROLLER_H_
#include <stdbool.h>
#include <stdint.h>
#include "rtc.h"
typedef uint16_t output_ch_t;
typedef uint32_t output_val_t;
typedef enum 
{
  OUTPUT_TYPE_BINARY,
  OUTPUT_TYPE_PWM,
  NUM_OF_OUTPUT_TYPE
} output_type_t;
typedef struct
{
  void         (*init)();
  void         (*set)(const output_ch_t ch, const output_val_t val);
  output_val_t (*get)(const output_ch_t ch);
} output_driver_t;
void          output_controller_init();
void          output_controller_set_val(const output_ch_t ch, const output_val_t val);
output_val_t  output_controller_get_val(const output_ch_t ch);
output_type_t output_controller_get_type(const output_ch_t ch);

#endif