#ifndef _SYS_CONFIG_H_
#define _SYS_CONFIG_H_
#define DISPLAY_SSD1306_OLED (0)
#define DISPLAY_ST7567_LCD   (1)
#define CONFIG_DISPLAY_USE                  DISPLAY_ST7567_LCD
#define CONFIG_DISPLAY_I2C                  (i2c0)
#define CONFIG_DISPLAY_I2C_SPEED            (400*1000)
#define CONFIG_DISPLAY_I2C_SCL              (5)
#define CONFIG_DISPLAY_I2C_SDA              (4)

#if CONFIG_DISPLAY_USE == DISPLAY_ST7567_LCD
#define CONFIG_DISPLAY_I2C_ADDR             (0x3f)
#elif CONFIG_DISPLAY_USE == DISPLAY_SSD1306_OLED
#define CONFIG_DISPLAY_I2C_ADDR             (0x3c)
#else
#error Please choose correct display
#endif

#endif