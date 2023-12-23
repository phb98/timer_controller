#include "hardware/i2c.h"
#include "console.h"
#include <stdint.h>
#include "display_u8g2_port.h"
#include "sys_config.h"
#include "FreeRTOS.h"
#include "task.h"
void display_u8g2_port_init()
{
  CONSOLE_LOG_INFO("Init u8g2 hardware");
}
static uint8_t display_send_bytes(uint8_t * p_bytes, const uint16_t byte_num);

uint8_t display_u8g2_port_gpio_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
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

uint8_t display_u8g2_port_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
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
  int byte_transfered = i2c_write_blocking(CONFIG_DISPLAY_I2C, CONFIG_DISPLAY_I2C_ADDR, p_bytes, byte_num, false);
  if(byte_transfered == byte_num)
  {
    return 0;
  }
  CONSOLE_LOG_WARN("display send bytes fail");
  return 1;
}