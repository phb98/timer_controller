#ifndef WATCHDOG_H
#define WATCHDOG_H
#include <stdint.h>
#include <stdbool.h>
// Function to initialize the watchdog
void watchdog_start(uint32_t timeout_ms);

// Function to kick the watchdog
void watchdog_kick();

// Function to disable the watchdog
void watchdog_stop();

// Function to check if the watchdog was reset
// Returns true if the watchdog was reset
bool is_watchdog_reset();

void watchdog_force_reboot();
#endif // WATCHDOG_H
