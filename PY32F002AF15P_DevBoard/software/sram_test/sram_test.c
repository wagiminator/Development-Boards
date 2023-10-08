// ===================================================================================
// Project:   Example for PY32F002A
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
// Find out the real size of chip's SRAM. Chip will hang if trying to write to invalid
// SRAM address, so press reset to restart. I bet the PY32F002A actually has 4KB SRAM.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with PySerial is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect your MCU board via USB to your PC.
// - Set the MCU to boot mode by holding down the BOOT key and then pressing and 
//   releasing the RESET key. Finally release the BOOT key.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                         // system functions
#include "debug.h"                          // serial debug functions

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  uint32_t addr;                            // SRAM address
  DEBUG_init();                             // init DEBUG (TX: PA2, BAUD: 115200, 8N1)
  DEBUG_newline();                          // print newline
  
  // Loop
  while(1) {
    for(uint8_t i=1; i<=32; i++) {          // i contains current KB in the test
      addr = SRAM_BASE + (1024 * i) - 1;    // calculate last address of current KB
      DEBUG_print("Testing address 0x");    // info
      DEBUG_printW(addr);                   // print SRAM address to be tested
      *(uint8_t*)addr = 0x00;               // write to SRAM address, hangs if invalid
      DEBUG_print(" - passed ");            // ok, it hasn't failed yet
      DEBUG_printD(i); DEBUG_println("KB"); // print SRAM size tested
      DLY_ms(1000);                         // wait a bit
    }
  }
}
