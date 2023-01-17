// ===================================================================================
// Project:   Touchkey Demo for CH551, CH552, CH554
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
// Turn on built-in LED if touch key is pressed.
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
#include <config.h>           // user configurations
#include <ch554.h>            // CH55x header file
#include <clock.h>            // system clock functions
#include <gpio.h>             // GPIO functions
#include <delay.h>            // delay functions
#include <touch.h>            // touchkey functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  CLK_config();               // configure system clock
  DLY_ms(5);                  // wait for clock to settle
  TOUCH_start(PIN_TOUCH);     // start touchkey and sample baseline

  while(1) {
    PIN_write(PIN_LED, !TOUCH_read(PIN_TOUCH));   // set LED according to touchkey state
  }
}
