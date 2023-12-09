// ===================================================================================
// Project:   Example for STC8H1K08
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
// Echoes data sent via UART. Uses interrupts for RX.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with PySerial is installed.
// - Connect the MCU board via USB to your PC.
// - Run 'make flash'.
// - Perform a power cycle (press and release DOWNLOAD button) when prompted.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "uart1_int.h"                  // UART1 functions with interrupt

// Prototypes for used interrupts
void UART1_interrupt(void);
void UART1_ISR(void) __interrupt(INT_NO_UART1) {
  UART1_interrupt();
}

// ===================================================================================
// Main Function
// ===================================================================================
void main() {
  // Setup
  UART1_init();                         // init UART1 (RX: P30, TX: P31, BAUD: 115200, 8N1)

  // Loop
  while (1) {
    UART1_write(UART1_read());          // echo any received character
  }
}
