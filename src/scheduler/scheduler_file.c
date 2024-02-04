#define MODULE_CONSOLE_LOG_LEVEL CONSOLE_LOG_LEVEL_DEBUG
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
#define JSON_BUFFER_LENGTH_MAX                           (256)
#define FILE_HEADER_SIGNATURE                            (0xB00B1E5)
#define ENTRY_SIGNATURE                                  (0xC0FFEE)
#define SCHEDULER_FILE_VERSION                           (SCHEDULER_FILE_MAJOR_VER * 100 + SCHEDULER_FILE_MINOR_VER)
#define SCHEDULER_FILE_VERSION_CHECK_COMPATIBILITY(a, b) ((((int)a - (int)b) >= 100) || (((int)a - (int)b) < 100))
#define SCHEDULER_FILE_PATH                              ("/System/Scheduler")
#define SCHEDULER_FILE_TEMP_PATH                         ("/System/Scheduler_TEMP")

#define FILE_LOCK()                                      (xSemaphoreTakeRecursive(file_mutex, portMAX_DELAY))
#define FILE_UNLOCK()                                    (xSemaphoreGiveRecursive(file_mutex))
typedef struct __attribute__ ((__packed__))
{
  uint32_t file_version;
  uint8_t  RESERVED[16];
  uint32_t signature; // Should be at the bottom so if we add or remove any field, it will invalid the signature
}scheduler_file_header_t;
typedef struct  __attribute__ ((__packed__))
{
  uint32_t json_len;
  uint8_t RESERVED[16]; 
  uint32_t signature;
}scheduler_file_entry_t;
// Private function prototype
static bool     scheduler_file_validate(const char * const file_path);
static bool     scheduler_file_delete(const char * const file_path);
static bool     scheduler_file_append(const char * const file_path, const uint8_t * const p_buff, const size_t len);
static bool     scheduler_file_read(const char * const file_path, size_t read_idx, uint8_t * const p_buff, const size_t len);
static size_t   scheduler_file_size(const char * const file_path);
static bool     scheduler_file_new_file(const char * const file_path);
static bool     scheduler_file_get_single_scheduler(const char * const file_path, const uint8_t entry_idx, scheduler_t * const p_scheduler);
static int32_t  sheduler_file_get_num_entry(char * const file_path);
static bool     is_entry_equal(const scheduler_t * p_sch_1, const scheduler_t * p_sch_2);
// Private variables
static lfs_t * p_lfs = NULL;
static SemaphoreHandle_t file_mutex;
static char JSON_buffer[JSON_BUFFER_LENGTH_MAX];
static lfs_file_t sch_file;
// Public function
void scheduler_file_init()
{
  CONSOLE_LOG_INFO("Init Scheduler file");
  p_lfs = lfs_port_get_lfs_handle();
  assert(p_lfs); // This is critical, the whole device fail without file system
  file_mutex = xSemaphoreCreateMutex();
  assert(file_mutex);
  // Validate current tile
  if(!scheduler_file_validate(SCHEDULER_FILE_PATH))
  {
    CONSOLE_LOG_ERROR("File validate fail");
    scheduler_file_delete(SCHEDULER_FILE_PATH); // delete corrupted file, if file not exist it does not harm anything
    scheduler_file_new_file(SCHEDULER_FILE_PATH);
  }
  else
  {
    CONSOLE_LOG_INFO("Validate scheduler file success");
  }
  // Test
  #if 0
  scheduler_file_delete(SCHEDULER_FILE_PATH); // delete corrupted file, if file not exist it does not harm anything
  scheduler_file_new_file(SCHEDULER_FILE_PATH);
  scheduler_t test_sch = 
  {
    .trig_time = 
    {
      .date = {
        .year = 2024,
        .month = 2,
        .day = 1,
        .DoW = 0,
      },
      .time = {
        .hour = 10,
        .minute = 1,
        .second = 52
      },
    },
    .month_mask = 0xFFFF,
    .dow_mask = 0xFF,
    .action = SCHEDULER_ACTION_OUTPUT,
    .act_param = 
    {
      .output = {
        .channel = 1,
        .value = 1234,
      }
    }
  };
  for(int i = 0; i < 10; i++)
  {
    test_sch.act_param.output.channel = i;
    scheduler_file_new_scheduler(&test_sch);
  }
  scheduler_t sch2;
  scheduler_file_get_single_scheduler(SCHEDULER_FILE_PATH, 0, &sch2);
  sch2.act_param.output.channel = 3;
  scheduler_ret_t ret = scheduler_file_check_exist(&sch2);
  int32_t num = sheduler_file_get_num_entry(SCHEDULER_FILE_PATH);
  #endif
}

