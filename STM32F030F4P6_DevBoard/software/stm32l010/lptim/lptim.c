// ===================================================================================
// Project:   Example for STM32L010
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
// Blink built-in LED using low-power timer.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with PySerial is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect your MCU board via USB to your PC.
// - Set the MCU to boot mode by holding down the BOOT key and then pressing and 
//   releasing the RESET key. Finally release the BOOT key.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"               // system functions
#include "gpio.h"                 // GPIO functions

#define PIN_LED   PB1             // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  PIN_output(PIN_LED);              // set LED pin to OUTPUT
  LPT_init();                       // init low-power timer
  LPT_start(100);                   // start timer in continuous mode with 100ms period

  // Loop
  while(1) {
    while(!(LPT_readReloadFlag())); // wait for end of period
    LPT_clearReloadFlag();          // clear reload flag
    PIN_toggle(PIN_LED);            // toggle LED
  }
}
