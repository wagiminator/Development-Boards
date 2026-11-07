// ===================================================================================
// Project:   Example for ATtiny3216
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
// Blink built-in LED using sleep mode with the periodic interrupt timer (PIT) as
// wake-up source.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with tinyupdi (via pip)
//   are installed. If necessary, a driver for the CH340N USB-to-serial converter
//   must be installed.
// - Set the Serial Mode Switch on the board to "UPDI" (UPDI programming mode).
// - Connect the board via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions

#define PIN_LED   PA7               // define LED pin

// ===================================================================================
// Periodic Interrupt Timer (PIT) Interrupt Service Routine
// ===================================================================================
ISR(RTC_PIT_vect) {
  PIT_FLAG_clear();                 // clear PIT interrupt flag
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CLK_init();                       // set system clock frequency
  PIN_output(PIN_LED);              // set LED pin to output
  PIT_enable(4096);                 // enable PIT, period = 4096 / 32768 = 0.125 s
  PIT_INT_enable();                 // enable PIT interrupt
  INT_enable();                     // enable interrupts
  SLEEP_set_STDBY();                // set sleep mode to STDBY

  // Loop
  while(1) {
    SLEEP_now();                    // go to sleep, wake up by PIT
    PIN_toggle(PIN_LED);            // toggle LED
  }
}
