// ===================================================================================
// Project:   Example for ATtiny814
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
// Blink built-in LED using millis counter.
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
#include "millis.h"                 // millis counter functions

#define PIN_LED   PA7               // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CLK_init();                       // set system clock frequency
  MIL_init();                       // init and start millis counter
  PIN_output(PIN_LED);              // set LED pin to output
  uint32_t next = 100;              // next event in 100ms

  // Loop
  while(1) {
    while(MIL_read() < next);       // wait for next event
    PIN_toggle(PIN_LED);            // toggle LED
    next += 100;                    // next event in 100ms
  }
}
