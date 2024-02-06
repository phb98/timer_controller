#ifndef _SCHEDULER_FILE_H_
#define _SCHEDULER_FILE_H_
#include "scheduler.h"
#define SCHEDULER_FILE_MAJOR_VER  (1)
#define SCHEDULER_FILE_MINOR_VER  (0)
void scheduler_file_init();
scheduler_ret_t scheduler_file_new_scheduler(const scheduler_t * const p_new_scheduler);
scheduler_ret_t scheduler_file_check_exist(const scheduler_t * const p_scheduler);
scheduler_ret_t scheduler_file_delete_all();
scheduler_ret_t scheduler_file_get_num_sch(uint32_t * p_num);
scheduler_ret_t scheduler_file_get_scheduler(scheduler_t * p_sch, uint32_t sch_idx);
scheduler_ret_t scheduler_file_modify(scheduler_t * const p_new_sch, uint32_t sch_idx);
scheduler_ret_t scheduler_file_get_multi_scheduler(scheduler_t * p_sch, uint32_t start_idx, uint32_t * p_num_entry_requested);
#endif