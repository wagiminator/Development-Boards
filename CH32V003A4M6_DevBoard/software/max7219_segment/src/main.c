// ===================================================================================
// Project:   Example for CH32V003
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
// Connect PC4 (CS), PC5 (CLK), PC6 (DIN).
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with rvprog
//   are installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>                   // user configurations
#include <system.h>                   // system functions
#include <gpio.h>                     // GPIO functions
#include <max7219.h>                  // MAX7219 functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
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
