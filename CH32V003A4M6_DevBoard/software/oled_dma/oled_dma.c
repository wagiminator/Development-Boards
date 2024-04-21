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
// SSD1306 128x64 Pixels I2C OLED demo with constant screen refresh. 
// This implementation utilizes DMA in circular mode for continuous data transfer of 
// the screen buffer to the OLED. Please note that the I2C bus is occupied the whole 
// time, preventing the use of other I2C devices on the same bus.
// Connect an SSD1306 128x64 Pixels I2C OLED to PC1 (SDA) and PC2 (SCL). 
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
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
#include <system.h>                               // system functions
#include <oled_dma.h>                             // OLED functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  OLED_init();

  // Loop
  while(1) {
    for(uint16_t i = 2000; i; i--) {
      OLED_fillRect(random(128), random(64), random(64), random(32), 1);
      OLED_fillRect(random(128), random(64), random(64), random(32), 0);
    }

    OLED_clear();
    for(uint16_t i = 200; i; i--) {
      OLED_drawCircle(random(128), random(64), random(16), 1);
      DLY_ms(10);
    }

    OLED_clear();
    for(uint16_t i = 200; i; i--) {
      OLED_drawRect(random(128), random(64), random(64), random(32), 1);
      DLY_ms(10);
    }

    OLED_clear();
    for(uint16_t i = 200; i; i--) {
      OLED_drawLine(random(128), random(64), random(128), random(64), 1);
      DLY_ms(10);
    }
  }
}
