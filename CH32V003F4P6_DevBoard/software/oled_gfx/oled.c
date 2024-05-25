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
// SSD1306 128x64 Pixels I2C OLED graphics demo. 
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
#include <ssd1306_gfx.h>                            // SSD1306 OLED graphics functions

// ===================================================================================
// Pseudo Random Number Generator
// ===================================================================================
uint32_t random(uint32_t max) {
  static uint32_t rnval = 0xACE1DFEE;
  rnval = rnval << 16 | (rnval << 1 ^ rnval << 2) >> 16;
  return(rnval % max);
}

// ===================================================================================
// Bitmaps
// ===================================================================================
const uint8_t UFO[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x30, 0x08, 0x04, 0x3A, 0xFA, 0x39, 0x01,
  0x01, 0x31, 0xF8, 0xFA, 0x12, 0x04, 0x18, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0C, 0x3C, 0x7E, 0x7A, 0xF9, 0xFF, 0xFF, 0xCF, 0xC7, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0x8E,
  0x8E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFA, 0x7A, 0x7E, 0x3C, 0x08
};

const uint8_t BAT_OK[] = {
  0x7C, 0x04, 0x07, 0x01, 0x07, 0x84, 0xDC, 0x73, 0x46, 0x4C, 0x46, 0x43, 0x41, 0x7C
};

const uint8_t BAT_WEAK[] = {
  0x34, 0x64, 0xC7, 0x81, 0xC7, 0x64, 0x34, 0x58, 0x4C, 0x46, 0x43, 0x46, 0x4C, 0x58
};

const uint8_t ANT_BIG[] = {
  0x03, 0x0C, 0x30, 0xFF, 0x30, 0x0C, 0x03, 0x00, 0x00, 0x00, 0x7F, 0x00, 0x00, 0x00
};

const uint8_t ANT[] = {
  0x01, 0x02, 0x04, 0x7F, 0x04, 0x02, 0x01
};

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Variables
  uint16_t i;
  
  // Setup
  OLED_init();

  // Loop
  while(1) {
    OLED_clear();
    for(i=200; i; i--) OLED_setPixel(random(OLED_WIDTH), random(OLED_HEIGHT), 1);
    for(i=32; i; i--) OLED_drawCircle(100, 50, i, 1);
    OLED_drawSprite(20, 10, 32, 16, UFO);
    OLED_refresh();
    DLY_ms(2000);

    OLED_clear();
    OLED_print(0, 0, "Hello World", 1, 1);
    OLED_print(0, 32, "TEST", 1, 4);
    OLED_print(0, 12, "1234567890", 1, OLED_STRETCH);
    OLED_print(64, 12, "12345", 1, OLED_SMOOTH);
    OLED_refresh();
    DLY_ms(2000);
    for(i=0; i<=64; i++) {
      OLED_vscroll(i);
      DLY_ms(25);
    }
    DLY_ms(1000);

    uint8_t strength = 32;
    uint8_t volume = 10;
    OLED_clear();
    OLED_print(0, 0, "FM Radio", 1, OLED_SMOOTH);
    OLED_drawBitmap(121, 0, 7, 16, BAT_OK);
    OLED_printSegment(-10, 20, 10885, 5, 1, 2);
    OLED_print(94, 36, "MHz", 1, OLED_SMOOTH);
    OLED_drawBitmap(94, 20, 7, 8, ANT);
    OLED_drawRect(104, 20, 24, 7, 1);
    if(strength > 64) strength = 64;
    strength = (strength >> 2) + (strength >> 3);
    if(strength) OLED_fillRect(104, 20, strength, 7, 1);
    OLED_print(0, 56, "Volume:", 1, 1);
    OLED_drawRect(50, 56, 78, 7, 1);
    uint8_t xpos = 47;
    while(volume--) OLED_fillRect(xpos+=5, 58, 4, 3, 1);
    OLED_refresh();
    DLY_ms(2000);

    OLED_clear();
    OLED_fillCircle(32, 32, 30, 1);
    OLED_drawCircle(64 + 32, 32, 30, 1);
    OLED_refresh();
    DLY_ms(2000); OLED_invert(1);
    DLY_ms(2000); OLED_invert(0);
    DLY_ms(2000); OLED_flip(0, 1);
    DLY_ms(2000); OLED_flip(1, 1);

    OLED_clear();
    OLED_fillRect(0, 0, 60, 60, 1);
    OLED_drawRect(64, 0, 60, 60, 1);
    OLED_refresh();
    DLY_ms(2000);

    OLED_clear();
    for(i = 200; i; i--) {
      OLED_drawLine(random(OLED_WIDTH), random(OLED_HEIGHT), random(OLED_WIDTH), random(OLED_HEIGHT), 1);
      OLED_refresh();
    }

    OLED_clear();
    for(i = 200; i; i--) {
      OLED_drawRect(random(OLED_WIDTH), random(OLED_HEIGHT), random(OLED_WIDTH/2), random(OLED_HEIGHT/2), 1);
      OLED_refresh();
    }

    OLED_clear();
    for(i = 200; i; i--) {
      OLED_drawCircle(random(OLED_WIDTH), random(OLED_HEIGHT), random(16), 1);
      OLED_refresh();
    }

    for(i = 200; i; i--) {
      OLED_print(random(OLED_WIDTH), random(OLED_HEIGHT), "Hello", 1, 1);
      OLED_print(random(OLED_WIDTH), random(OLED_HEIGHT), "World", 1, 2);
      OLED_print(random(OLED_WIDTH), random(OLED_HEIGHT), "Hello", 0, 2);
      OLED_print(random(OLED_WIDTH), random(OLED_HEIGHT), "World", 0, 1);
      OLED_refresh();
    }
  }
}
