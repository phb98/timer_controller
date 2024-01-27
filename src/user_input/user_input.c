#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "console.h"
#include "sys_config.h"
#include "user_input.h"
#include "user_input_button.h"
#include "user_input_def.h"
#include "ux.h"
typedef void (*input_process_t)(const uint32_t timestamp_ms, const user_input_value_t * p_value);

typedef struct
{
  uint32_t input_ms;
  user_input_type_t type;
  user_input_value_t value;
} user_input_queue_item_t;


static void user_input_thread_entry(void * p_param);
static void button_process(const uint32_t timestamp_ms, const user_input_value_t * p_value);
static void button_long_press_timer_cb(TimerHandle_t timer);
// Queue to receive user input to process


static input_process_t input_process_table[NUM_OF_USER_INPUT_TYPE] = {
  [USER_INPUT_BUTTON] = button_process
};

static struct
{
  QueueHandle_t queue;
  struct __button__{
    TimerHandle_t long_press_timer;
    uint32_t      press_time_ms;
    bool          is_pressing;
  }button[NUM_OF_BUTTON];
} user_input;
// PUBLIC FUNCTIONS
void user_input_init()
{
  CONSOLE_LOG_INFO("Init user input");
  // Init input hardware
  user_input_button_init();
  // Create button timer
  for(int i = 0; i < NUM_OF_BUTTON; i++)
  {
    user_input.button[i].long_press_timer = xTimerCreate("Button_longpress", 
                                                         CONFIG_BUTTON_LONG_PRESS_MS, 
                                                         false, 
                                                         &(user_input.button[i]), 
                                                         button_long_press_timer_cb);
  }
  // Create queue to handle user input
  user_input.queue = xQueueCreate(USER_INPUT_QUEUE_LENGTH, sizeof(user_input_queue_item_t));
  if(!user_input.queue)
  {
    CONSOLE_LOG_ERROR("Fail to create queue");
    return;
  }
  // Spawn a thread for gpio debounce and handle input
  xTaskCreate(user_input_thread_entry, 
              "USER_INPUT", 
              (USER_INPUT_STACK_SIZE/sizeof(configSTACK_DEPTH_TYPE)), 
              NULL, 
              USER_INPUT_THREAD_PRIORITY, // Maximum priority 
              NULL);
  CONSOLE_LOG_INFO("User Input init done");
}

static void user_input_thread_entry(void * p_param)
{
  CONSOLE_LOG_DEBUG("User Input thread entry");
  user_input_queue_item_t new_user_input;
  while(1)
  {
    xQueueReceive(user_input.queue, &new_user_input, portMAX_DELAY); 
    CONSOLE_LOG_DEBUG("Thread received input type:%d, time:%d", new_user_input.type, new_user_input.input_ms);
    if(input_process_table[new_user_input.type])
    {
      input_process_table[new_user_input.type](new_user_input.input_ms, &(new_user_input.value));
    }
    else CONSOLE_LOG_WARN("Does not have process function for this input type:%d", new_user_input.type);
  }
}

void user_input_notify(user_input_type_t type, user_input_value_t * p_value)
{
  if(type >= NUM_OF_USER_INPUT_TYPE || (!p_value))
  {
    CONSOLE_LOG_ERROR("Invalid user input type:%d", type);
    return;
  }
  user_input_queue_item_t new_input = 
  {
    .type = type,
    .input_ms = (xTaskGetTickCount()*1000)/configTICK_RATE_HZ
  };
  memcpy(&(new_input.value), p_value, sizeof(user_input_value_t));
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  xQueueSendFromISR(user_input.queue, &new_input, &xHigherPriorityTaskWoken);
  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// PRIVATE FUNCTIONS
static void button_process(const uint32_t timestamp_ms, const user_input_value_t * p_value)
{
  const char * button_name_str[] = {
    "Enter", "Up", "Down"
  };
  if(!p_value || p_value->button.button >= NUM_OF_BUTTON)
  {
    CONSOLE_LOG_ERROR("Invalid input");
    return;
  }
  CONSOLE_LOG_VERBOSE("Button:%s, is_Press:%s", button_name_str[p_value->button.button], (p_value->button.is_active ? "True" : "False"));
  // Get button
  struct __button__ * p_button = &(user_input.button[p_value->button.button]);
  // If button is not pressing
  if(!(p_button->is_pressing))
  {
    if(p_value->button.is_active)
    {
      //Save the timestamp
      p_button->press_time_ms = timestamp_ms;
      p_button->is_pressing = true;
      // Start long press timer
      xTimerStart(p_button->long_press_timer, portMAX_DELAY);
    }
  }
  else
  {
    // Button is pressing
    if(!p_value->button.is_active)
    {
      p_button->is_pressing = false;
      // Stop long press timer
      xTimerStop(p_button->long_press_timer, portMAX_DELAY);
      // Check if when we release button, Has it become long press or not
      if(timestamp_ms - p_button->press_time_ms < CONFIG_BUTTON_LONG_PRESS_MS)
      {
        // Has not become long press, consider it as normal event button click
        // send to ux
        ux_evt_param_t new_button_evt;
        new_button_evt.button.button = p_value->button.button;
        new_button_evt.button.evt = BUTTON_EVT_CLICK;
        ux_send_event(UX_EVENT_BUTTON, &new_button_evt);
      }
    }
  }
}

static void button_long_press_timer_cb(TimerHandle_t timer)
{
  struct __button__ * p_button = pvTimerGetTimerID(timer);
  // Get the button id
  user_input_button_t button_id;
  for(button_id = BUTTON_ENTER; button_id < NUM_OF_BUTTON; button_id++)
  {
    if(p_button == &(user_input.button[button_id])) break;
  }
  if(button_id < NUM_OF_BUTTON)
  {
    CONSOLE_LOG_VERBOSE("Long press timer fired for button:%d", button_id);
    // send to ux
    ux_evt_param_t new_button_evt;
    new_button_evt.button.button = button_id;
    new_button_evt.button.evt = BUTTON_EVT_LONG_PRESS;
    ux_send_event(UX_EVENT_BUTTON, &new_button_evt);
  }
  else 
  {
    CONSOLE_LOG_WARN("Fail to find button for long press timer");
  }
}