// ===================================================================================
// Project:   RST Pin as GPIO Demo for CH32V003
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
// Control built-in LED with RST button.
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
#include <system.h>         // system functions
#include <gpio.h>           // GPIO functions
#include <flash.h>          // flash functions

#define PIN_LED   PC0       // define LED pin
#define PIN_RST   PD7       // define RST pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  FLASH_OB_RESET2GPIO();    // make RST pin a normal GPIO pin
  PIN_input_PU(PIN_RST);    // set RST pin as input with pullup
  PIN_output(PIN_LED);      // set LED pin as output

  // Loop
  while(1) {
    PIN_write(PIN_LED, PIN_read(PIN_RST));  // set built-in LED according to RST button
  }
}