/**
 * @return SCHEDULER_OK if add success
 * @return other if add fail
*/
scheduler_ret_t scheduler_file_new_scheduler(const scheduler_t * const p_new_scheduler)
{
  bool ret = SCHEDULER_OK;
  size_t json_len = 0;
  JSONStatus_t result = 0;
  if(!p_new_scheduler)
  {
    ret = SCHEDULER_ERR_PARAM;
    CONSOLE_LOG_ERROR("Invalid param");
    return ret;
  }
  // Create a new JSON string
  char * p_json_buff = JSON_buffer + sizeof(scheduler_file_entry_t); // save space for header
  memset(JSON_buffer, 0x0, JSON_BUFFER_LENGTH_MAX);
  JSON_START_NEW(p_json_buff);
  p_json_buff += add_scheduler_to_JSON_string(p_json_buff, JSON_BUFFER_LENGTH_MAX - sizeof(scheduler_file_entry_t), p_new_scheduler);
  JSON_END_NEW(p_json_buff);
  json_len = p_json_buff - (JSON_buffer + sizeof(scheduler_file_entry_t));
  // Validate if new json is valid
  result = JSON_Validate(JSON_buffer + sizeof(scheduler_file_entry_t), json_len);
  CONSOLE_LOG_VERBOSE("New scheduler JSON, len:%d", json_len);
  CONSOLE_LOG_VERBOSE("%s", JSON_buffer + sizeof(scheduler_file_entry_t));
  if(result == JSONSuccess)
  {
    CONSOLE_LOG_DEBUG("Validate new JSON success");
    // Write new json to file
    scheduler_file_entry_t * p_new_entry = (scheduler_file_entry_t *)JSON_buffer;
    p_new_entry->json_len  = json_len;
    p_new_entry->signature = ENTRY_SIGNATURE;
    if(!scheduler_file_append(SCHEDULER_FILE_PATH, JSON_buffer, json_len + sizeof(scheduler_file_entry_t)))
    {
      CONSOLE_LOG_ERROR("Append JSON to file fail");
      ret = SCHEDULER_ERR_INTERNAL;
    }
  }
  else
  {
    CONSOLE_LOG_ERROR("Validate JSON fail");
    ret = SCHEDULER_ERR_INTERNAL;
  }
  return ret;
}
// @return SCHEDULER_OK if not exist
// @return SCHEDULER_ERR_DUPLICATE if exist
scheduler_ret_t scheduler_file_check_exist(const scheduler_t * const p_scheduler)
{
  ASSERT_LOG_ERROR_RETURN_VAL(p_scheduler, SCHEDULER_ERR_PARAM, "Invalid param");
  int num_entry = sheduler_file_get_num_entry(SCHEDULER_FILE_PATH);
  scheduler_t checking_entry = {0};
  if(num_entry < 0)
  {
    return SCHEDULER_ERR_INTERNAL;
  }
  for(int i = 0; i < num_entry; i++)
  {
    if(!scheduler_file_get_single_scheduler(SCHEDULER_FILE_PATH, i, &checking_entry))
    {
      CONSOLE_LOG_ERROR("Get single entry fail");
      return SCHEDULER_ERR_INTERNAL;
    }
    if(is_entry_equal(p_scheduler, &checking_entry))
    {
      return SCHEDULER_ERR_DUPLICATE;
    }
  }
  return SCHEDULER_OK;
}

scheduler_ret_t scheduler_file_delete_all()
{
  CONSOLE_LOG_WARN("Delete all scheduler");
  scheduler_file_delete(SCHEDULER_FILE_PATH);
  return  (scheduler_file_new_file(SCHEDULER_FILE_PATH) ? SCHEDULER_OK : SCHEDULER_ERR_INTERNAL);
}
scheduler_ret_t scheduler_file_get_num_sch(uint32_t * p_num)
{
  ASSERT_LOG_ERROR_RETURN_VAL(p_num, SCHEDULER_ERR_PARAM, "Invalid param");
  int32_t num_entry = sheduler_file_get_num_entry(SCHEDULER_FILE_PATH);
  if(num_entry >= 0) *p_num = num_entry;
  return (num_entry >= 0 ? SCHEDULER_OK : SCHEDULER_ERR_INTERNAL);
}
scheduler_ret_t scheduler_file_get_scheduler(scheduler_t * p_sch, uint32_t sch_idx)
{
  if(scheduler_file_get_single_scheduler(SCHEDULER_FILE_PATH, sch_idx, p_sch))
  {
    return SCHEDULER_OK;
  }
  return SCHEDULER_ERR_NOT_FOUND;
}

