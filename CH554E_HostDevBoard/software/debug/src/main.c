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
// Sends a message once a second via UART1 and the CH340N USB-to-serial
// converter (115200 BAUD, 8N1).
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
