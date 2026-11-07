// ===================================================================================
// Project:   Example for ATmega4808
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
// Blink built-in LED using software resets.
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
// Main Function
// ===================================================================================
int main(void) {
  CLK_init();                       // set system clock frequency
  PIN_output(PIN_LED);              // set LED pin as output
  DLY_ms(100);                      // wait a bit
  PIN_toggle(PIN_LED);              // toggle LED
  DLY_ms(100);                      // wait a bit
  RST_now();                        // conduct software reset
}
