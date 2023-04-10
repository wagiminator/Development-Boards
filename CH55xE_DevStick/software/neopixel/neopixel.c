// ===================================================================================
// Project:   NeoPixel Demo for CH551, CH552 and CH554
// Version:   v1.1
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
#include "src/config.h"         // user configurations
#include "src/system.h"         // system functions
#include "src/delay.h"          // delay functions
#include "src/gpio.h"           // GPIO functions
#include "src/neo.h"            // NeoPixel functions

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
