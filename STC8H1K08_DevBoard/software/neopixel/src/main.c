// ===================================================================================
// Project:   Example for STC8H1K08
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
// Shows a colorful animation on a string of NeoPixels (WS2812). The number of
// pixels can be changed in config.h. Connect pin P1.0 to DIN of the pixels string
// (via a ~330 ohms resistor).
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with stc8isp (via pip) are installed. If
//   necessary, a driver for the USB-to-serial converter must be installed.
// - Connect the board via USB to your PC.
// - Run 'make flash' (set ISPPORT in the makefile, if necessary).
// - Perform a power cycle (press and release DOWNLOAD button) when prompted.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "config.h"                 // user configurations
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "neo.h"                    // NeoPixel functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  uint8_t i, j;
  uint8_t nst = 0;
  P_SW2 |= 0x80;                    // enable access to extended registers (MUST!)
  NEO_init();                       // init NeoPixels

  // Loop
  while(1) {
    j = nst;
    for(i=0; i<NEO_COUNT; i++) {
      NEO_writeHue(i, j, 0);        // write hue and brightness to pixel
      j += 192 / NEO_COUNT;
      if(j >= 192) j -= 192;
    }
    NEO_update();                   // update pixels
    if(!nst--) nst = 191;
    DLY_ms(10);                     // wait a bit
  }
}
