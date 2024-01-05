#ifndef _DISPLAY_H_
#define _DISPLAY_H_
#include <stdint.h>
#include "u8g2.h"
#define DISPLAY_THREAD_STACK_SIZE     (8192) // 4KB
#define DISPLAY_MSG_QUEUE_LENGTH      (32)
typedef void (*display_msg_func)(void* p_param);
typedef struct
{
  display_msg_func func;
  void * p_param;
  size_t param_size;
} display_msg_t;
void      display_init();
u8g2_t *  display_get_u8g2_handle();
void*     display_mem_malloc(uint32_t num_bytes);
void      display_mem_free(void * const p_mem);

void      display_thread_post_msg(display_msg_t msg);
#endif