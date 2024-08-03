// ===================================================================================
// Project:   Example for CH32X035/X034/X033
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
// ST7789 240x135 Pixels Color TFT Display Graphics Demo. Connect the TFT as follows:
// MCU - Display
// GND - GND
// VCC - VCC (3V3)
// SCL - PA5
// SDA - PA7
// RES - VCC (3V3)
//  DC - PA6
//  CS - PA4
// BLK - VCC (3V3)
//
// !!! Check operating voltages of your MCU and your display !!!
//
// References:
// -----------
// - David Johnson-Davies:  http://www.technoblogy.com/show?3WAI
// - TFT Font Editor:       http://sourpuss.net/projects/fontedit/
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <st7735_gfx.h>                         // TFT graphics functions

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
  TFT_init();

  // Loop
  while(1) {
    TFT_clear();
    TFT_drawRect(0, 0, TFT_WIDTH, TFT_HEIGHT, TFT_YELLOW);
    TFT_drawVLine(TFT_WIDTH/2, 0, TFT_HEIGHT, TFT_YELLOW);
    TFT_drawHLine(0, TFT_HEIGHT/2, TFT_WIDTH, TFT_YELLOW);
    TFT_cursor(2, 2); TFT_textcolor(TFT_YELLOW); TFT_textsize(1); TFT_print("(0,0)");
    TFT_cursor(2,10); TFT_print("upper left");
    DLY_ms(3000);

    TFT_clear();
    for(i=200; i; i--) TFT_setPixel(random(TFT_WIDTH), random(TFT_HEIGHT), TFT_WHITE);
    for(i=32; i; i--) TFT_drawCircle(100, 50, i, TFT_YELLOW);
    TFT_drawSprite(20, 10, 32, 16, UFO, TFT_MAGENTA);
    TFT_cursor(150,  2); TFT_textcolor(TFT_WHITE); TFT_textsize(1);           TFT_print("Hello World");
    TFT_cursor(150, 12); TFT_textcolor(TFT_RED);   TFT_textsize(TFT_STRETCH); TFT_print("1234567890");
    TFT_cursor(150, 36); TFT_textcolor(TFT_WHITE); TFT_textsize(TFT_SMOOTH);  TFT_print("12345");
    TFT_cursor(150, 60); TFT_textcolor(TFT_GREEN); TFT_textsize(4);           TFT_print("TEST");
    DLY_ms(3000);

    uint8_t strength = 32;
    uint8_t volume = 6;
    TFT_clear();
    TFT_cursor(0, 0); TFT_textcolor(TFT_YELLOW); TFT_textsize(TFT_SMOOTH); TFT_print("FM Radio");
    TFT_drawSprite(121, 0, 7, 16, BAT_OK, TFT_YELLOW);
    TFT_cursor(94, 36); TFT_textcolor(TFT_WHITE); TFT_print("MHz");
    TFT_drawSprite(94, 20, 7, 8, ANT, TFT_WHITE);
    TFT_drawRect(104, 20, 24, 7, TFT_WHITE);
    if(strength > 64) strength = 64;
    strength = (strength >> 2) + (strength >> 3);
    if(strength) TFT_fillRect(104, 20, strength, 7, TFT_WHITE);
    TFT_cursor(0, 56); TFT_textsize(1); TFT_print("Volume:");
    TFT_drawRect(50, 56, 78, 7, TFT_WHITE);
    uint8_t xpos = 47;
    while(volume--) TFT_fillRect(xpos+=5, 58, 4, 3, TFT_WHITE);

    TFT_fillCircle(144 + 25, 25, 25, TFT_WHITE);
    TFT_drawCircle(144 + 55 + 25, 25, 25, TFT_WHITE);
    TFT_fillRect(144, 60, 50, 50, TFT_WHITE);
    TFT_drawRect(144 + 55, 60, 50, 50, TFT_WHITE);

    for(i=9800; i<10280; i++) {
      TFT_cursor(0, 20);
      TFT_printSegment(i, 5, 1, 2);
    }

    DLY_ms(3000);
    TFT_invert(!TFT_INVERT);
    DLY_ms(3000);
    TFT_invert(TFT_INVERT);

    TFT_clear();
    for(i = 30000; i; i--) {
      TFT_setPixel(random(TFT_WIDTH), random(TFT_HEIGHT), random(65536));
    }

    TFT_clear();
    for(i = 1000; i; i--) {
      TFT_drawLine(random(TFT_WIDTH), random(TFT_HEIGHT), random(TFT_WIDTH), random(TFT_HEIGHT), random(65536));
    }

    TFT_clear();
    for(i = 5000; i; i--) {
      TFT_drawRect(random(TFT_WIDTH), random(TFT_HEIGHT), random(TFT_WIDTH/2), random(TFT_HEIGHT/2), random(65536));
    }

    TFT_clear();
    for(i = 1000; i; i--) {
      TFT_drawCircle(random(TFT_WIDTH), random(TFT_HEIGHT), random(32), random(65536));
    }

    TFT_clear();
    for(i = 1000; i; i--) {
      TFT_fillRect(random(TFT_WIDTH), random(TFT_HEIGHT), random(TFT_WIDTH/2), random(TFT_HEIGHT/2), random(65536));
    }

    TFT_clear();
    for(i = 1000; i; i--) {
      TFT_fillCircle(random(TFT_WIDTH), random(TFT_HEIGHT), random(32), random(65536));
    }

    TFT_clear();
    for(i = 500; i; i--) {
      TFT_cursor(random(TFT_WIDTH), random(TFT_HEIGHT)); 
      TFT_textcolor(random(65536)); TFT_textsize(random(3) + 1); TFT_print("Hello");
      TFT_cursor(random(TFT_WIDTH), random(TFT_HEIGHT)); 
      TFT_textcolor(random(65536)); TFT_textsize(random(3) + 1); TFT_print("World");
    }
  }
}
