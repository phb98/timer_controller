#ifndef _USER_INPUT_H_
#define _USER_INPUT_H_
#include <stdint.h>
#include <stdbool.h>
#define USER_INPUT_STACK_SIZE       (2048)
#define USER_INPUT_THREAD_PRIORITY  (4)
#define USER_INPUT_QUEUE_LENGTH     (32)
#include "user_input_def.h"

typedef union
{
  struct
  {
    user_input_button_t button;
    bool is_active;
  } button;
}user_input_value_t;
void user_input_init();
void user_input_notify(user_input_type_t type, user_input_value_t * p_value);
#endif