/**
 * @note if p_new_sch is NULL, the entry at sch_idx will be delete
*/
scheduler_ret_t scheduler_file_modify(scheduler_t * const p_new_sch, uint32_t sch_idx)
{
  // only modify if needed
  scheduler_t temp_sch;
  if(p_new_sch && (false == scheduler_file_get_single_scheduler(SCHEDULER_FILE_PATH, sch_idx, &temp_sch)))
  {
    return SCHEDULER_ERR_NOT_FOUND;
  }
  if(p_new_sch && is_entry_equal(p_new_sch, &temp_sch))
  {
    CONSOLE_LOG_WARN("No change");
    return SCHEDULER_OK;
  }
  uint32_t                file_idx = 0;
  size_t                  file_size = scheduler_file_size(SCHEDULER_FILE_PATH);
  char                    file_buff[JSON_BUFFER_LENGTH_MAX] = {0};
  scheduler_file_entry_t  *p_entry = (scheduler_file_entry_t *)&file_buff[0];
  char                    * const p_json_buff = file_buff + sizeof(scheduler_file_entry_t);
  uint32_t                num_entry_checked = 0;
  FILE_LOCK();
  // Create temp file to write entry
  lfs_file_t temp_file = {0};
  do
  {
    if(lfs_file_open(p_lfs, &temp_file, SCHEDULER_FILE_TEMP_PATH, LFS_O_CREAT | LFS_O_WRONLY | LFS_O_TRUNC) != LFS_ERR_OK)
    {
      CONSOLE_LOG_ERROR("File open fail");
      break;
    }
    // Write file header to temp file
    scheduler_file_new_file(SCHEDULER_FILE_TEMP_PATH);
    file_idx = sizeof(scheduler_file_header_t);
    while(file_idx < file_size)
    {
      if(!scheduler_file_read(SCHEDULER_FILE_PATH, file_idx, (uint8_t*) p_entry, sizeof(scheduler_file_entry_t)))
      {
        CONSOLE_LOG_ERROR("Read fail");
        lfs_file_close(p_lfs, &temp_file);
        lfs_remove(p_lfs, SCHEDULER_FILE_TEMP_PATH);
        FILE_UNLOCK();
        return SCHEDULER_ERR_INTERNAL;
      }
      file_idx += sizeof(scheduler_file_entry_t);
      if(num_entry_checked == sch_idx)
      {
        file_idx += p_entry->json_len;
        if(p_new_sch != NULL)
        {
          char * p_write_json = p_json_buff;
          JSON_START_NEW(p_write_json);
          p_write_json += add_scheduler_to_JSON_string(p_write_json, sizeof(file_buff) - sizeof(scheduler_file_entry_t), p_new_sch);
          JSON_END_NEW(p_write_json);
          p_entry->json_len = p_write_json - p_json_buff;
          if(JSONSuccess != JSON_Validate(p_json_buff, p_entry->json_len))
          {
            CONSOLE_LOG_ERROR("Validate JSON fail");
          }
          scheduler_file_append(SCHEDULER_FILE_TEMP_PATH, file_buff, sizeof(scheduler_file_entry_t) + p_entry->json_len);
        }
      }
      else
      {
        scheduler_file_read(SCHEDULER_FILE_PATH, file_idx, p_json_buff, p_entry->json_len);
        scheduler_file_append(SCHEDULER_FILE_TEMP_PATH, file_buff, sizeof(scheduler_file_entry_t) + p_entry->json_len);
        file_idx += p_entry->json_len;
      }
      num_entry_checked++;
    }
  } while(0);
  lfs_file_close(p_lfs, &temp_file);
  lfs_remove(p_lfs, SCHEDULER_FILE_PATH); // Remove original file
  FILE_UNLOCK();
  if(LFS_ERR_OK == lfs_rename(p_lfs, SCHEDULER_FILE_TEMP_PATH, SCHEDULER_FILE_PATH))
  {
    return scheduler_file_validate(SCHEDULER_FILE_PATH) ? SCHEDULER_OK : SCHEDULER_ERR_INTERNAL;
  }
  return SCHEDULER_ERR_INTERNAL;
}
// Private function
/**
 * @details Validate the current scheduler file in file system
 * @return true if file exist and non-corrupted
 * @return false if file non-exist or corrupted
*/
static bool scheduler_file_validate(const char * const file_path)
{
  bool ret = false;
  FILE_LOCK();
  int lfs_ret = 0;
  int file_size = 0;
  uint32_t file_idx = 0;
  uint32_t num_checked_entry = 0;
  static char checking_json_buffer[JSON_BUFFER_LENGTH_MAX] = {0}; // consider using malloc here
  scheduler_file_entry_t checking_entry = {0};

  lfs_ret = lfs_file_open(p_lfs, &sch_file, file_path, LFS_O_RDONLY); // Open scheduler file as read-only. Validation should not write anything
  if(lfs_ret != LFS_ERR_OK)
  {
    CONSOLE_LOG_WARN("Fail to open file:%s", lfs_port_get_err_string(lfs_ret));
    ret = false;
  }
  else
  {
    do 
    {
      // Validate file header
      scheduler_file_header_t file_header = {0};
      lfs_ret   = lfs_file_read(p_lfs, &sch_file, &file_header, sizeof(scheduler_file_header_t));
      file_size = lfs_file_size(p_lfs, &sch_file);
      lfs_ret   = lfs_file_close(p_lfs, &sch_file);
      if(file_size < sizeof(scheduler_file_header_t))
      {
        CONSOLE_LOG_WARN("File size invalid");
        break;
      }
      if(file_header.signature != FILE_HEADER_SIGNATURE)
      {
        CONSOLE_LOG_WARN("Invalid file header:0x%x", file_header.signature);
        break;
      }
      if(!SCHEDULER_FILE_VERSION_CHECK_COMPATIBILITY(file_header.file_version, SCHEDULER_FILE_VERSION))
      {
        CONSOLE_LOG_WARN("Scheduler file incompatible, saved:%lu, fw:%lu", file_header.file_version, SCHEDULER_FILE_VERSION);
        break;
      }
      // Validate each entry
      ret = true; // set it to true because there maybe no entry in file, so it's auto valid
      file_idx = sizeof(scheduler_file_header_t); // move file idx to end of header (it's pointing to start of entry 0 now);
      while(file_idx < file_size)
      {
        memset(&checking_entry, 0x0, sizeof(scheduler_file_entry_t));
        scheduler_file_read(file_path, file_idx, (uint8_t *)&checking_entry, sizeof(scheduler_file_entry_t));
        if(checking_entry.signature != ENTRY_SIGNATURE)
        {
          ret = false;
          CONSOLE_LOG_WARN("Invalid entry signature:%d", num_checked_entry);
          break;
        }
        file_idx += sizeof(scheduler_file_entry_t);
        // Check if entry is valid JSON
        scheduler_file_read(SCHEDULER_FILE_PATH, file_idx, checking_json_buffer, checking_entry.json_len);
        JSONStatus_t json_stat = JSON_Validate(checking_json_buffer, checking_entry.json_len);
        if(json_stat != JSONSuccess)
        {
          ret = false;
          CONSOLE_LOG_WARN("Invalid entry json:%d", num_checked_entry);
          break;
        }
        // Entry is valid, move to next entry
        file_idx += checking_entry.json_len;
        num_checked_entry++;
        ret = true;
      }
      if(ret)
      {
        CONSOLE_LOG_INFO("Scheduler file version:%lu", file_header.file_version);
        CONSOLE_LOG_INFO("Num saved entry:%lu", num_checked_entry);
      }
      else
      {
        CONSOLE_LOG_WARN("Scheduler entries corrupted");
      }
    } while(0);
  }
  FILE_UNLOCK();
  return ret;
}

