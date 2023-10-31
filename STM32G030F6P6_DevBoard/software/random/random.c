// ===================================================================================
// Project:   Example for STM32G03x/04x
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
// Check the true random number generator, which should not exist on the STM32G030.
// Sends a 32-bit random number via UART once a second.
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
#include "system.h"                 // system functions
#include "debug_serial.h"           // serial debug functions

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  DEBUG_init();                     // init DEBUG (TX: PA2, BAUD: 115200, 8N1)
  RNG_init();                       // init true random number generator
  
  // Loop
  while(1) {
    DEBUG_printW(RNG_read());       // send 32-bit random number as hex value
    DEBUG_newline();                // new line
    DLY_ms(1000);                   // wait a second
  }
}
