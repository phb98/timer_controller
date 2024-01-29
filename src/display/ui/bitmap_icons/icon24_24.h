#ifndef _ICON_24_24_H_
#define _ICON_24_24_H_


#define ICON_DEFINE(icon_name, ...) static const char icon_name##_24[] = {__VA_ARGS__}

ICON_DEFINE(icClock, 
  0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x7E, 0x00, 0x80, 0xFF, 0x01, 
  0xE0, 0x00, 0x07, 0x30, 0x00, 0x0C, 0x18, 0x00, 0x18, 0x08, 0x00, 0x10, 
  0x0C, 0x80, 0x30, 0x04, 0x40, 0x20, 0x06, 0x60, 0x60, 0x06, 0x30, 0x60, 
  0x06, 0x38, 0x60, 0x06, 0x18, 0x60, 0x06, 0x00, 0x60, 0x06, 0x00, 0x60, 
  0x04, 0x00, 0x20, 0x0C, 0x00, 0x30, 0x08, 0x00, 0x10, 0x18, 0x00, 0x18, 
  0x30, 0x00, 0x0C, 0xE0, 0x00, 0x07, 0x80, 0xE7, 0x01, 0x00, 0x7E, 0x00, 
  );
ICON_DEFINE(icAlarm, 
  0x00, 0x18, 0x00, 0x7C, 0x7E, 0x3E, 0xFE, 0xFF, 0x7F, 0xE6, 0x81, 0x67, 
  0x76, 0x00, 0x6E, 0x3E, 0x18, 0x7C, 0x1E, 0x18, 0x78, 0x0C, 0x18, 0x30, 
  0x0C, 0x18, 0x30, 0x06, 0x18, 0x60, 0x06, 0x18, 0x60, 0x06, 0x18, 0x60, 
  0x06, 0x38, 0x60, 0x06, 0xF0, 0x60, 0x06, 0xC0, 0x61, 0x0C, 0x80, 0x31, 
  0x0C, 0x00, 0x30, 0x18, 0x00, 0x18, 0x38, 0x00, 0x1C, 0x78, 0x00, 0x1E, 
  0xFC, 0x81, 0x3F, 0xCC, 0xFF, 0x31, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 
  );
ICON_DEFINE(icSetting,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x42, 0x00, 
  0x00, 0xC3, 0x00, 0xF0, 0xC3, 0x0F, 0xD8, 0x00, 0x1B, 0x0C, 0x00, 0x30, 
  0x0C, 0x7E, 0x30, 0x0C, 0xC3, 0x30, 0x18, 0x81, 0x18, 0x10, 0x81, 0x08, 
  0x10, 0x81, 0x08, 0x18, 0x81, 0x18, 0x0C, 0xC3, 0x30, 0x0C, 0x7E, 0x30, 
  0x0C, 0x00, 0x30, 0xD8, 0x00, 0x1B, 0xF0, 0xC3, 0x0F, 0x00, 0xC3, 0x00, 
  0x00, 0x42, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x00, 0x00, 
  );
ICON_DEFINE(icList, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x07, 0x00, 0x00, 0xCF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0xCF, 0xFF, 0xFF, 
  0xCF, 0xFF, 0xFF, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x07, 0x00, 0x00, 0xCF, 0xFF, 0xFF, 0x07, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  );
ICON_DEFINE(icManual, 
  0x0C, 0x18, 0x00, 0x1E, 0x3C, 0x00, 0x33, 0x66, 0x00, 0x33, 0x66, 0x00, 
  0x1E, 0x3C, 0x00, 0x0C, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x18, 0x00, 
  0x0C, 0x18, 0x00, 0x0C, 0x18, 0x00, 0x0C, 0x18, 0x00, 0xFC, 0xFF, 0x0F, 
  0xFC, 0xFF, 0x1F, 0x0C, 0x18, 0x30, 0x0C, 0x18, 0x30, 0x0C, 0x18, 0x30, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 0x18, 0x30, 0x1E, 0x3C, 0x78, 
  0x33, 0x66, 0xCC, 0x33, 0x66, 0xCC, 0x1E, 0x3C, 0x78, 0x0C, 0x18, 0x30, 
  );
ICON_DEFINE(icHome, 
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x18, 0x00, 0x00, 0x66, 0x00, 0x00, 0xC3, 0x00, 0x80, 0x81, 0x01, 
  0xC0, 0x00, 0x03, 0x20, 0x00, 0x04, 0x30, 0x00, 0x0C, 0x30, 0x00, 0x0C, 
  0x30, 0x3C, 0x0C, 0x30, 0x7E, 0x0C, 0x30, 0xC3, 0x0C, 0x30, 0xC3, 0x0C, 
  0x30, 0xC3, 0x0C, 0x30, 0xC3, 0x0C, 0x30, 0xC3, 0x0C, 0xE0, 0xFF, 0x07, 
  0xC0, 0xFF, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  );
#endif