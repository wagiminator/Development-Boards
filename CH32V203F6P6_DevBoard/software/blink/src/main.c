// ===================================================================================
// Project:   Example for CH32V203
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
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with
//   chprog and rvprog (via pip) are installed. In addition, Linux requires access
//   rights to the USB bootloader.
// - Press the BOOT0 button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>                               // system functions
#include <gpio.h>                                 // GPIO functions

#define PIN_LED PB1                               // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  PIN_output(PIN_LED);                            // set LED pin to output
  
  // Loop
  while(1) {
    PIN_toggle(PIN_LED);                          // toggle LED
    DLY_ms(100);                                  // wait a bit
  }
}
