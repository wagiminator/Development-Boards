// ===================================================================================
// Project:   Example for CH32V003
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
// SSD1306 128x64 Pixels I2C OLED text demo. 
// Connect an SSD1306 128x64 Pixels I2C OLED to PC1 (SDA) and PC2 (SCL). 
//
// References:
// -----------
// - CNLohr ch32v003fun:    https://github.com/cnlohr/ch32v003fun
// - Neven Boyanov:         https://github.com/tinusaur/ssd1306xled
// - Stephen Denne:         https://github.com/datacute/Tiny4kOLED
// - David Johnson-Davies:  http://www.technoblogy.com/show?3AJ7
// - OLED Font Editor:      http://sourpuss.net/projects/fontedit/
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <ssd1306_txt.h>                            // SSD1306 OLED text functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  OLED_init();

  // Loop
  while(1) {
    OLED_clear();
    for(uint16_t i=9750; i<=10285; i++) {
      OLED_cursor(0, 0);
      OLED_printSegment(i, 5, 1, 2);
    }
    DLY_ms(3000);

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
    DLY_ms(3000);
  }
}
