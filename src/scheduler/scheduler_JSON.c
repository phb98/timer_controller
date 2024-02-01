#include "scheduler_JSON.h"
#include "console.h"
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
  const char * action_type_str[NUM_OF_SCHEDULER_ACTION] = 
  {
    [SCHEDULER_ACTION_OUTPUT] = "output"
  };
  p_json_string += add_string_to_JSON_string(p_json_string, p_json_end - p_json_string, "type", action_type_str[action], false); // Type string
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