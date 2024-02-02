#include "lfs.h"
#include "littlefs_port.h"
#include "sys_config.h"
#include <hardware/flash.h>
#include <hardware/sync.h>
#define MODULE_CONSOLE_LOG_LEVEL CONSOLE_LOG_LEVEL_DEBUG
#include "console.h"
#ifdef LFS_THREADSAFE
#define MUTEX_BLOCK_TIME_MS (MS_TO_TICK_RTOS(5000))
#include "FreeRTOS.h"
#include "semphr.h"
#endif
// The address we used in system is offset from XIP_BASE, but flash and programe api
// expect offset from start flash address, so we need to subtrace the offset
#define FLASH_PROG_ERASE_OFFSET   (XIP_BASE)
#define LFS_LOOKAHEAD_SIZE        (32)
// variables used by the filesystem
static uint8_t read_cache[FLASH_SECTOR_SIZE];
static uint8_t prog_cache[FLASH_SECTOR_SIZE];
static uint8_t look_ahead_cache[LFS_LOOKAHEAD_SIZE];
static lfs_t lfs;
#ifdef LFS_THREADSAFE
SemaphoreHandle_t lfs_mutex;
#endif
// Private function prototype 
static int lfs_port_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size);
static int lfs_port_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size);
static int lfs_port_erase(const struct lfs_config *c, lfs_block_t block);
static int lfs_port_sync(const struct lfs_config *c);
#ifdef LFS_THREADSAFE
static int lfs_port_lock(const struct lfs_config *c);
static int lfs_port_unlock(const struct lfs_config *c);
#endif
// configuration of the filesystem is provided by this struct
const struct lfs_config cfg = {
    // block device operations
    .read  = lfs_port_read,
    .prog  = lfs_port_prog,
    .erase = lfs_port_erase,
    .sync  = lfs_port_sync,
#ifdef LFS_THREADSAFE
    .lock   = lfs_port_lock,
    .unlock = lfs_port_unlock,
#endif 
    // block device configuration
    .read_size          = CONFIG_FLASH_MIN_BYTE_READ,
    .prog_size          = CONFIG_FLASH_PAGE_SIZE,
    .block_size         = FLASH_SECTOR_SIZE,
    .block_count        = CONFIG_FLASH_SECTOR_COUNT,
    .block_cycles       = CONFIG_FLASH_WRITE_CYCLE,
    .cache_size         = FLASH_SECTOR_SIZE,
    .lookahead_size     = LFS_LOOKAHEAD_SIZE,
    .read_buffer        = read_cache,
    .prog_buffer        = prog_cache,
    .lookahead_buffer   = look_ahead_cache,
};

// int test(void) {
//     lfs_file_t file;

//     // mount the filesystem
//     int err = lfs_mount(&lfs, &cfg);

//     // reformat if we can't mount the filesystem
//     // this should only happen on the first boot
//     if (err) {
//         lfs_format(&lfs, &cfg);
//         lfs_mount(&lfs, &cfg);
//     }

//     // read current count
//     uint32_t boot_count = 0;
//     lfs_file_open(&lfs, &file, "boot_count", LFS_O_RDWR | LFS_O_CREAT);
//     lfs_file_read(&lfs, &file, &boot_count, sizeof(boot_count));

//     // update boot count
//     boot_count += 1;
//     lfs_file_rewind(&lfs, &file);
//     lfs_file_write(&lfs, &file, &boot_count, sizeof(boot_count));

//     // remember the storage is not updated until the file is closed successfully
//     lfs_file_close(&lfs, &file);

//     // release any resources we were using
//     lfs_unmount(&lfs);

//     // print the boot count
//     printf("boot_count: %d\n", boot_count);
// }

void lfs_port_init()
{
  uint64_t flash_uuid;
  flash_get_unique_id((uint8_t*)&flash_uuid);
  CONSOLE_LOG_INFO("Flash UUID:0x%016llx", flash_uuid);
  CONSOLE_LOG_INFO("Init LittleFS");
  // Create mutex
  #ifdef LFS_THREADSAFE
  lfs_mutex = xSemaphoreCreateMutex();
  #endif
  // mount the filesystem
  int err = lfs_mount(&lfs, &cfg);

  // reformat if we can't mount the filesystem
  // this should only happen on the first boot
  if (err) {
    CONSOLE_LOG_WARN("LittleFS is not formated, format now");
    lfs_format(&lfs, &cfg);
    lfs_mount(&lfs, &cfg);
  }
  // Create system folder
  int lfs_ret = lfs_mkdir(&lfs, "/System");
  CONSOLE_LOG_INFO("Create System folder:%s", lfs_port_get_err_string(lfs_ret));
}

