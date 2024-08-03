// ===================================================================================
// Project:   Sleep / Automatic Wake-Up Timer Demo for CH32V003
// Version:   v1.1
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Blink built-in LED using deep sleep with automatic wake-up timer (AWU).
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>           // user configurations
#include <system.h>           // system functions
#include <gpio.h>             // GPIO functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  PIN_output(PIN_LED);        // set LED pin as output
  AWU_start(100);             // start automatic wake-up timer, period 100ms

  // Loop
  while(1) {
    PIN_toggle(PIN_LED);      // toggle LED on/off
    STDBY_WFE_now();          // go to STANDBY (deep sleep), wake up by AWU
  }
}
