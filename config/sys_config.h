#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_

#define CONFIG_MAJOR_VER                    (1)
#define CONFIG_MINOR_VER                    (0)
#define CONFIG_HARDWARE_VER                 (0)
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
#define CONFIG_DISPLAY_USE                  DISPLAY_SSD1306_OLED
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
#define CONSOLE_LOG_LEVEL_ERROR               (1)
#define CONSOLE_LOG_LEVEL_WARN                (2)
#define CONSOLE_LOG_LEVEL_INFO                (3)
#define CONSOLE_LOG_LEVEL_DEBUG               (4)
#define CONSOLE_LOG_LEVEL_VERBOSE             (5)
#define CONFIG_CONSOLE_LOG_LEVEL              CONSOLE_LOG_LEVEL_VERBOSE
#define CONFIG_CONSOLE_PRINTF(...)            printf(__VA_ARGS__)
// User input config
#define CONFIG_BUTTON_ENTER_PIN              (7)
#define CONFIG_BUTTON_ENTER_ACTIVE           (0)
#define CONFIG_BUTTON_UP_PIN                 (6)
#define CONFIG_BUTTON_UP_ACTIVE              (0)
#define CONFIG_BUTTON_DOWN_PIN               (8)
#define CONFIG_BUTTON_DOWN_ACTIVE            (0)
#define CONFIG_BUTTON_DEBOUNCE_MS            (20)
#define CONFIG_BUTTON_LONG_PRESS_MS          (1500)
// RTC config
#define CONFIG_RTC_CHIP_DS3231               (1)
#define CONFIG_RTC_CHIP_USING                (CONFIG_RTC_CHIP_DS3231)
#define CONFIG_RTC_I2C                       (HAL_I2C0)
#define CONFIG_RTC_I2C_SPEED                 (400*1000)
#define CONFIG_RTC_INTERRUPT_PIN             (3)
#define CONFIG_RTC_MAX_CALLBACK_NUM          (8)
// Flash
#define CONFIG_FLASH_SIZE                    (8*1024*1024) // 8MB
#define CONFIG_PROGRAM_SIZE                  (2*1024*1024) // Default Pico board use 2MB flash
#define CONFIG_LFS_SIZE                      (4*1024*1024) // 4MB
#define CONFIG_PROGRAM_START_ADDR            (XIP_BASE)
#define CONFIG_PROGRAM_END_ADDR              (CONFIG_PROGRAM_START_ADDR + CONFIG_PROGRAM_SIZE - 1)
#define CONFIG_LFS_START_ADDR                (CONFIG_PROGRAM_END_ADDR + 1)
#define CONFIG_LFS_END_ADDR                  (CONFIG_LFS_START_ADDR + CONFIG_LFS_SIZE - 1)
#if ((CONFIG_PROGRAM_SIZE + CONFIG_LFS_SIZE) > CONFIG_FLASH_SIZE)
#error Invalid config flash size
#endif

#define CONFIG_FLASH_WRITE_CYCLE             (1000)
#define CONFIG_FLASH_MIN_BYTE_READ           (1) // the beauty of XIP
#define CONFIG_FLASH_SECTOR_SIZE             (FLASH_SECTOR_SIZE) // Min erasing size. 4KB for rp2040
#define CONFIG_FLASH_PAGE_SIZE               (FLASH_PAGE_SIZE)   // Min program size, 256B for rp2040
#define CONFIG_FLASH_SECTOR_COUNT            (CONFIG_LFS_SIZE / CONFIG_FLASH_SECTOR_SIZE)
// Output
#define CONFIG_OUTPUT_CHIP_PCF8574           (0)
#define CONFIG_OUTPUT_CHIP_MCU_PWM           (1)

#define CONFIG_PCF8574_I2C                   (HAL_I2C0)
#define CONFIG_PCF8574_I2C_ADDR              (0X20)
#define CONFIG_PCF8574_I2C_SPEED             (100*1000)

#define CONFIG_MCU_PWM_NUM_CH                (4)
#define CONFIG_MCU_PWM_CH0_PIN               (29)
#define CONFIG_MCU_PWM_CH1_PIN               (28)
#define CONFIG_MCU_PWM_CH2_PIN               (25)
#define CONFIG_MCU_PWM_CH3_PIN               (24)
#define CONFIG_MCU_PWM_FREQ                  (2*1000)

#define CONFIG_OUTPUT_NUM_BLOCK              (2) // 1 is Binary relay, 1 is PWM output
#define CONFIG_OUTPUT_BLOCK_1_TYPE           (OUTPUT_TYPE_BINARY) // Check output_controller_type_t
#define CONFIG_OUTPUT_BLOCK_1_CHIP           (CONFIG_OUTPUT_CHIP_PCF8574)
#define CONFIG_OUTPUT_BLOCK_1_START_CH       (0)
#define CONFIG_OUTPUT_BLOCK_1_NUM_CH         (8)
#define CONFIG_OUTPUT_BLOCK_1_END_CH         (CONFIG_OUTPUT_BLOCK_1_START_CH + CONFIG_OUTPUT_BLOCK_1_NUM_CH - 1)
#define CONFIG_OUTPUT_BLOCK_2_TYPE           (OUTPUT_TYPE_PWM)
#define CONFIG_OUTPUT_BLOCK_2_CHIP           (CONFIG_OUTPUT_CHIP_MCU_PWM)
#define CONFIG_OUTPUT_BLOCK_2_START_CH       (CONFIG_OUTPUT_BLOCK_1_END_CH + 1)
#define CONFIG_OUTPUT_BLOCK_2_NUM_CH         (4)
#define CONFIG_OUTPUT_BLOCK_2_END_CH         (CONFIG_OUTPUT_BLOCK_2_START_CH + CONFIG_OUTPUT_BLOCK_2_NUM_CH - 1)
#endif