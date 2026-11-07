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
// Sends a message via UART1 once a second. Connect pin P1.7 (TX) to RX of a
// USB-to-serial converter (115200 BAUD, 8N1).
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
#include "debug_serial.h"           // serial debug functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  P_SW2 |= 0x80;                    // enable access to extended registers (MUST!)
  DEBUG_init();                     // init DEBUG (TX: P17, BAUD: 115200, 8N1)

  // Loop
  while(1) {
    DEBUG_printf("Hello world!\n"); // send message via UART
    DLY_ms(1000);                   // wait a second
  }
}
