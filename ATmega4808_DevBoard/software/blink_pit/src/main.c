// ===================================================================================
// Project:   Example for ATmega4808
// Version:   v1.1
// Year:      2020
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
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with megaupdi (via pip)
//   are installed. If necessary, a driver for the USB-to-serial converter used by
//   the SerialUPDI programmer must be installed.
// - Connect the SerialUPDI programmer to the UPDI header of the board.
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
  PIT_enable(16384);                // enable PIT, period = 16384 / 32768 = 0.5 s
  PIT_INT_enable();                 // enable PIT interrupt
  INT_enable();                     // enable interrupts
  SLEEP_set_PDOWN();                // set sleep mode to PDOWN

  // Loop
  while(1) {
    SLEEP_now();                    // go to sleep, wake up by PIT
    PIN_toggle(PIN_LED);            // toggle LED
  }
}