static bool scheduler_file_delete(const char * const file_path)
{
  FILE_LOCK();
  int lfs_ret = lfs_remove(p_lfs, file_path);
  CONSOLE_LOG_DEBUG("Removed scheduler file, ret:%s", lfs_port_get_err_string(lfs_ret));
  FILE_UNLOCK();
  return (lfs_ret == LFS_ERR_OK);
}

static bool scheduler_file_new_file(const char * const file_path)
{
  FILE_LOCK();
  const scheduler_file_header_t default_file_header = 
  {
    .file_version = SCHEDULER_FILE_VERSION,
    .signature = FILE_HEADER_SIGNATURE,
    .RESERVED = {0},
  };
  int lfs_ret = 0;
  do{
    lfs_ret = lfs_file_open(p_lfs, &sch_file, file_path, LFS_O_WRONLY | LFS_O_CREAT);
    if(lfs_ret != LFS_ERR_OK) break;
    lfs_ret = lfs_file_write(p_lfs, &sch_file, &default_file_header, sizeof(scheduler_file_header_t));
    lfs_ret = lfs_file_close(p_lfs, &sch_file);  
  } while(0);
  CONSOLE_LOG_INFO("Write default header to scheduler file, ret:%s", lfs_port_get_err_string(lfs_ret));
  FILE_UNLOCK();
  return lfs_ret == LFS_ERR_OK ? true : false;
}

