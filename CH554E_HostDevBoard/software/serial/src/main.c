// ===================================================================================
// Project:   Example for CH554E
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
// Echoes data sent via UART1 and the CH340N USB-to-serial converter (115200
// BAUD, 8N1). Uses interrupts and a ring buffer for RX.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with chprog (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the board and keep it pressed while connecting the
//   USB-A female connector (USB host port) to your PC using a USB-A male-to-male
//   cable.
// - Run 'make flash' immediatly afterwards.
//
// Operating Instructions:
// -----------------------
// - Connect the USB-A male connector of the board to your PC and open a serial
//   monitor (115200 BAUD, 8N1) with the serial port of the CH340N.
// - Send some text, it should be echoed by the device.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "uart_int.h"               // UART with RX interrupt functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                     // configure system clock
  UART_init();                      // init UART1 (TXD1: P17, RXD1: P16, BAUD: 115200)

  // Loop
  while(1) {
    UART_write(UART_read());        // echo any received character
  }
}
