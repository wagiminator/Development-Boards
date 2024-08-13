// ===================================================================================
// Project:   Example for PY32F0xx
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
// Print numbers on a MAX7219 8-digit 8-segment display.
// Connect PA4 (CS), PA1 (CLK), PA7 (DIN).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with puyaisp is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect the USB-to-serial converter to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"
#include "gpio.h"
#include "max7219.h"

#define PIN_LED   PB0

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  uint32_t counter = 0;               // counter variable
  PIN_output(PIN_LED);                // set LED pin to output
  DLY_ms(500);                        // wait a bit for MAX7219
  SEG_init();                         // init MAX7219 8-segment display
  
  // Loop
  while(1) {
    SEG_print(counter++, 1);          // show counter on display and increment
    PIN_toggle(PIN_LED);              // toggle LED
    DLY_ms(100);                      // wait 100ms
  }
}
