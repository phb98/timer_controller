#ifndef _USER_INPUT_DEF_H_
#define _USER_INPUT_DEF_H_
typedef enum 
{
  USER_INPUT_BUTTON = 0,
  USER_INPUT_ENCODER, // Unsupport for now
  NUM_OF_USER_INPUT_TYPE,
}user_input_type_t;

typedef enum
{
  BUTTON_ENTER = 0,
  BUTTON_UP,
  BUTTON_DOWN,
  NUM_OF_BUTTON,
} user_input_button_t;
typedef enum
{
  BUTTON_EVT_CLICK,
  BUTTON_EVT_DOUBLE_CLICK, // Unsupport for now
  BUTTON_EVT_LONG_PRESS,
  NUM_OF_BUTTON_EVT,
} user_input_button_evt_t;
#endif