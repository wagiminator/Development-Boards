// ===================================================================================
// Project:   Touchkey Demo for CH551, CH552 and CH554
// Version:   v1.1
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Toggle built-in LED by pressing the touch key.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "src/config.h"       // user configurations
#include "src/system.h"       // system functions
#include "src/gpio.h"         // GPIO functions
#include "src/touch.h"        // touchkey functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();               // configure system clock
  TOUCH_start(PIN_TOUCH);     // start touchkey

  // Loop
  while(1) {
    if(TOUCH_pressed(PIN_TOUCH)) PIN_toggle(PIN_LED); // toggle LED on touch
  }
}
