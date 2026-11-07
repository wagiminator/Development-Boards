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
// Blink built-in BUSY LED using millis counter (TIMER2).
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with chprog (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the board and keep it pressed while connecting the
//   USB-A female connector (USB host port) to your PC using a USB-A male-to-male
//   cable.
// - Run 'make flash' immediatly afterwards.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "millis.h"                 // millis counter functions

#define PIN_LED   P14               // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  uint32_t next = 100;              // next event in 100ms
  CLK_config();                     // configure system clock
  MIL_init();                       // init and start millis counter
  PIN_output(PIN_LED);              // set LED pin to output

  // Loop
  while(1) {
    while(MIL_read() < next);       // wait for next event
    PIN_toggle(PIN_LED);            // toggle LED
    next += 100;                    // next event in 100ms
  }
}
