#ifndef _SCHEDULER_JSON_H_
#define _SCHEDULER_JSON_H_
#include <stdint.h>
#include <stdbool.h>
#include "scheduler.h"
#include <string.h>
#define JSON_START_NEW(p)     (*p++ = '{')
#define JSON_END_NEW(p)       (*p++ = '}')
size_t add_start_object_to_JSON_string(char * const p_json_buffer, size_t max_json_length, const char * p_key_string, bool is_first);
size_t add_end_object_to_JSON_string(char * const p_json_buffer, size_t max_json_length);
size_t add_number_to_JSON_string(char * p_json_buff, size_t json_buff_len, const char * const p_key, int value, bool is_first);
size_t add_RTC_to_JSON_string(char * p_json_string, size_t max_json_length, const rtc_t * const p_rtc);
size_t add_scheduler_action_to_JSON_string(char * p_json_string, size_t max_json_length, const scheduler_action_t action, const scheduler_action_param_t * p_param, bool is_first);
size_t add_string_to_JSON_string(char * p_json_string, size_t max_json_length, const char * const p_key, const char * const p_string, bool is_first);
size_t add_scheduler_to_JSON_string(char * p_json_buff, size_t max_json_length, const scheduler_t * const p_scheduler);

bool JSON_query_number(const char * const p_json_buff, const size_t json_len, const char * const p_query_str, int32_t * const p_value);
bool JSON_query_RTC(const char * const p_json_buff, const size_t json_len, const char * const p_query_str, rtc_t * const p_rtc);
bool JSON_query_action(const char * const p_json_buff, const size_t json_len, const char * const p_query_str, scheduler_action_t * p_action_type, scheduler_action_param_t * p_act_param);
bool JSON_to_scheduler(scheduler_t * const p_scheduler, const char * const p_json_str, const size_t json_len);
#endif