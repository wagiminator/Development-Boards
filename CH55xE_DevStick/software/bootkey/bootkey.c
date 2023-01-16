// ===================================================================================
// Project:   Bootloader Demo for CH551, CH552, CH554
// Version:   v1.0
// Year:      2022
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Jump to bootloader on ACT-button press. Built-in LED lights up when in boot mode.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: doesn't matter
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>             // user configurations
#include <ch554.h>              // CH55x header file
#include <clock.h>              // system clock functions
#include <gpio.h>               // GPIO functions
#include <boot.h>               // for bootloader functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  CLK_config();                 // configure system clock
  PIN_high(PIN_LED);            // turn off built-in LED
  PIN_input_PU(PIN_ACTKEY);     // set ACT key pin to INPUT PULLUP

  while(1) {
    if(!PIN_read(PIN_ACTKEY)) { // ACT key pressed?
      PIN_low(PIN_LED);         // turn on built-in LED
      BOOT_now();               // jump to bootloader
    }
  }
}
