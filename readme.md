# Timer Controller

## Overview

The Timer Controller is a versatile device designed to manage and control multiple timers with precision. It features an LCD screen for display and three buttons for user interaction. The device is capable of storing and editing an extensive number of timers, allowing users to customize their timing requirements efficiently. It can output to relay or MOSFET for controlling external devices. Importantly, the Timer Controller ensures reliability by storing timers persistently, enabling recovery from power loss. This project utilizes the u8g2 library for the LCD screen, FreeRTOS for efficient task management, LittleFS as main filesystem, and the DS3231 Real-Time Clock (RTC) for precise timekeeping, all running on the RP2040 microcontroller unit (MCU).

The source code is written with cross-platform and flexible hardware in mind. You can change the RTC or change Digital output IC with very little change in source code
## Features

- LCD screen for clear display and user interaction.
- Three buttons for intuitive navigation and control.
- Storage and editing of a vast number of timers.
- Persistence of timers to recover from power loss.
- Utilizes the u8g2 library for LCD functionality.
- Incorporates FreeRTOS for efficient multitasking.
- Utilizes the DS3231 Real-Time Clock for precise timekeeping.
- Implemented on the RP2040 MCU for reliable performance.
- Timer config is stored in JSON format for flexible and future offline edit(from PC or phone)