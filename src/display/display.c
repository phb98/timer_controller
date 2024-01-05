/**
 * Module display provides A single thread to run all the UI/UX
 * It also provides basic API for memory allocation
 * ALl UI/UX should run in display's context since they are not thread safe
 * 
*/

#include "display.h"
#include "console.h"
#include "u8g2.h"
#include "display_u8g2_port.h"
#include "FreeRTOS.h"
#include "task.h"
#include "ux.h"
#include "queue.h"
// Private variable
static QueueHandle_t thread_msg_queue;
static void display_thread_entry(void* unused_arg);
void display_init()
{
  CONSOLE_LOG_INFO("Init display");
  // Create message queue
  thread_msg_queue = xQueueCreate(DISPLAY_MSG_QUEUE_LENGTH, sizeof(display_msg_t));
  if(thread_msg_queue == NULL)
  {
    CONSOLE_LOG_ERROR("Fail to create msg queue");
  }
  xTaskCreate(display_thread_entry, 
              "DISPLAY_THREAD", 
              (DISPLAY_THREAD_STACK_SIZE/sizeof(configSTACK_DEPTH_TYPE)), 
              NULL, 
              1, 
              NULL);
}

void* display_mem_malloc(uint32_t num_bytes)
{
  return pvPortMalloc(num_bytes);
}

void display_mem_free(void * const p_mem)
{
  vPortFree(p_mem);
}

void display_thread_post_msg(display_msg_t msg)
{
  if(!msg.func)
  {
    CONSOLE_LOG_ERROR("Display msg has no function");
    return;
  }
  // Allocate new memory if needed
  if(msg.param_size)
  {
    void * p_param_mem = display_mem_malloc(msg.param_size);
    if(!p_param_mem)
    {
      CONSOLE_LOG_ERROR("Fail to alloc %d bytes, out of heap ?", msg.param_size);
      return;
    }
    // Copy param to new allocated memory
    memcpy(p_param_mem, msg.p_param, msg.param_size);
    // change the pointer to our new allocated memory
    msg.p_param = p_param_mem;
  }
  if(xQueueSend(thread_msg_queue, &msg, 0) != pdTRUE)
  {
    // Post fail
    CONSOLE_LOG_ERROR("Send Display msg fail");
    // Free the memory if allocated
    if(msg.param_size) display_mem_free(msg.p_param);
  }
}

static void display_thread_entry(void* unused_arg)
{
  CONSOLE_LOG_DEBUG("Display Thread entry");
  display_u8g2_port_init();
  // Init ux
  ux_init();
  ux_send_event(UX_EVENT_STARTUP, NULL);
  while (1)
  {
    display_msg_t thread_msg;
    xQueueReceive(thread_msg_queue, &thread_msg, portMAX_DELAY);
    // Thread will block until there is new message
    if(thread_msg.func == NULL)
    {
      // Invalid message received
      CONSOLE_LOG_ERROR("Received invalid disp message");
    }
    else
    {
      thread_msg.func(thread_msg.p_param);
      if(thread_msg.param_size)
      {
        display_mem_free(thread_msg.p_param);
      }
    }
  }
}

