// ===================================================================================
// Project:   Example for CH32V203
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
// Blink built-in LED using software resets.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with
//   chprog (via pip) are installed. In addition, Linux requires access rights to
//   the USB bootloader.
// - Press the BOOT0 button on the board and keep it pressed while connecting it via
//   USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions

#define PIN_LED   PB1               // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  PIN_output(PIN_LED);              // set LED pin as output
  DLY_ms(100);                      // wait a bit
  PIN_toggle(PIN_LED);              // toggle LED
  DLY_ms(100);                      // wait a bit
  RST_now();                        // conduct software reset
}
