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
// SSD1306 128x64 Pixels I2C OLED graphics demo. 
// Connect an SSD1306 128x64 Pixels I2C OLED to PA6 (SDA) and PA5 (SCL). 
//
// References:
// -----------
// - Neven Boyanov:         https://github.com/tinusaur/ssd1306xled
// - Stephen Denne:         https://github.com/datacute/Tiny4kOLED
// - David Johnson-Davies:  http://www.technoblogy.com/show?3AJ7
// - OLED Font Editor:      http://sourpuss.net/projects/fontedit/
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
#include <oled_gfx.h>                             // OLED graphics functions

// ===================================================================================
// Bitmaps
// ===================================================================================
const uint8_t ufo[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x30, 0x08, 0x04, 0x3A, 0xFA, 0x39, 0x01,
  0x01, 0x31, 0xF8, 0xFA, 0x12, 0x04, 0x18, 0xE0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x0C, 0x3C, 0x7E, 0x7A, 0xF9, 0xFF, 0xFF, 0xCF, 0xC7, 0xCF, 0xFF, 0xFF, 0xFF, 0xFF, 0x9F, 0x8E,
  0x8E, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xCF, 0xCF, 0xFF, 0xFF, 0xFF, 0xFA, 0x7A, 0x7E, 0x3C, 0x08
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
    OLED_home(0, 0);
    OLED_clear();
    for(i=200; i; i--) OLED_setPixel(random(128), random(64), 1);
    for(i=32; i; i--) OLED_drawCircle(100, 50, i, 1);
    OLED_drawSprite(20, 10, 32, 16, ufo);
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
      OLED_drawLine(random(128), random(64), random(128), random(64), 1);
      OLED_refresh();
    }

    OLED_clear();
    for(i = 200; i; i--) {
      OLED_drawRect(random(128), random(64), random(64), random(32), 1);
      OLED_refresh();
    }

    OLED_clear();
    for(i = 200; i; i--) {
      OLED_drawCircle(random(128), random(64), random(16), 1);
      OLED_refresh();
    }

    for(i = 200; i; i--) {
      OLED_print(random(128), random(64), "Hello", 1, 1);
      OLED_print(random(128), random(64), "World", 1, 2);
      OLED_print(random(128), random(64), "Hello", 0, 2);
      OLED_print(random(128), random(64), "World", 0, 1);
      OLED_refresh();
    }
  }
}
