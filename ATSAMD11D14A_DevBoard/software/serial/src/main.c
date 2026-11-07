// ===================================================================================
// Project:   Example for ATSAMD11D14A
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
// Echoes data sent via UART (115200 BAUD, 8N1).
// Connect pins PA8 (TX) and PA9 (RX) to a USB-to-serial converter.
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
#include "uart.h"                   // UART functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  UART_init();                      // init UART (TX: PA8, RX: PA9, BAUD: 115200, 8N1)

  // Loop
  while(1) {
    UART_write(UART_read());        // echo any received character
  }
}
