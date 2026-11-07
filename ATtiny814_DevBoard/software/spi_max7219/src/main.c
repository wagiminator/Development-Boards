// ===================================================================================
// Project:   Example for ATtiny814
// Version:   v1.1
// Year:      2021
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Prints numbers on a MAX7219 8-digit 7-segment display using hardware SPI.
// Connect DIN to PA1 (MOSI), CLK to PA3 (SCK) and LOAD/CS to PA4.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with tinyupdi (via pip)
//   are installed. If necessary, a driver for the CH340N USB-to-serial converter
//   must be installed.
// - Set the Serial Mode Switch on the board to "P" (UPDI programming mode).
// - Connect the board via USB to your PC.
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
  CLK_init();                       // set system clock frequency
  DLY_ms(500);                      // wait a bit for MAX7219
  SEG_init();                       // init MAX7219 7-segment display (and SPI)

  // Loop
  while(1) {
    for(uint32_t i=0; i<100000000; i++) {  // print numbers 0 - 99999999
      SEG_printD(i);                // print number
      DLY_ms(10);                   // wait a bit
    }
  }
}
