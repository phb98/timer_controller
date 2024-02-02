#include "scheduler_JSON.h"
#include "console.h"
#include "scheduler.h"
#include "core_json.h"

// PRivate function prototype 
static int32_t str2int(const char* str, int len);
static bool action_string_to_action_type(const char * act_str, scheduler_action_t * const p_act_type);
// Public function
size_t add_scheduler_to_JSON_string(char * p_json_buff, size_t max_json_length, const scheduler_t * const p_scheduler)
{
  if(!p_scheduler || !p_scheduler || max_json_length < 2)
  {
    CONSOLE_LOG_ERROR("Invalid Param");
    return 0;
  }
  const char * p_json_start = p_json_buff;
  const char * p_json_end = p_json_buff + max_json_length;
  const rtc_t * p_trig_time = &(p_scheduler->trig_time);
  const scheduler_action_param_t * p_act_param = &(p_scheduler->act_param);
  p_json_buff += add_start_object_to_JSON_string(p_json_buff,     p_json_end - p_json_buff, "trig_time", true);
  p_json_buff += add_RTC_to_JSON_string(p_json_buff,              p_json_end - p_json_buff, p_trig_time);
  p_json_buff += add_end_object_to_JSON_string(p_json_buff,       p_json_end - p_json_buff);
  p_json_buff += add_number_to_JSON_string(p_json_buff,           p_json_end - p_json_buff, "dow_mask", p_scheduler->dow_mask, false);
  p_json_buff += add_number_to_JSON_string(p_json_buff,           p_json_end - p_json_buff, "month_mask", p_scheduler->month_mask, false);
  p_json_buff += add_start_object_to_JSON_string(p_json_buff,     p_json_end - p_json_buff, "action", false);
  p_json_buff += add_scheduler_action_to_JSON_string(p_json_buff, p_json_end - p_json_buff, p_scheduler->action, p_act_param, false);
  p_json_buff += add_end_object_to_JSON_string(p_json_buff,       p_json_end - p_json_buff);
  return p_json_buff - p_json_start;
}

/**
 * @return number of byte added to buffer
*/
size_t add_start_object_to_JSON_string(char * const p_json_buffer, size_t max_json_length, const char * p_key_string, bool is_first)
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

size_t add_end_object_to_JSON_string(char * const p_json_buffer, size_t max_json_length)
{
  if(max_json_length)
  {
    *p_json_buffer = '}';
    return 1;
  }
  return 0;
}

size_t add_number_to_JSON_string(char * p_json_buff, size_t json_buff_len, const char * const p_key, int value, bool is_first)
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

size_t add_RTC_to_JSON_string(char * p_json_string, size_t max_json_length, const rtc_t * const p_rtc)
{
  if(!p_rtc || !p_json_string || max_json_length < 0);
  char * const p_start_json = p_json_string;
  char * const p_end_json   = p_json_string + max_json_length;
  // Date object
  p_json_string += add_start_object_to_JSON_string(p_json_string, p_end_json - p_json_string, "date", true);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "year",  p_rtc->date.year, true);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "month", p_rtc->date.month, false);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "day",   p_rtc->date.day, false);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "dow",   p_rtc->date.DoW, false);
  p_json_string += add_end_object_to_JSON_string(p_json_string,   p_end_json - p_json_string);
  // Time object
  p_json_string += add_start_object_to_JSON_string(p_json_string, p_end_json - p_json_string, "time", false);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "hour",   p_rtc->time.hour,  true);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "minute", p_rtc->time.minute, false);
  p_json_string += add_number_to_JSON_string(p_json_string,       p_end_json - p_json_string, "second", p_rtc->time.second,  false);
  p_json_string += add_end_object_to_JSON_string(p_json_string,   p_end_json - p_json_string);

  return p_json_string - p_start_json;
}

size_t add_string_to_JSON_string(char * p_json_string, size_t max_json_length, const char * const p_key, const char * const p_string, bool is_first)
{
  if(!p_json_string || !max_json_length || !p_string)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return 0;
  }
  char * p_start_json = p_json_string;
  if(is_first)
  {
    *p_json_string = ',';
    p_json_string++;
    max_json_length--;
  }
  p_json_string += snprintf(p_json_string, max_json_length, "\"%s\":\"%s\"", p_key, p_string);
  return p_json_string - p_start_json;
}