static bool scheduler_file_append(const char * const file_path, const uint8_t * const p_buff, const size_t len)
{
  int lfs_ret = 0;
  CONSOLE_LOG_DEBUG("Append to scheduler file:%s", file_path);
  FILE_LOCK();
  do
  {
    lfs_ret = lfs_file_open(p_lfs, &sch_file, file_path, LFS_O_WRONLY | LFS_O_APPEND);
    if(lfs_ret != LFS_ERR_OK) break;
    lfs_ret = lfs_file_write(p_lfs, &sch_file, p_buff, len);
    lfs_ret = lfs_file_close(p_lfs, &sch_file);  
  } while(0);
  CONSOLE_LOG_INFO("Scheduler file append ret:%s", lfs_port_get_err_string(lfs_ret));
  FILE_UNLOCK();
  return (lfs_ret == LFS_ERR_OK);
}

static bool scheduler_file_read(const char * const file_path, size_t read_idx, uint8_t * const p_buff, const size_t len)
{
  int lfs_ret = 0;
  CONSOLE_LOG_VERBOSE("Read from scheduler file:%s", file_path);
  FILE_LOCK();
  do
  {
    lfs_ret = lfs_file_open(p_lfs, &sch_file, file_path, LFS_O_RDONLY);
    if(lfs_ret != LFS_ERR_OK) break;
    lfs_ret = lfs_file_seek(p_lfs, &sch_file, read_idx, LFS_SEEK_SET);
    lfs_ret = lfs_file_read(p_lfs, &sch_file, p_buff, len);
    lfs_ret = lfs_file_close(p_lfs, &sch_file);  
  } while(0);
  CONSOLE_LOG_VERBOSE("Scheduler file read ret:%s", lfs_port_get_err_string(lfs_ret));
  FILE_UNLOCK();
  return (lfs_ret == LFS_ERR_OK);
}

static void scheduler_file_write(const char * const file_path, size_t write_idx, const uint8_t * const p_buff, const size_t len)
{
  ASSERT_LOG_ERROR_RETURN(file_path && p_buff && len, "Invalid_param");
  int lfs_ret = 0;
  CONSOLE_LOG_DEBUG("write to scheduler file:%s", file_path);
  FILE_LOCK();
  do
  {
    lfs_ret = lfs_file_open(p_lfs, &sch_file, file_path, LFS_O_WRONLY);
    if(lfs_ret != LFS_ERR_OK) break;
    lfs_ret = lfs_file_seek(p_lfs, &sch_file, write_idx, LFS_SEEK_SET);
    lfs_ret = lfs_file_write(p_lfs, &sch_file, p_buff, len);
    lfs_ret = lfs_file_close(p_lfs, &sch_file);  
  } while(0);
  CONSOLE_LOG_INFO("Scheduler file write ret:%s", lfs_port_get_err_string(lfs_ret));
  FILE_UNLOCK();
}

static size_t scheduler_file_size(const char * const file_path)
{
  ASSERT_LOG_ERROR_RETURN_VAL(file_path, 0, "Invalid param");
  FILE_LOCK();
  lfs_file_t file;
  lfs_file_open(p_lfs, &file, file_path, LFS_O_RDONLY);
  size_t file_size = lfs_file_size(p_lfs,  &file);
  lfs_file_close(p_lfs, &file);
  return file_size;
}

