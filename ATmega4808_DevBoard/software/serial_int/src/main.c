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
// Echoes data sent via UART (115200 BAUD, 8N1). Uses interrupts and a ring
// buffer for RX.
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
#include "uart_int.h"               // UART with RX interrupt functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CLK_init();                       // set system clock frequency
  UART_init();                      // init UART (TX: PA0, RX: PA1, BAUD: 115200, 8N1)

  // Loop
  while(1) {
    UART_write(UART_read());        // echo any received character
  }
}
