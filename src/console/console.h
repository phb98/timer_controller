#ifndef _CONSOLE_H_
#define _CONSOLE_H_
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#define CONSOLE_LOG_LEVEL_ERROR   (1)
#define CONSOLE_LOG_LEVEL_WARN    (2)
#define CONSOLE_LOG_LEVEL_INFO    (3)
#define CONSOLE_LOG_LEVEL_DEBUG   (4)
#define CONSOLE_LOG_LEVEL_VERBOSE (5)

#define CONSOLE_LOG_LEVEL         CONSOLE_LOG_LEVEL_VERBOSE

// ANSI color escape codes
#define COLOR_RESET   "\x1B[0m\r\n"
#define COLOR_WHITE   "\x1B[0m"
#define COLOR_RED     "\x1B[31m"
#define COLOR_YELLOW  "\x1B[33m"
#define COLOR_GREEN   "\x1B[32m"
#define COLOR_BLUE    "\x1B[34m"

#define GET_FILE_NAME(path) (strrchr(path, '/') ? strrchr(path, '/') + 1 : path)
#define _FILE_NAME_ GET_FILE_NAME(__FILE__)

#define CONSOLE_LOG(level_char, color_str, fmt, ...) printf("%s[%-10d] %-10s  %-20s  %-4d  [%c]  " fmt COLOR_RESET, color_str, console_get_time_ms(), _FILE_NAME_, __func__, __LINE__, level_char, ##__VA_ARGS__)

#if CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_ERROR
#define CONSOLE_LOG_ERROR(fmt, ...)    CONSOLE_LOG('E', COLOR_RED, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_ERROR(...)
#endif

#if CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_WARN
#define CONSOLE_LOG_WARN(fmt, ...)    CONSOLE_LOG('W', COLOR_YELLOW, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_WARN(...)
#endif

#if CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_INFO
#define CONSOLE_LOG_INFO(fmt, ...)    CONSOLE_LOG('I', COLOR_GREEN, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_INFO(...)
#endif

#if CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_DEBUG
#define CONSOLE_LOG_DEBUG(fmt, ...)    CONSOLE_LOG('D', COLOR_BLUE, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_DEBUG(...)
#endif

#if CONSOLE_LOG_LEVEL >= CONSOLE_LOG_LEVEL_VERBOSE
#define CONSOLE_LOG_VERBOSE(fmt, ...)    CONSOLE_LOG('E', COLOR_WHITE, fmt, ##__VA_ARGS__)
#else
#define CONSOLE_LOG_VERBOSE(...)
#endif

uint32_t console_get_time_ms();
#endif