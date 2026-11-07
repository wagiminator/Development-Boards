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
// Echoes data sent via UART1 (115200 BAUD, 8N1). Connect pins P3.7 (TX) and
// P3.6 (RX) to a USB-to-serial converter. Uses interrupts and a ring buffer
// for RX.
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
#include "uart1_int.h"              // UART1 with RX interrupt functions

// Prototypes for used interrupts
void UART1_ISR(void) __interrupt(INT_NO_UART1) { UART1_interrupt(); }

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  P_SW2 |= 0x80;                    // enable access to extended registers (MUST!)
  UART1_init();                     // init UART1 (TX: P37, RX: P36, BAUD: 115200)

  // Loop
  while(1) {
    UART1_write(UART1_read());      // echo any received character
  }
}
