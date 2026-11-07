// ===================================================================================
// Project:   Example for ATmega4808
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
// Connect DIN to PA4 (MOSI), CLK to PA6 (SCK) and LOAD/CS to PD1.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with megaupdi (via pip)
//   are installed. If necessary, a driver for the USB-to-serial converter used by
//   the SerialUPDI programmer must be installed.
// - Connect the SerialUPDI programmer to the UPDI header of the board.
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
