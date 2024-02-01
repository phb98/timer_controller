#include "scheduler_file.h"
#include "littlefs_port.h"
#include "console.h"
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#include "core_json.h"
#include "scheduler_JSON.h"
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
#define JSON_BUFFER_LENGTH_MAX    (512)
#define SCHEDULER_FILE_VERSION    (SCHEDULER_FILE_MAJOR_VER * 100 + SCHEDULER_FILE_MINOR_VER)
#define FILE_HEADER_SIGNATURE     (0xB00B1E5)
typedef struct 
{
  uint32_t signature;
}scheduler_file_header_t;
// Private function prototype

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
  // scheduler_t test_sch = 
  // {
  //   .trig_time = 
  //   {
  //     .date = {
  //       .year = 2024,
  //       .month = 2,
  //       .day = 1
  //     },
  //     .time = {
  //       .hour = 10,
  //       .minute = 1,
  //       .second = 52
  //     },
  //   },
  //   .action = SCHEDULER_ACTION_OUTPUT,
  //   .act_param = 
  //   {
  //     .output = {
  //       .channel = 1,
  //       .value = 1
  //     }
  //   }
  // };
  // scheduler_file_new_scheduler(&test_sch);
}

/**
 * @return true if add success
 * @false false if add fail
*/
bool scheduler_file_new_scheduler(const scheduler_t * const p_new_scheduler)
{
  bool ret = true;
  if(!p_new_scheduler)
  {
    ret = false;
    CONSOLE_LOG_ERROR("Invalid param");
    return ret;
  }
  // Create a new JSON string
  char * p_json_buff = JSON_buffer;
  JSONStatus_t result;
  memset(p_json_buff, 0x0, JSON_BUFFER_LENGTH_MAX);
  JSON_START_NEW(p_json_buff);
  p_json_buff += add_scheduler_to_JSON_string(p_json_buff, JSON_BUFFER_LENGTH_MAX, p_new_scheduler);
  JSON_END_NEW(p_json_buff);
  // Validate if new json is valid
  result = JSON_Validate(JSON_buffer, p_json_buff - JSON_buffer);
  CONSOLE_LOG_VERBOSE("New scheduler JSON, len:%d", p_json_buff - JSON_buffer);
  CONSOLE_LOG_VERBOSE("%s", JSON_buffer);
  if(result == JSONSuccess)
  {
    CONSOLE_LOG_DEBUG("Validate JSON success");
  }
  else
  {
    CONSOLE_LOG_ERROR("Validate JSON fail");
    ret = false;
  }
  return ret;
}


// Private function
/**
 * @details Validate the current scheduler file in file system
 * @return true if file exist and non-corrupted
 * @return false if file non-exist or corrupted
*/
static bool file_validate()
{
  bool ret = false;

  return ret;
}
