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
// SSD1306 128x64 pixels I2C OLED text demo (text sizes, segment digits and
// bitmaps). Connect the OLED to P3.3 (SDA) and P3.2 (SCL).
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
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "ssd1306_txt.h"            // SSD1306 OLED text functions

// ===================================================================================
// Bitmaps
// ===================================================================================
const uint8_t UFO[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x30, 0x08, 0x04, 0x3A, 0xFA, 0x39, 0x01,
  0x01, 0x31, 0xF8, 0xFA, 0x12, 0x04, 0x18, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0C, 0x3C, 0x7E, 0x7A, 0xF9, 0xFF, 0xFF, 0xCF, 0xC7, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0x8E,
  0x8E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFA, 0x7A, 0x7E, 0x3C, 0x08
};

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  P_SW2 |= 0x80;                    // enable access to extended registers (MUST!)
  OLED_init();                      // init OLED

  // Loop
  while(1) {
    uint16_t i;
    OLED_clear();
    for(i=9750; i<=10285; i++) {
      OLED_cursor(0, 0);
      OLED_printSegment(i, 5, 1, 2);
    }
    DLY_ms(1000);

    OLED_clear();
    OLED_textsize(0);
    OLED_print("Test 01234\n");
    OLED_textinvert(1);
    OLED_print("Test 01234\n");
    OLED_textinvert(0);
    OLED_textsize(1);
    OLED_print("Test 01234\n");
    OLED_textsize(2);
    OLED_print("Test 01234\n");
    OLED_cursor(95, 0);
    OLED_drawBitmap(UFO, 32, 2);
    DLY_ms(3000);
  }
}
