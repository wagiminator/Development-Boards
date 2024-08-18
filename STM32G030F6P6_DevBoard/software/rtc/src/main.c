// ===================================================================================
// Project:   Example for STM32G03x/04x
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Blink built-in LED and print RTC time register via serial interface every second.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with stm32isp is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect your MCU board via USB to your PC.
// - Set the MCU to boot mode by holding down the BOOT key and then pressing and 
//   releasing the RESET key. Finally release the BOOT key.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "debug_serial.h"           // serial debug functions

#define PIN_LED   PB3               // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  PIN_output(PIN_LED);              // set LED pin as output
  DEBUG_init();                     // init serial debug
  RTC_init();                       // init real-time clock
  RTC_setTime(0x15, 0x59, 0x30);    // set time to 15:59:30
  RTC_startWakeupTimer(1000);       // set wakeup timer to 1000ms period

  // Loop
  while(1) {
    while(!(RTC_readWakeupFlag())); // wait for end of wakeup timer period
    RTC_clearWakeupFlag();          // clear the flag
    PIN_toggle(PIN_LED);            // toggle LED on/off
    DEBUG_print("RTC time register: ");
    DEBUG_printW(RTC_readTime());   // print time register
    DEBUG_newline();
  }
}
