// ===================================================================================
// Project:   Example for CH32V003
// Version:   v1.0
// Year:      2024
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// ST7302 250x122 Pixels Monochrome Low-Power Liquid Crystal Display Graphics Demo. 
// Connect the LCD as follows:
// MCU - Display
// GND - GND
// VCC - VCC (3V3)
// SCL - PC5
// SDA - PC6
// RES - VCC (3V3)
//  DC - PC3
//  CS - PC4
//
// !!! Check operating voltages of your MCU and your display !!!
//
// References:
// -----------
// - CNLohr ch32v003fun:    https://github.com/cnlohr/ch32v003fun
// - David Johnson-Davies:  http://www.technoblogy.com/show?3YB0
// - LCD Font Editor:       http://sourpuss.net/projects/fontedit/
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
#include <st7302_gfx_soft.h>                        // LCD graphics functions

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
  LCD_init();

  // Loop
  while(1) {
    LCD_clear();
    for(i=200; i; i--) LCD_setPixel(random(LCD_WIDTH), random(LCD_HEIGHT), 1);
    for(i=32; i; i--) LCD_drawCircle(100, 50, i, 1);
    LCD_drawSprite(20, 10, 32, 16, UFO);
    LCD_cursor(150,  0); LCD_textsize(1);           LCD_print("Hello World");
    LCD_cursor(150, 12); LCD_textsize(LCD_STRETCH); LCD_print("1234567890");
    LCD_cursor(150, 36); LCD_textsize(LCD_SMOOTH);  LCD_print("12345");
    LCD_cursor(150, 60); LCD_textsize(4);           LCD_print("TEST");
    LCD_flush();
    DLY_ms(3000);

    uint8_t strength = 32;
    uint8_t volume = 10;
    LCD_clear();
    LCD_cursor(0, 0); LCD_textsize(LCD_SMOOTH); LCD_print("FM Radio");
    LCD_drawBitmap(121, 0, 7, 16, BAT_OK);
    LCD_cursor(-10, 20); LCD_printSegment(10885, 5, 1, 2);
    LCD_cursor(94, 36); LCD_print("MHz");
    LCD_drawBitmap(94, 20, 7, 8, ANT);
    LCD_drawRect(104, 20, 24, 7, 1);
    if(strength > 64) strength = 64;
    strength = (strength >> 2) + (strength >> 3);
    if(strength) LCD_fillRect(104, 20, strength, 7, 1);
    LCD_cursor(0, 56); LCD_textsize(1); LCD_print("Volume:");
    LCD_drawRect(50, 56, 78, 7, 1);
    uint8_t xpos = 47;
    while(volume--) LCD_fillRect(xpos+=5, 58, 4, 3, 1);

    LCD_fillCircle(144 + 25, 25, 25, 1);
    LCD_drawCircle(144 + 55 + 25, 25, 25, 1);
    LCD_fillRect(144, 60, 50, 50, 1);
    LCD_drawRect(144 + 55, 60, 50, 50, 1);
    LCD_flush();
    DLY_ms(3000);
    LCD_invert(!LCD_INVERT);
    DLY_ms(3000);
    LCD_invert(LCD_INVERT);

    LCD_clear();
    for(i = 300; i; i--) {
      LCD_drawLine(random(LCD_WIDTH), random(LCD_HEIGHT), random(LCD_WIDTH), random(LCD_HEIGHT), 1);
    }

    LCD_clear();
    for(i = 300; i; i--) {
      LCD_drawRect(random(LCD_WIDTH), random(LCD_HEIGHT), random(LCD_WIDTH/2), random(LCD_HEIGHT/2), 1);
    }

    LCD_clear();
    for(i = 300; i; i--) {
      LCD_drawCircle(random(LCD_WIDTH), random(LCD_HEIGHT), random(16), 1);
    }

    for(i = 100; i; i--) {
      LCD_cursor(random(LCD_WIDTH), random(LCD_HEIGHT));
      LCD_textsize(random(3) + 1);
      LCD_textinvert(random(2));
      LCD_print("Hello");
    }
    LCD_textinvert(0);
  }
}
