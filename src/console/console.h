#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#include "sys_config.h"

// Module override log level
#ifdef MODULE_CONSOLE_LOG_LEVEL
  #if MODULE_CONSOLE_LOG_LEVEL < CONFIG_CONSOLE_LOG_LEVEL
  #undef CONFIG_CONSOLE_LOG_LEVEL
  #define CONFIG_CONSOLE_LOG_LEVEL MODULE_CONSOLE_LOG_LEVEL
  #endif
#endif
// ANSI color escape codes
#define COLOR_RESET   "\x1B[0m\r\n"
#define COLOR_WHITE   "\x1B[0m"
#define COLOR_RED     "\x1B[31m"
#define COLOR_YELLOW  "\x1B[33m"
#define COLOR_GREEN   "\x1B[32m"
#define COLOR_BLUE    "\x1B[34m"

#define GET_FILE_NAME(path) (strrchr(path, '/') ? strrchr(path, '/') + 1 : path)
#define _FILE_NAME_ GET_FILE_NAME(__FILE__)

#define CONSOLE_LOG(level_char, color_str, fmt, ...) CONFIG_CONSOLE_PRINTF("%s[%-8d] %-20s  %-25s %-4d  [%c]  " fmt COLOR_RESET, color_str, console_get_time_ms(), _FILE_NAME_, __func__, __LINE__, level_char, ##__VA_ARGS__)

#if CONFIG_CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_ERROR
#define CONSOLE_LOG_ERROR(fmt, ...)    CONSOLE_LOG('E', COLOR_RED, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_ERROR(...)
#endif

#if CONFIG_CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_WARN
#define CONSOLE_LOG_WARN(fmt, ...)    CONSOLE_LOG('W', COLOR_YELLOW, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_WARN(...)
#endif

#if CONFIG_CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_INFO
#define CONSOLE_LOG_INFO(fmt, ...)    CONSOLE_LOG('I', COLOR_GREEN, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_INFO(...)
#endif

#if CONFIG_CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_DEBUG
#define CONSOLE_LOG_DEBUG(fmt, ...)    CONSOLE_LOG('D', COLOR_BLUE, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_DEBUG(...)
#endif

#if CONFIG_CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_VERBOSE
#define CONSOLE_LOG_VERBOSE(fmt, ...)    CONSOLE_LOG('E', COLOR_WHITE, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_VERBOSE(...)
#endif

uint32_t console_get_time_ms();
#endif