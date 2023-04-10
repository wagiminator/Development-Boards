// ===================================================================================
// Project:   Touch-controlled NeoPixel Demo for CH551, CH552 and CH554
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
// Control NeoPixel color by holding the touch key.
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
#include "src/config.h"             // user configurations
#include "src/system.h"             // system functions
#include "src/gpio.h"               // GPIO functions
#include "src/delay.h"              // delay functions
#include "src/touch.h"              // touchkey functions
#include "src/neo.h"                // NeoPixel functions

#define BRIGHT   2                  // define NeoPixel brightness (0..2)

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  uint8_t hue = 0;
  CLK_config();                     // configure system clock
  NEO_init();                       // init NeoPixel
  TOUCH_start(PIN_TOUCH);           // start touchkey

  // Loop
  while(1) {
    if(TOUCH_on(PIN_TOUCH)) {       // touched?
      NEO_writeHue(hue, BRIGHT);    // set hue and brightness
      if(!hue--) hue = 191;         // cycle hue value (0..191)
      DLY_ms(15);                   // a little delay, also NeoPixel latch
    }
  }
}
