#ifndef _DISPLAY_U8G2_PORT_H_
#define _DISPLAY_U8G2_PORT_H_
#include "u8g2.h"
void    display_u8g2_port_init();
uint8_t display_u8g2_port_i2c(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
uint8_t display_u8g2_port_gpio_delay(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr);
#endif