static bool scheduler_file_get_single_scheduler(const char * const file_path, const uint8_t entry_idx, scheduler_t * const p_scheduler)
{
  ASSERT_LOG_ERROR_RETURN_VAL((file_path && p_scheduler), false, "Invalid Param");
  int file_size;
  size_t file_idx = 0;
  int32_t lfs_err = 0;
  lfs_file_t fil = {0};
  uint32_t num_entry_checked = 0;
  scheduler_file_entry_t entry = {0};
  bool entry_found = false;
  char json_buff[JSON_BUFFER_LENGTH_MAX]; // Consider using malloc, this may lead to stack overflow
  // Check file size first
  FILE_LOCK();
  
  lfs_err = lfs_file_open(p_lfs, &fil, file_path, LFS_O_RDONLY);
  if(lfs_err != LFS_ERR_OK)
  {
    CONSOLE_LOG_ERROR("File open fail:%s", lfs_port_get_err_string(lfs_err));
    return false;
  }
  file_size = lfs_file_size(p_lfs, &fil);
  lfs_file_close(p_lfs, &fil);
  FILE_UNLOCK();
  if(file_size < (sizeof(scheduler_file_header_t) + sizeof(scheduler_file_entry_t)))
  {
    CONSOLE_LOG_ERROR("File size invalid");
    return false;
  }
  // Start at the begin of first entry
  file_idx = sizeof(scheduler_file_header_t);
  while(file_idx < file_size)
  {
    if(!scheduler_file_read(file_path, file_idx, (uint8_t*)&entry, sizeof(scheduler_file_entry_t)))
    {
      CONSOLE_LOG_ERROR("File read fail");
      return false;
    }
    file_idx += sizeof(scheduler_file_entry_t);
    if(entry.signature != ENTRY_SIGNATURE)
    {
      CONSOLE_LOG_ERROR("Invalid entry signature");
      return false;
    }
    if(num_entry_checked == entry_idx)
    {
      // Found our entry, copy to json buffer
      if(!scheduler_file_read(file_path, file_idx, json_buff, entry.json_len))
      {
        CONSOLE_LOG_ERROR("File read fail");
        return false;
      }
      entry_found = true;
      file_idx += entry.json_len;
      break;
    }
    file_idx += entry.json_len; // move to next entry
    num_entry_checked++;
  }
  if(entry_found)
  {
    // Convert JSON to scheduler
    if(!JSON_to_scheduler(p_scheduler, json_buff, entry.json_len))
    {
      CONSOLE_LOG_ERROR("Fail to parse json to scheduler");
      return false;
    }
    p_scheduler->id = entry_idx;
  }
  return entry_found;
}

static int32_t sheduler_file_get_num_entry(char * const file_path)
{
  int32_t ret = 0;
  ASSERT_LOG_ERROR_RETURN_VAL((file_path), -1, "Invalid Param");
  int file_size;
  size_t file_idx = 0;
  int32_t lfs_err = 0;
  lfs_file_t fil = {0};
  uint32_t num_entry_checked = 0;
  scheduler_file_entry_t entry = {0};
  // Check file size first
  FILE_LOCK();
  
  lfs_err = lfs_file_open(p_lfs, &fil, file_path, LFS_O_RDONLY);
  if(lfs_err != LFS_ERR_OK)
  {
    CONSOLE_LOG_ERROR("File open fail:%s", lfs_port_get_err_string(lfs_err));
    return -1;
  }
  file_size = lfs_file_size(p_lfs, &fil);
  lfs_file_close(p_lfs, &fil);
  FILE_UNLOCK();
  if(file_size < (sizeof(scheduler_file_header_t)))
  {
    CONSOLE_LOG_ERROR("File size invalid");
    return -1;
  }
  // Start at the begin of first entry
  file_idx = sizeof(scheduler_file_header_t);
  while(file_idx < file_size)
  {
    if(!scheduler_file_read(file_path, file_idx, (uint8_t*)&entry, sizeof(scheduler_file_entry_t)))
    {
      CONSOLE_LOG_ERROR("File read fail");
      return -1;
    }
    file_idx += sizeof(scheduler_file_entry_t);
    if(entry.signature != ENTRY_SIGNATURE)
    {
      CONSOLE_LOG_ERROR("Invalid entry signature");
      return -1;
    }
    file_idx += entry.json_len; // move to next entry
    num_entry_checked++;
  }
  return num_entry_checked; 
}

static bool is_entry_equal(const scheduler_t * p_sch_1, const scheduler_t * p_sch_2)
{
  ASSERT_LOG_ERROR_RETURN_VAL((p_sch_1 && p_sch_2), false, "Invalid param"); // Shoule return this error, but too lazy for now
  if(!rtc_is_equal(&(p_sch_1->trig_time), &(p_sch_2->trig_time))) return false;
  if(memcmp(&(p_sch_1->act_param), &(p_sch_2->act_param), sizeof(scheduler_action_param_t)) != 0) return false;
  if(p_sch_1->action != p_sch_2->action) return false;
  if(p_sch_1->dow_mask != p_sch_2->dow_mask) return false;
  if(p_sch_1->month_mask != p_sch_2->month_mask) return false;
  return true;
}

