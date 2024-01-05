// This file abstract the hardware for button
// This file only generate reliable event press and release for user_input.c, all the complex action(double click, long press, etc) is calculated on upper level

#include "user_input.h"
#include "hardware/gpio.h"
#include "hardware/irq.h"
#include "sys_config.h"
#include "console.h"
#include "user_input_button.h"
#include "FreeRTOS.h"
#include "timers.h"
#define GPIO_SET_PULL(pin, active_state) if(active_state){gpio_pull_down(pin);} else gpio_pull_up(pin)
#define GPIO_GET_IRQ_MASK(active_state) (active_state ? GPIO_IRQ_EDGE_RISE:GPIO_IRQ_EDGE_FALL)

typedef enum 
{
  BUTTON_STATE_IDLE,
  BUTTON_STATE_DEBOUNCING,
  BUTTON_STATE_ACTIVE,
} button_state_t;
struct input_buttons_t
{
  const uint8_t pin;
  const uint8_t active_state;
  button_state_t state;
  TimerHandle_t debounce_timer;
} input_buttons[NUM_OF_BUTTON]=
{
  [BUTTON_ENTER]  = {
    .pin          = CONFIG_BUTTON_ENTER_PIN,
    .active_state = CONFIG_BUTTON_ENTER_ACTIVE,
    .state        = BUTTON_STATE_IDLE,
  },
  [BUTTON_UP]     = {
    .pin          = CONFIG_BUTTON_UP_PIN,
    .active_state = CONFIG_BUTTON_UP_ACTIVE,
    .state        = BUTTON_STATE_IDLE,
  },
  [BUTTON_DOWN]   = {
    .pin          = CONFIG_BUTTON_DOWN_PIN,
    .active_state = CONFIG_BUTTON_DOWN_ACTIVE,
    .state        = BUTTON_STATE_IDLE,
  }
};

static void user_input_button_irq_cb(uint pin, uint32_t event_mask);
static void button_enable_press_irq(const user_input_button_t button, bool is_enable);
static void button_enable_release_irq(const user_input_button_t button, bool is_enable);
static inline user_input_button_t get_button_from_pin(const uint8_t pin);
static void button_debouncing(const user_input_button_t button, uint8_t is_active);
static void button_timer_cb(TimerHandle_t xTimer);
static bool button_pin_read(uint8_t pin);
void user_input_button_init()
{
  // Init GPIO
  for(int i = 0; i < NUM_OF_BUTTON; i++)
  {
    gpio_set_function(input_buttons[i].pin,  GPIO_FUNC_SIO);
    GPIO_SET_PULL(input_buttons[i].pin, input_buttons[i].active_state);
    // Create button timer for debouncing
    input_buttons[i].debounce_timer = xTimerCreate("Button_timer", CONFIG_BUTTON_DEBOUNCE_MS, false, &input_buttons[i], button_timer_cb);
    // Enable GPIO interrupt
    gpio_set_irq_enabled_with_callback(input_buttons[i].pin, GPIO_GET_IRQ_MASK(input_buttons[i].active_state), true, user_input_button_irq_cb);
  }
  CONSOLE_LOG_DEBUG("Init User Input Button Done");
}
// Private functions
static inline user_input_button_t get_button_from_pin(const uint8_t pin)
{
  for(int i = 0; i < NUM_OF_BUTTON; i++)
  {
    if(pin == input_buttons[i].pin) return i;
  }
  CONSOLE_LOG_ERROR("Can find button for pin:%d", pin);
  return NUM_OF_BUTTON; // so that it's always invalid
}

static void user_input_button_irq_cb(uint pin, uint32_t event_mask)
{
  CONSOLE_LOG_VERBOSE("Button pin:%d event:%d", pin, event_mask);
  user_input_button_t button = get_button_from_pin(pin);
  if(button >= NUM_OF_BUTTON) return;
  // Disable interrupt, gpio debouncing state machine will set interrupt again
  if(event_mask & GPIO_GET_IRQ_MASK(input_buttons[button].active_state))
  {
    // Disable interrupt to avoid glitch
    button_enable_press_irq(button, false);
    button_debouncing(button, true);
  }
  else
  {
    // Disable interrupt to avoid glitch
    button_enable_release_irq(button, false);
    button_debouncing(button, false);
  }
}

static void button_debouncing(const user_input_button_t button, uint8_t is_active)
{
  switch(input_buttons[button].state)
  {
    case BUTTON_STATE_IDLE:
    {
      if(is_active)
      {
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        xTimerStartFromISR(input_buttons[button].debounce_timer, &xHigherPriorityTaskWoken);
        input_buttons[button].state = BUTTON_STATE_DEBOUNCING;
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
      }
      break;
    }
    case BUTTON_STATE_DEBOUNCING:
    {
      // There should be no interrupt when debouncing
      CONSOLE_LOG_WARN("Interrupt on pin %d when debouncing", input_buttons[button].pin);
      break;
    }
    case BUTTON_STATE_ACTIVE:
    {
      if(!is_active)
      {
        input_buttons[button].state = BUTTON_STATE_IDLE;
        user_input_value_t new_button_value = {.button ={.is_active = false, .button = button}};
        user_input_notify(USER_INPUT_BUTTON, &new_button_value);
        button_enable_press_irq(button, true);
      }
    }
    default:
      break;
  }
}

static void button_timer_cb(TimerHandle_t xTimer)
{
  struct input_buttons_t * p_input_button = pvTimerGetTimerID(xTimer);
  CONSOLE_LOG_VERBOSE("Debounce timer fired for pin %d", p_input_button->pin);
  if(button_pin_read(p_input_button->pin) == p_input_button->active_state)
  {
    p_input_button->state = BUTTON_STATE_ACTIVE;
    user_input_value_t new_button_value = {.button ={.is_active = true, .button = get_button_from_pin(p_input_button->pin)}};
    user_input_notify(USER_INPUT_BUTTON, &new_button_value);
    button_enable_release_irq(get_button_from_pin(p_input_button->pin), true);
  }
  else
  {
    p_input_button->state = BUTTON_STATE_IDLE;
    button_enable_press_irq(get_button_from_pin(p_input_button->pin), true);
  }
}

static void button_enable_press_irq(const user_input_button_t button, bool is_enable)
{
  gpio_set_irq_enabled(input_buttons[button].pin, GPIO_GET_IRQ_MASK(input_buttons[button].active_state), is_enable);
}

static void button_enable_release_irq(const user_input_button_t button, bool is_enable)
{
  gpio_set_irq_enabled(input_buttons[button].pin, GPIO_GET_IRQ_MASK(!(input_buttons[button].active_state)), is_enable);
}
static bool button_pin_read(uint8_t pin)
{
  if(0 == gpio_get(pin)) return false;
  return true;
}