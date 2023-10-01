// ===================================================================================
// Project:   Example for PY32F0xx
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
// Sends a message via UART once a second.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with PySerial is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect the development board via USB to your PC.
// - Set the MCU to boot mode by holding down the BOOT key and then pressing and 
//   releasing the RESET key.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"
#include "debug.h"

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  DEBUG_init();               // init DEBUG (TX: PA2, BAUD: 115200, 8N1)
  
  // Loop
  while(1) {
    DEBUG_println("Hello World!");
    DLY_ms(1000);
  }
}