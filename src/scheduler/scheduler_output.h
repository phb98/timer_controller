#ifndef _SCHEDULER_OUTPUT_H_
#define _SCHEDULER_OUTPUT_H_
scheduler_ret_t scheduler_output_get_by_channel(const output_ch_t ch, uint32_t start_idx, scheduler_t *p_out_sch, uint32_t * p_num_out);
scheduler_ret_t scheduler_output_set_channel(const scheduler_t * const p_sch);
#endif