size_t add_scheduler_action_to_JSON_string(char * p_json_string, size_t max_json_length, const scheduler_action_t action, const scheduler_action_param_t * p_param, bool is_first)
{
  if(!p_json_string || !max_json_length || !p_param || (action >= NUM_OF_SCHEDULER_ACTION))
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return 0;
  }
  char * p_json_start = p_json_string;
  char * p_json_end = p_json_string + max_json_length;
  p_json_string += add_string_to_JSON_string(p_json_string, p_json_end - p_json_string, "type", scheduler_action_str[action], false); // Type string
  p_json_string += add_start_object_to_JSON_string(p_json_string, p_json_end - p_json_string, "param", is_first); // param object
  switch(action)
  {
    case SCHEDULER_ACTION_OUTPUT:
    {
      p_json_string += add_number_to_JSON_string(p_json_string, p_json_end - p_json_string, "channel", p_param->output.channel, true);
      p_json_string += add_number_to_JSON_string(p_json_string, p_json_end - p_json_string, "value", p_param->output.value, false);
      break;
    }
    default:
      break;
  }
  p_json_string += add_end_object_to_JSON_string(p_json_string, p_json_end - p_json_string); // End param object
  return p_json_string - p_json_start;
}

bool JSON_query_number(const char * const p_json_buff, const size_t json_len, const char * const p_query_str, int32_t * const p_value)
{
  ASSERT_LOG_ERROR_RETURN_VAL((p_json_buff && p_query_str && p_value && json_len), false, "Invalid Param");
  JSONStatus_t json_stat = JSON_Validate(p_json_buff, json_len);
  JSONTypes_t json_type;
  ASSERT_LOG_ERROR_RETURN_VAL(json_stat == JSONSuccess, false, "JSON validate fail:%d", json_stat);
  size_t value_len = 0;
  char * p_json_number_str;
  json_stat = JSON_SearchT(p_json_buff, json_len, p_query_str, strlen(p_query_str), &p_json_number_str, &value_len, &json_type);
  ASSERT_LOG_ERROR_RETURN_VAL(json_stat == JSONSuccess, false, "JSON query fail:%d", json_stat);
  ASSERT_LOG_ERROR_RETURN_VAL(json_type == JSONNumber, false, "JSON unexpected type, %d", json_type);
  *p_value = str2int(p_json_number_str, value_len);
  return true;
}

bool JSON_query_RTC(const char * const p_json_buff, const size_t json_len, const char * const p_query_str, rtc_t * const p_rtc)
{
  ASSERT_LOG_ERROR_RETURN_VAL((p_json_buff && p_query_str && p_rtc && json_len), false, "Invalid Param");
  JSONStatus_t json_stat = JSON_Validate(p_json_buff, json_len);
  ASSERT_LOG_ERROR_RETURN_VAL(json_stat == JSONSuccess, false, "JSON validate fail:%d", json_stat);

  JSONTypes_t json_type;
  bool ret = true;
  size_t json_rtc_len = 0;
  char * p_json_rtc_str;
  json_stat = JSON_SearchT(p_json_buff, json_len, p_query_str, strlen(p_query_str), &p_json_rtc_str, &json_rtc_len, &json_type);
  ASSERT_LOG_ERROR_RETURN_VAL(json_stat == JSONSuccess, false, "JSON query fail:%d", json_stat);
  ASSERT_LOG_ERROR_RETURN_VAL(json_type == JSONObject, false, "JSON unexpected type, %d", json_type);
  // Query RTC
  int32_t temp_num;
  ret = JSON_query_number(p_json_rtc_str, json_rtc_len, "date.year", (int32_t*)&(temp_num));
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  p_rtc->date.year = temp_num;
  ret = JSON_query_number(p_json_rtc_str, json_rtc_len, "date.month", (int32_t*)&(temp_num));
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  p_rtc->date.month = temp_num;
  ret = JSON_query_number(p_json_rtc_str, json_rtc_len, "date.day", (int32_t*)&(temp_num));
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  p_rtc->date.day = temp_num;
  ret = JSON_query_number(p_json_rtc_str, json_rtc_len, "date.dow", (int32_t*)&(temp_num));
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  p_rtc->date.DoW = temp_num;
  ret = JSON_query_number(p_json_rtc_str, json_rtc_len, "time.hour", (int32_t*)&(temp_num));
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  p_rtc->time.hour = temp_num;
  ret = JSON_query_number(p_json_rtc_str, json_rtc_len, "time.minute", (int32_t*)&(temp_num));
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  p_rtc->time.minute = temp_num;
  ret = JSON_query_number(p_json_rtc_str, json_rtc_len, "time.second", (int32_t*)&(temp_num));
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  p_rtc->time.second = temp_num;
  return ret;
}

