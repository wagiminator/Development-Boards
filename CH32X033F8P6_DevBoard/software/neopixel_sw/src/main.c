// ===================================================================================
// Project:   Example for CH32X035/X034/X033
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
// Shows a colorful animation on a string of NeoPixels. The pin connected to the
// pixels and the number of pixels can be defined in the config.h.
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with
//   chprog (via pip) are installed. In addition, Linux requires access rights to
//   the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <neo_sw.h>                     // NeoPixel Functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  uint8_t i, j;
  uint8_t nst = 0;
  NEO_init();
  
  // Loop
  while(1) {
    j = nst;
    for(i=0; i<NEO_COUNT; i++) {
      NEO_writeHue(i, j, 0);
      j += 192 / NEO_COUNT;
      if(j >= 192) j -= 192;
    }
    NEO_update();
    if(!nst--) nst = 191;
    DLY_ms(10);
  }
}
