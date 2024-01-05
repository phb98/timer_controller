#ifndef _DS3231_REGS_H_
#define _DS3231_REGS_H_
#define DS3231_ADDRESS                (0x68)   ///< I2C address for DS3231
#define DS3231_TIMEREG                (0x00)      ///< Time register
#define DS3231_ALARM1                 (0x07)    ///< Alarm 1 register
#define DS3231_ALARM2                 (0x0B)    ///< Alarm 2 register
#define DS3231_CONTROL                (0x0E)   ///< Control register
#define DS3231_STATUSREG              (0x0F) ///< Status register
#define DS3231_STATUSREG_OSC_STOP_BIT (1 << 7)
#define DS3231_TEMPERATUREREG 0x11 ///< Temperature register (high byte - low byte is at 0x12)
#endif