// ===================================================================================
// Project:   Example for ATSAMD11C14A
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
// Sends a message via UART once a second.
// Connect pin PA8 (TX) to RX of a USB-to-serial converter (115200 BAUD, 8N1).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with tinyocd (via pip)
//   are installed. If necessary, grant access permissions for the SWD programmer.
// - Connect the SWD programmer (e.g. CMSIS-DAP) to the SWD pins of the board.
// - Run 'make flash'.
// - Alternatively, if the SAM-BA bootloader is installed, set LDSCRIPT and ISPTOOL
//   in the makefile accordingly (see readme) and upload the firmware via USB.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "debug_serial.h"           // serial debug functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DEBUG_init();                     // init DEBUG (TX: PA8, BAUD: 115200, 8N1)

  // Loop
  while(1) {
    DEBUG_println("Hello world!");  // send message via UART
    DLY_ms(1000);                   // wait a second
  }
}
