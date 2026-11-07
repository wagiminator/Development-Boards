// ===================================================================================
// Project:   Example for STC8H8K64U
// Version:   v1.0
// Year:      2026
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
// - Make sure SDCC toolchain and Python3 with stc8usb (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press and hold the BOOT button while connecting the board via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "gpio.h"                   // GPIO functions

#define PIN_LED   P34               // define LED pin (active low)

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  P_SW2 |= 0x80;                    // enable access to extended registers (MUST!)
  PIN_output(PIN_LED);              // set LED pin as output
  WDT_start(256);                   // start watchdog with 256ms period
  DLY_ms(128);                      // wait 128ms
  PIN_toggle(PIN_LED);              // toggle LED

  // Loop
  while(1);                         // wait for watchdog reset
}
