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
// Blink example.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with PySerial is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect the USB-to-serial converter to the MCU board.
// - Set the MCU to boot mode by holding down the BOOT key and then pressing and 
//   releasing the RESET key.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"
#include "gpio.h"

#define PIN_LED   PB0

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  PIN_output(PIN_LED);

  // Loop
  while(1) {
    PIN_toggle(PIN_LED);
    DLY_ms(100);
  }
}
