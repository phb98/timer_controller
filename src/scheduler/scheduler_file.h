#ifndef _SCHEDULER_FILE_H_
#define _SCHEDULER_FILE_H_
#include "scheduler.h"
#define SCHEDULER_FILE_MAJOR_VER  (1)
#define SCHEDULER_FILE_MINOR_VER  (0)
void scheduler_file_init();
bool scheduler_file_new_scheduler(const scheduler_t * const p_new_scheduler);


#endif