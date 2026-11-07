// ===================================================================================
// Project:   Example for ATtiny814
// Version:   v1.1
// Year:      2021
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Blink built-in LED using watchdog resets.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with tinyupdi (via pip)
//   are installed. If necessary, a driver for the CH340N USB-to-serial converter
//   must be installed.
// - Set the Serial Mode Switch on the board to "P" (UPDI programming mode).
// - Connect the board via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions

#define PIN_LED   PA7               // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CLK_init();                       // set system clock frequency
  PIN_output(PIN_LED);              // set LED pin as output
  WDT_start(256);                   // start watchdog with 256ms period
  DLY_ms(128);                      // wait 128ms
  PIN_toggle(PIN_LED);              // toggle LED

  // Loop
  while(1);                         // wait for watchdog reset
}
