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
// Flashes the built-in LED at 1Hz and sends a message once a second via the
// USB-to-serial converter (115200 BAUD, 8N1).
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
#include "debug_serial.h"           // serial debug functions

#define PIN_LED   PA7               // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CLK_init();                       // set system clock frequency
  PIN_output(PIN_LED);              // set LED pin to output
  DEBUG_init();                     // init DEBUG (TX: PA0, BAUD: 115200, 8N1)

  // Loop
  while(1) {
    PIN_high(PIN_LED);              // LED on
    DEBUG_println("Hello World !"); // send message
    DLY_ms(500);                    // wait 500ms
    PIN_low(PIN_LED);               // LED off
    DLY_ms(500);                    // wait 500ms
  }
}
