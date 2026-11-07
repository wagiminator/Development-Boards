// ===================================================================================
// Project:   Example for STM32C011
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
// Counts on a MAX7219 8-digit 7-segment display using hardware SPI. Connect DIN to
// PA7 (MOSI), CLK to PA5 (SCK) and LOAD/CS to PA4.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with stm32isp (via pip)
//   are installed. If necessary, a driver for the USB-to-serial converter must be
//   installed.
// - Connect the board via USB to your PC.
// - Put the MCU into boot mode (press and hold BOOT, press and release RESET, then
//   release BOOT).
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "max7219.h"                // MAX7219 functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DLY_ms(100);                      // wait a bit for MAX7219
  SEG_init();                       // init MAX7219 7-segment display (and SPI)

  // Loop
  while(1) {
    for(uint32_t i=0; i<100000000; i++) {  // count from 0 to 99999999
      SEG_printD(i);                // print number
      DLY_ms(10);                   // wait a bit
    }
  }
}
