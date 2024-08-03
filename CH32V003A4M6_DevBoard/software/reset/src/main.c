// ===================================================================================
// Project:   Software Reset Demo for CH32V003
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
// Blink built-in LED using continuous software resets.
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
#include <config.h>         // user configurations
#include <system.h>         // system functions
#include <gpio.h>           // GPIO functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  PIN_output(PIN_LED);      // set LED pin as output
  DLY_ms(100);              // wait 100ms
  PIN_toggle(PIN_LED);      // toggle LED on/off
  DLY_ms(100);              // wait 100ms
  RST_now();                // reset
}