lfs_t * lfs_port_get_lfs_handle()
{
  return &lfs;
}

const char * lfs_port_get_err_string(const int err)
{
  static const char * err_str[] = {
    "LFS_ERR_OK",         
    "LFS_ERR_IO",         
    "LFS_ERR_CORRUPT",    
    "LFS_ERR_NOENT",      
    "LFS_ERR_EXIST",      
    "LFS_ERR_NOTDIR",     
    "LFS_ERR_ISDIR",      
    "LFS_ERR_NOTEMPTY",   
    "LFS_ERR_BADF",       
    "LFS_ERR_FBIG",       
    "LFS_ERR_INVAL",      
    "LFS_ERR_NOSPC",      
    "LFS_ERR_NOMEM",      
    "LFS_ERR_NOATTR",     
    "LFS_ERR_NAMETOOLONG",
  };
  static const char * unknow_err_str = "LFS_ERR_UNKNOWN";
  const int err_idx[] = {0,-5,-84,-2,-17,-20,-21,-39,-9,-27,-22,-28,-12,-61,-36};
  for(int i = 0; i < (sizeof(err_idx) / sizeof(err_idx[0])); i++)
  {
    if(err_idx[i] == err)
    {
      return err_str[i];
    }
  }
  return unknow_err_str;
}
// PORTING 

static int lfs_port_read(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, void *buffer, lfs_size_t size)
{
  if(!buffer || !c)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return -1;
  }
  if(size % (c->read_size) != 0)
  {
    CONSOLE_LOG_ERROR("Read not round byte:%d", size);
    return -1;
  }
  uint32_t read_addr = CONFIG_LFS_START_ADDR + block * (c->block_size) + off;
  memcpy(buffer, (uint8_t*)(read_addr), size);
  CONSOLE_LOG_VERBOSE("Read %d bytes from addr:0x%x", size, read_addr);
  return 0;
}
static int lfs_port_prog(const struct lfs_config *c, lfs_block_t block, lfs_off_t off, const void *buffer, lfs_size_t size)
{
  if(!buffer || !c)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return -1;
  }
  if(size % (c->prog_size) != 0)
  {
    CONSOLE_LOG_ERROR("Program not round byte:%ld", size);
    return -1;
  }
  uint32_t prog_addr = CONFIG_LFS_START_ADDR + (c->block_size) * block + off - FLASH_PROG_ERASE_OFFSET;
  uint32_t ints = save_and_disable_interrupts();
  flash_range_program(prog_addr, buffer, size);
  restore_interrupts(ints);
  CONSOLE_LOG_VERBOSE("Program %d bytes to addr:0x%x", size, prog_addr + FLASH_PROG_ERASE_OFFSET);
  return 0;
}
static int lfs_port_erase(const struct lfs_config *c, lfs_block_t block)
{
    volatile uint32_t a = CONFIG_LFS_START_ADDR;
  uint32_t erase_addr = CONFIG_LFS_START_ADDR + (c->block_size) * block - FLASH_PROG_ERASE_OFFSET;
  uint32_t ints = save_and_disable_interrupts();
  flash_range_erase(erase_addr, CONFIG_FLASH_SECTOR_SIZE);
  restore_interrupts(ints);
  CONSOLE_LOG_VERBOSE("Erase sector:0x%x ", erase_addr + FLASH_PROG_ERASE_OFFSET);
  return 0;
}
static int lfs_port_sync(const struct lfs_config *c)
{
  // Do nothing
  return 0;
}

#ifdef LFS_THREADSAFE
static int lfs_port_lock(const struct lfs_config *c)
{
  if(pdFALSE == xSemaphoreTake(lfs_mutex, MUTEX_BLOCK_TIME_MS)) return -1;
  return 0;
}
static int lfs_port_unlock(const struct lfs_config *c)
{
  xSemaphoreGive(lfs_mutex);
  return 0;
}
#endif