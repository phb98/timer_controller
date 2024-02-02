#ifndef _SCHEDULER_FILE_H_
#define _SCHEDULER_FILE_H_
#include "scheduler.h"
#define SCHEDULER_FILE_MAJOR_VER  (1)
#define SCHEDULER_FILE_MINOR_VER  (0)
void scheduler_file_init();
scheduler_ret_t scheduler_file_new_scheduler(const scheduler_t * const p_new_scheduler);
scheduler_ret_t scheduler_file_is_existed(const scheduler_t * const p_scheduler);

#endif