bool JSON_query_action(const char * const p_json_buff, const size_t json_len, const char * const p_query_str, scheduler_action_t * p_action_type, scheduler_action_param_t * p_act_param)
{
  ASSERT_LOG_ERROR_RETURN_VAL((p_json_buff && p_query_str && p_action_type && json_len && p_act_param), false, "Invalid Param");
  JSONTypes_t json_type;
  bool ret = true;
  size_t json_action_len = 0;
  char * p_json_action_str;
  JSONStatus_t json_stat;
  int32_t temp_num;
  // Check and Get action object
  json_stat = JSON_SearchT(p_json_buff, json_len, p_query_str, strlen(p_query_str), &p_json_action_str, &json_action_len, &json_type);
  ASSERT_LOG_ERROR_RETURN_VAL(json_stat == JSONSuccess, false, "JSON query fail:%d", json_stat);
  ASSERT_LOG_ERROR_RETURN_VAL(json_type == JSONObject, false, "JSON unexpected type, %d", json_type);
  char * json_action_type_str;
  size_t temp_len = sizeof(json_action_type_str);
  // Check and get Action type string
  json_stat = JSON_SearchT(p_json_action_str, json_action_len, "type", strlen("type"), &json_action_type_str, &temp_len, &json_type);
  ASSERT_LOG_ERROR_RETURN_VAL(json_stat == JSONSuccess, false, "JSON query fail:%d", json_stat);
  ASSERT_LOG_ERROR_RETURN_VAL(json_type == JSONString, false, "JSON unexpected type, %d", json_type);
  if(!action_string_to_action_type(json_action_type_str, p_action_type))
  {
    CONSOLE_LOG_ERROR("Fail to parse action type");
    return false;
  }
  // Check and get action param
  switch(*p_action_type)
  {
    case SCHEDULER_ACTION_OUTPUT:
    {
      char * p_query = "param.channel";
      char * p_json_number_str;
      bool temp_ret;
      temp_ret = JSON_query_number(p_json_action_str, json_action_len, "param.channel", &temp_num);
      ASSERT_LOG_ERROR_RETURN_VAL(temp_ret, false, "JSON query fail");
      p_act_param->output.channel = temp_num;
      temp_ret = JSON_query_number(p_json_action_str, json_action_len, "param.value", &temp_num);
      ASSERT_LOG_ERROR_RETURN_VAL(temp_ret, false, "JSON query fail");
      p_act_param->output.value = temp_num;
      break;
    }
    default:
    {
      CONSOLE_LOG_WARN("Unknow action type:%d", *p_action_type);
      return false;
      break;
    }
  }
  return true;
}
bool JSON_to_scheduler(scheduler_t * const p_scheduler, const char * const p_json_str, const size_t json_len)
{
  ASSERT_LOG_ERROR_RETURN_VAL(p_scheduler && p_json_str && json_len, false, "Invalid param");
  bool ret = true;
  int32_t temp_num = 0;
  ret = JSON_query_RTC(p_json_str, json_len, "trig_time", &(p_scheduler->trig_time));
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");

  ret = JSON_query_number(p_json_str, json_len, "dow_mask", &temp_num);
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  p_scheduler->dow_mask = temp_num;
  
  ret = JSON_query_number(p_json_str, json_len, "month_mask", &temp_num);
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  p_scheduler->month_mask = temp_num;

  ret = JSON_query_action(p_json_str, json_len, "action", &(p_scheduler->action), &(p_scheduler->act_param));
  ASSERT_LOG_ERROR_RETURN_VAL(ret, false, "Query fail");
  return ret;
}
// Private function
static int32_t str2int(const char* str, int len)
{
  int i;
  int ret = 0;
  for(i = 0; i < len; ++i)
  {
    ret = ret * 10 + (str[i] - '0');
  }
  return ret;
}

static bool action_string_to_action_type(const char * act_str, scheduler_action_t * const p_act_type)
{
  for(int i = 0; i < NUM_OF_SCHEDULER_ACTION; i++)
  {
    if(memcmp(act_str, scheduler_action_str[i], strlen(scheduler_action_str[i])) == 0)
    {
      *p_act_type = i;
      return true;
    }
  }
  return false;
}