#include "display.h"
#include "console.h"
#include "u8g2.h"
#include "display_u8g2_port.h"
#include "FreeRTOS.h"
#include "task.h"
static u8g2_t u8g2_disp;
static void display_thread_entry(void* unused_arg);
void display_init()
{
  CONSOLE_LOG_INFO("Init display");
  display_u8g2_port_init();
  xTaskCreate(display_thread_entry, 
              "DISPLAY_THREAD", 
              1024, 
              NULL, 
              1, 
              NULL);
}

static void display_thread_entry(void* unused_arg)
{
  CONSOLE_LOG_INFO("Display Thread entry");
  u8g2_Setup_st7567_i2c_enh_dg128064i_f(&u8g2_disp, U8G2_R0, display_u8g2_port_i2c, display_u8g2_port_gpio_delay);
  u8g2_InitDisplay(&u8g2_disp);
  u8g2_SetPowerSave(&u8g2_disp, 0);
  u8g2_ClearBuffer(&u8g2_disp);
    u8g2_SetFontMode(&u8g2_disp, 1);	// Transparent
    u8g2_SetDrawColor(&u8g2_disp, 1);  // use this before call to DrawStr
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
    u8g2_SendBuffer(&u8g2_disp);
    u8g2_UpdateDisplay(&u8g2_disp);
  while (1)
  {
    vTaskDelay(1000);
    /* code */
  }
  
}