#define  MODULE_CONSOLE_LOG_LEVEL CONSOLE_LOG_LEVEL_DEBUG
#include "hardware/i2c.h"
#include "pico/stdlib.h"
#include <stdint.h>
#include "display_u8g2_port.h"
#include "sys_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "console.h"
#include "display.h"
#include "hal_i2c.h"
static u8g2_t u8g2_disp;
static hal_i2c_handle_t i2c_handle;
static uint8_t display_u8g2_port_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t display_u8g2_port_gpio_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
static uint8_t display_send_bytes(uint8_t * p_bytes, const uint16_t byte_num);

void display_u8g2_port_init()
{
  // Init i2c peripheral
  CONSOLE_LOG_DEBUG("Init I2C Display");
  hal_i2c_get_handle(&i2c_handle, CONFIG_DISPLAY_I2C, CONFIG_DISPLAY_I2C_SPEED);
  // Check if display is attached
  uint8_t temp_buf;
  if(display_send_bytes(&temp_buf, 1) == 0)
  {
    CONSOLE_LOG_INFO("Found I2C Display 0x%x", CONFIG_DISPLAY_I2C_ADDR);
  }
  else 
  {
    CONSOLE_LOG_ERROR("Can't find I2C Display 0x%x %d", CONFIG_DISPLAY_I2C_ADDR);
    return;
  }
  // Init display controller
  #if CONFIG_DISPLAY_USE == DISPLAY_ST7567_LCD
  u8g2_Setup_st7567_i2c_enh_dg128064i_f(&u8g2_disp, CONFIG_DISPLAY_ORIENTATION, display_u8g2_port_i2c, display_u8g2_port_gpio_delay);
  #elif CONFIG_DISPLAY_USE == DISPLAY_SSD1306_OLED
  u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g2_disp, U8G2_R0, display_u8g2_port_i2c, display_u8g2_port_gpio_delay);
  #endif
  u8g2_InitDisplay(&u8g2_disp);
  u8g2_SetPowerSave(&u8g2_disp, 0);
  // u8g2_SetContrast(&u8g2_disp, 1);
  u8g2_ClearBuffer(&u8g2_disp);
  // Uncomment below code block to test display
  #if 0
  Draw u8g2 logo
  Uncomment those block to test screen
  u8g2_SetDrawColor(&u8g2_disp, 1);  // use this before call to DrawStr
  u8g2_SetFontMode(&u8g2_disp, 1);	// Transparent
  u8g2_SetFontDirection(&u8g2_disp, 0);
  u8g2_SetFont(&u8g2_disp,u8g2_font_inb24_mf);
  u8g2_DrawStr(&u8g2_disp, 0, 30, "U");
  u8g2_SetFontDirection(&u8g2_disp,1);
  u8g2_SetFont(&u8g2_disp, u8g2_font_inb30_mn);
  u8g2_DrawStr(&u8g2_disp, 21,8,"8");
  u8g2_SetFontDirection(&u8g2_disp, 0);
  u8g2_SetFont(&u8g2_disp, u8g2_font_inb24_mf);
  u8g2_DrawStr(&u8g2_disp, 51,30,"g");
  u8g2_DrawStr(&u8g2_disp, 67,30,"\xb2");
  u8g2_DrawHLine(&u8g2_disp, 2, 35, 47);
  u8g2_DrawHLine(&u8g2_disp, 3, 36, 47);
  u8g2_DrawVLine(&u8g2_disp, 45, 32, 12);
  u8g2_DrawVLine(&u8g2_disp, 46, 33, 12);
  #endif
  u8g2_SendBuffer(&u8g2_disp);
  CONSOLE_LOG_DEBUG("I2C Display init done");

}

u8g2_t * display_get_u8g2_handle()
{
  return &u8g2_disp;
}

// PRIVATE FUNCTIONS
static uint8_t display_u8g2_port_gpio_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	switch(msg)
	{
    case U8X8_MSG_GPIO_AND_DELAY_INIT:
      /* Insert codes for initialization */
      break;
    case U8X8_MSG_DELAY_MILLI:
      /* ms Delay */
      vTaskDelay(arg_int/portTICK_RATE_MS);
      break;
	}
	return 1;
}

static uint8_t display_u8g2_port_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
	/* u8g2/u8x8 will never send more than 128 bytes between START_TRANSFER and END_TRANSFER */
	static uint8_t buffer[256];
	static uint16_t buf_idx;
	uint8_t *data;

	switch(msg)
	{
	case U8X8_MSG_BYTE_SEND:
		data = (uint8_t *)arg_ptr;
		while( arg_int > 0 )
		{
			buffer[buf_idx++] = *data;
			data++;
			arg_int--;
      if(buf_idx >= sizeof(buffer))
      {
        CONSOLE_LOG_ERROR("Buffer overflow");
        buf_idx = 0;
      }
		}
		break;
	case U8X8_MSG_BYTE_INIT:
		break;
	case U8X8_MSG_BYTE_SET_DC:
		break;
	case U8X8_MSG_BYTE_START_TRANSFER:
		buf_idx = 0;
		break;
	case U8X8_MSG_BYTE_END_TRANSFER:
    display_send_bytes(buffer, buf_idx);
		break;
	default:
		return 0;
	}
	return 1;
}

static uint8_t display_send_bytes(uint8_t * p_bytes, const uint16_t byte_num)
{
  if(!p_bytes || !byte_num)
  {
    CONSOLE_LOG_ERROR("Invalid param");
    return 1;
  }
  CONSOLE_LOG_VERBOSE("Display send %d bytes", byte_num);
  uint32_t byte_transfered = hal_i2c_send_blocking(&i2c_handle, CONFIG_DISPLAY_I2C_ADDR, p_bytes, byte_num);

  // int byte_transfered = i2c_write_blocking(CONFIG_DISPLAY_I2C, CONFIG_DISPLAY_I2C_ADDR, p_bytes, byte_num, false);
  if(byte_transfered == byte_num)
  {
    return 0;
  }
  CONSOLE_LOG_WARN("display send bytes fail");
  return 1;
}