// ===================================================================================
// Project:   Watchdog Demo for CH551, CH552 and CH554
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
// Flashes the built-in LED by regular watchdog timer resets. Uses the RESET_KEEP
// register to retain LED status across reset.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: doesn't matter
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>                     // user configurations
#include <ch554.h>                      // CH55x header file
#include <clock.h>                      // system clock functions
#include <power.h>                      // power, sleep, reset and watchdog functions
#include <gpio.h>                       // GPIO functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                         // configure system clock
  RST_keep(!RST_getKeeped());           // toggle value in RESET_KEEP register
  PIN_write(PIN_LED, RST_getKeeped());  // set LED according to KEEP value
  WDT_start();                          // start watchdog timer
  WDT_set(250);                         // set watchdog timer to 250ms

  // Loop
  while(1);                             // do nothing, just wait for watchdog reset
}
