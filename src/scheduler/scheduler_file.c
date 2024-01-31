#include "scheduler_file.h"
#include "littlefs_port.h"
#include "console.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "core_json.h"
// FILE FORMAT
/**
 * [HEADER][JSON_INFO_1][JSON_1][JSON_INFO_2][JSON_2]...
*/
/**
 * SAMPLE JSON
{
  "trig_time": {
    "date":{
      "year":2024,
      "month":12,
      "day":2
    },
    "time":{
      "hour":23,
      "minute":9,
      "second":10
    }
  },
  "dow_mask":7,
  "month_mask":7,
  "action":{
    "type":"output",
    "param":{
      "channel":10,
      "value":20
    }
  }
}
*/
#define JSON_BUFFER_LENGTH_MAX (512)
#define JSON_START_NEW(p)     (*p++ = '{')
#define JSON_END_NEW(p)       (*p++ = '}')
// Private function prototype
static size_t add_object_to_JSON_string(char * const p_json_buffer, size_t max_json_length, const char * p_key_string, const char * const p_val_string, bool is_first);
static size_t add_start_object_to_JSON_string(char * const p_json_buffer, size_t max_json_length, const char * p_key_string, bool is_first);
static size_t add_end_object_to_JSON_string(char * const p_json_buffer, size_t max_json_length);
static size_t add_number_to_JSON_string(char * p_json_buff, size_t json_buff_len, const char * const p_key, int value, bool is_first);
static size_t add_RTC_to_JSON_string(char * p_json_string, size_t max_json_length, const rtc_t * const p_rtc);
static lfs_t * p_lfs = NULL;
static SemaphoreHandle_t file_mutex;
static char JSON_buffer[JSON_BUFFER_LENGTH_MAX];
void scheduler_file_init()
{
  CONSOLE_LOG_INFO("Init Scheduler file");
  p_lfs = lfs_port_get_lfs_handle();
  assert(p_lfs); // This is critical, the whole device fail without file system
  file_mutex = xSemaphoreCreateMutex();
  assert(file_mutex);

  // Test
  char * p_json_buff = JSON_buffer;
  rtc_t test_rtc = {0};
  test_rtc.date.year =1234;
  test_rtc.date.month = 01;
  test_rtc.date.day = 30;
  JSON_START_NEW(p_json_buff);
  p_json_buff += add_RTC_to_JSON_string(p_json_buff, 512, &test_rtc);
  JSON_END_NEW(p_json_buff);
  JSONStatus_t result;
  result = JSON_Validate( JSON_buffer, p_json_buff - JSON_buffer);
  CONSOLE_LOG_DEBUG("%s", JSON_buffer);
}

/**
 * @return true if add success
 * @false false if add fail
*/
bool scheduler_file_new_scheduler(const scheduler_t * const p_new_scheduler)
{
  bool ret = false;
  if(!p_new_scheduler)
  {
    ret = false;
    CONSOLE_LOG_ERROR("Invalid param");
    return ret;
  }
  
}


// Private function
/**
 * @details Validate the current scheduler file in file system
 * @return true if file exist and non-corrupted
 * @return false if file non-exist or corrupted
*/
static bool file_validate()
{

}
static bool scheduler_to_JSON_string(const scheduler_t * const p_scheduler, char * const p_json_string, size_t max_json_length)
{
  bool ret = false;
  if(!p_scheduler || !p_scheduler || max_json_length < 2)
  {
    ret = false;
    CONSOLE_LOG_ERROR("Invalid Param");
    return ret;
  }
  // Zero out the string to prevent garbage
  memset(p_json_string, 0x0, max_json_length);
}

/**
 * @return number of byte added to buffer
*/
static size_t add_start_object_to_JSON_string(char * const p_json_buffer, size_t max_json_length, const char * p_key_string, bool is_first)
{
  char * p_buff_start = p_json_buffer;
  char * p_buff_end   = p_json_buffer + max_json_length;
  if(!p_json_buffer || !max_json_length || !p_key_string)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return 0;
  }
  if(!is_first) 
  {
    p_buff_start[0] = ',';
    p_buff_start++;
  }
  p_buff_start += (snprintf(p_buff_start, max_json_length, "\"%s\":{", p_key_string)); 
  return p_buff_start - p_json_buffer;
}
static size_t add_end_object_to_JSON_string(char * const p_json_buffer, size_t max_json_length)
{
  if(max_json_length)
  {
    *p_json_buffer = '}';
    return 1;
  }
}
static size_t add_number_to_JSON_string(char * p_json_buff, size_t json_buff_len, const char * const p_key, int value, bool is_first)
{
  size_t json_length = 0;
  if(!p_json_buff || !json_buff_len || !p_key)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return 0;
  }
  if(!is_first)
  {
    p_json_buff[0] = ',';
    p_json_buff++;
    json_length++;
  }
  json_length += (snprintf(p_json_buff, json_buff_len, "\"%s\":%ld", p_key, value));
  return json_length;
}
static size_t add_RTC_to_JSON_string(char * p_json_string, size_t max_json_length, const rtc_t * const p_rtc)
{
  if(!p_rtc || !p_json_string || max_json_length < 0);
  char * const p_start_json = p_json_string;
  char * const p_end_json   = p_json_string + max_json_length;
  // Date object
  p_json_string += add_start_object_to_JSON_string(p_json_string, p_end_json - p_json_string, "date", true);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "year",  p_rtc->date.year, true);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "month", p_rtc->date.month, false);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "day",   p_rtc->date.day, false);
  p_json_string += add_end_object_to_JSON_string(p_json_string,   p_end_json - p_json_string);
  // Time object
  p_json_string += add_start_object_to_JSON_string(p_json_string, p_end_json - p_json_string, "time", false);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "hour",   p_rtc->time.hour,  true);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "minute", p_rtc->time.minute, false);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "second", p_rtc->time.second,  false);
  p_json_string += add_end_object_to_JSON_string(p_json_string,   p_end_json - p_json_string);

  return p_json_string - p_start_json;
}

static size_t add_scheduler_action_to_JSON_string(scheduler_action_t action, scheduler_action_param_t * p_param)
{
  
}