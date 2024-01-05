#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#define CONFIG_MAJOR_VER     (1)
#define CONFIG_MINOR_VER     (0)

// Macros
#define MS_TO_TICK_RTOS(n)                  (n/portTICK_RATE_MS)
// I2C config
#define CONFIG_I2C0_SCL                     (5)
#define CONFIG_I2C0_SDA                     (4)
#define CONFIG_I2C1_SCL                     (15)
#define CONFIG_I2C1_SDA                     (14)
// display config
#define DISPLAY_SSD1306_OLED                (0)
#define DISPLAY_ST7567_LCD                  (1)
#define CONFIG_DISPLAY_USE                  DISPLAY_ST7567_LCD
#define CONFIG_DISPLAY_I2C                  (HAL_I2C0)
#define CONFIG_DISPLAY_I2C_SPEED            (400*1000)
#define CONFIG_DISPLAY_ORIENTATION          (U8G2_R2) // U8G2_Rx, x= 0 or 2
#if CONFIG_DISPLAY_USE == DISPLAY_ST7567_LCD
#define CONFIG_DISPLAY_I2C_ADDR             (0x3f)
#elif CONFIG_DISPLAY_USE == DISPLAY_SSD1306_OLED
#define CONFIG_DISPLAY_I2C_ADDR             (0x3c)
#else
#error Please choose correct display
#endif
// Console config
#define CONSOLE_LOG_LEVEL_ERROR             (1)
#define CONSOLE_LOG_LEVEL_WARN              (2)
#define CONSOLE_LOG_LEVEL_INFO              (3)
#define CONSOLE_LOG_LEVEL_DEBUG             (4)
#define CONSOLE_LOG_LEVEL_VERBOSE           (5)
#define CONFIG_CONSOLE_LOG_LEVEL            CONSOLE_LOG_LEVEL_VERBOSE
#define CONFIG_CONSOLE_PRINTF(...)          printf(__VA_ARGS__)
// User input config
#define CONFIG_BUTTON_ENTER_PIN              (7)
#define CONFIG_BUTTON_ENTER_ACTIVE           (0)
#define CONFIG_BUTTON_UP_PIN                 (6)
#define CONFIG_BUTTON_UP_ACTIVE              (0)
#define CONFIG_BUTTON_DOWN_PIN               (8)
#define CONFIG_BUTTON_DOWN_ACTIVE            (0)
#define CONFIG_BUTTON_DEBOUNCE_MS            (20)
#define CONFIG_BUTTON_LONG_PRESS_MS          (3000)
// RTC config
#define CONFIG_RTC_CHIP_DS3231               (1)
#define CONFIG_RTC_CHIP_USING                (CONFIG_RTC_CHIP_DS3231)
#define CONFIG_RTC_I2C                       (HAL_I2C0)
#define CONFIG_RTC_I2C_SPEED                 (400*1000)
#endif