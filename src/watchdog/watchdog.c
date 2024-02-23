#include "watchdog.h"
#include "pico/stdlib.h"
#include "hardware/watchdog.h"
#include "console.h"
#define OSC_CLK_DIV (12)   // Assume crystal is 12 MHz, divide by 12 to get 1 MHz to the watchdog
// Public functions
void watchdog_start(uint32_t timeout_ms)
{
  watchdog_stop(); // Disable the watchdog before configuring it
  watchdog_enable(timeout_ms, true); // Pause on debug
  watchdog_start_tick(OSC_CLK_DIV);
  CONSOLE_LOG_INFO("Watchdog started with timeout of %d ms", timeout_ms);
}

void watchdog_kick(void)
{
  watchdog_update();
  CONSOLE_LOG_DEBUG("Watchdog kicked");
}

void watchdog_stop(void)
{
  // SDK doesn't have a watchdog_disable function
  // So we use register access to disable the watchdog
  hw_clear_bits(&watchdog_hw->ctrl, WATCHDOG_CTRL_ENABLE_BITS);
  CONSOLE_LOG_INFO("Watchdog stopped");
}

bool is_watchdog_reset()
{
  return watchdog_enable_caused_reboot();
}

void watchdog_force_reboot(void)
{
  CONSOLE_LOG_INFO("Forcing a watchdog reset");
  watchdog_enable(1, false);
  while (1)
  {
    // Wait for the watchdog to reset the system
  }
}