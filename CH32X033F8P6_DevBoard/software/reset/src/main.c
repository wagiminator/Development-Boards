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
// Blink built-in LED using software resets.
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
#include <config.h>         // user configurations
#include <system.h>         // system functions
#include <gpio.h>           // GPIO functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  PIN_output(PIN_LED);      // set LED pin to output
  DLY_ms(100);              // wait a bit
  PIN_toggle(PIN_LED);      // toggle LED on/off
  DLY_ms(100);              // wait a bit
  RST_now();                // reset
}
