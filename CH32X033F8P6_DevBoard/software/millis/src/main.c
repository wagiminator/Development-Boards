// ===================================================================================
// Project:   Example for CH32X035/X034/X033
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
// Blink built-in LED using millis counter.
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with
//   chprog (via pip) are installed. In addition, Linux requires access rights to
//   the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>                 // user configurations
#include <system.h>                 // system functions
#include <gpio.h>                   // GPIO functions
#include <millis.h>                 // millis functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  uint32_t nexttime = 0;            // next time variable
  PIN_output(PIN_LED);              // set LED pin as output
  MIL_init();                       // init millis counter
  
  // Loop
  while(1) {
    PIN_toggle(PIN_LED);            // toggle LED on/off
    while(MIL_read() < nexttime);   // wait for next time
    nexttime += 100;                // increase next timeby 100 milliseconds
  }
}
