// ===================================================================================
// Project:   Example for ATtiny3216
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
// pixels can be changed in neo.h. Connect pin PA6 to DIN of the pixels string.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with tinyupdi (via pip)
//   are installed. If necessary, a driver for the CH340N USB-to-serial converter
//   must be installed.
// - Set the Serial Mode Switch on the board to "UPDI" (UPDI programming mode).
// - Connect the board via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "neo.h"                    // NeoPixel functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  uint8_t i, j;
  uint8_t nst = 0;
  CLK_init();                       // set system clock frequency
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
