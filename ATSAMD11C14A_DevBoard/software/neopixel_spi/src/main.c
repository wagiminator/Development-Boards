// ===================================================================================
// Project:   Example for ATSAMD11C14A
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
// Shows a colorful animation on a string of NeoPixels (WS2812) using the
// hardware SPI (SERCOM). The number of pixels can be changed in neo_spi.h.
// Connect pin PA4 (MOSI) to DIN of the pixels string.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with tinyocd (via pip)
//   are installed. If necessary, grant access permissions for the SWD programmer.
// - Connect the SWD programmer (e.g. CMSIS-DAP) to the SWD pins of the board.
// - Run 'make flash'.
// - Alternatively, if the SAM-BA bootloader is installed, set LDSCRIPT and ISPTOOL
//   in the makefile accordingly (see readme) and upload the firmware via USB.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "neo_spi.h"                // NeoPixel functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  uint8_t i, j;
  uint8_t nst = 0;
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
