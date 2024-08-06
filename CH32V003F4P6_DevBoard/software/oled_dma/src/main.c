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
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with rvprog
//   are installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <oled_dma.h>                             // OLED functions

// ===================================================================================
// Pseudo Random Number Generator
// ===================================================================================
uint32_t random(uint32_t max) {
  static uint32_t rnval = 0xDEADBEEF;
  rnval = rnval << 16 | (rnval << 1 ^ rnval << 2) >> 16;
  return(rnval % max);
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  OLED_init();

  // Loop
  while(1) {
    OLED_clear();
    OLED_print(0, 0, "Hello World", 1, 1);
    OLED_print(0, 32, "TEST", 1, 4);
    OLED_stretchPrint(0, 12, "1234567890", 1);
    OLED_smoothPrint(64, 12, "12345", 1);
    DLY_ms(2000);

    OLED_clear();
    OLED_fillCircle(32, 32, 30, 1);
    OLED_drawCircle(64 + 32, 32, 30, 1);
    DLY_ms(2000);

    OLED_clear();
    OLED_fillRect(0, 0, 60, 60, 1);
    OLED_drawRect(64, 0, 60, 60, 1);
    DLY_ms(2000);

    OLED_clear();
    for(uint16_t i = 200; i; i--) {
      OLED_drawLine(random(128), random(64), random(128), random(64), 1);
      DLY_ms(10); // we need to slow it down for our eyes to be able to follow
    }

    OLED_clear();
    for(uint16_t i = 200; i; i--) {
      OLED_drawRect(random(128), random(64), random(64), random(32), 1);
      DLY_ms(10); // we need to slow it down for our eyes to be able to follow
    }

    OLED_clear();
    for(uint16_t i = 200; i; i--) {
      OLED_drawCircle(random(128), random(64), random(16), 1);
      DLY_ms(10); // we need to slow it down for our eyes to be able to follow
    }

    for(uint16_t i = 1000; i; i--) {
      OLED_fillRect(random(128), random(64), random(64), random(32), 1);
      OLED_fillRect(random(128), random(64), random(64), random(32), 0);
    }

    for(uint16_t i = 1000; i; i--) {
      OLED_fillCircle(random(128), random(64), random(16), 1);
      OLED_fillCircle(random(128), random(64), random(16), 0);
    }

    for(uint16_t i = 500; i; i--) {
      OLED_print(random(128), random(64), "Hello", 1, 1);
      OLED_print(random(128), random(64), "World", 1, 2);
      OLED_print(random(128), random(64), "Hello", 0, 2);
      OLED_print(random(128), random(64), "World", 0, 1);
    }
  }
}
