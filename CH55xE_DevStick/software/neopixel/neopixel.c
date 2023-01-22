// ===================================================================================
// Project:   NeoPixel Demo for CH551, CH552 and CH554
// Version:   v1.0
// Year:      2022
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Simple demonstration of driving an addressable LED.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: min. 6 MHz internal
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>             // user configurations
#include <ch554.h>              // CH55x header file
#include <clock.h>              // system clock functions
#include <delay.h>              // delay functions
#include <gpio.h>               // GPIO functions
#include <neo.h>                // NeoPixel functions

#define BRIGHT  2               // LED brightness (0..2)

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                 // configure system clock
  NEO_init();                   // init NeoPixel

  // Loop
  while(1) {
    uint8_t i;
    for(i=0; i<192; i++) {      // cycle hue values
      NEO_writeHue(i, BRIGHT);  // write hue and brightness to pixel
      DLY_ms(20);               // delay a bit (also latch)
    }
  }
}
