// ===================================================================================
// Project:   Example for CH55x
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
// Sends a message via UART1 once a second. Connect pin P1.7 (TXD1) to RX of a
// USB-to-serial converter (115200 BAUD, 8N1).
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with chprog (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the board and keep it pressed while connecting it via
//   USB to your PC.
// - Run 'make flash' immediatly afterwards.


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
  CLK_config();                     // configure system clock
  DEBUG_init();                     // init DEBUG (TXD1: P17, BAUD: 115200, 8N1)

  // Loop
  while(1) {
    DEBUG_println("Hello world!");  // send message via UART
    DLY_ms(1000);                   // wait a second
  }
}
