// ===================================================================================
// Project:   Example for STC8H8K64U
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
// P1.3 (MOSI), CLK to P1.5 (SCK) and LOAD/CS to P1.1.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with stc8usb (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press and hold the BOOT button while connecting the board via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "max7219.h"                // MAX7219 functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  uint32_t i;
  P_SW2 |= 0x80;                    // enable access to extended registers (MUST!)
  DLY_ms(100);                      // wait a bit for MAX7219
  SEG_init();                       // init MAX7219 7-segment display (and SPI)

  // Loop
  while(1) {
    for(i=0; i<100000000; i++) {    // count from 0 to 99999999
      SEG_printD(i);                // print number
      DLY_ms(10);                   // wait a bit
    }
  }
}
