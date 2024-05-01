// ===================================================================================
// Project:   Conway's Game of Life for CH32X035/X034/X033
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
// The Game of Life is a cellular automaton, and was invented by Cambridge 
// mathematician John Conway.
// This game became widely known when it was mentioned in an article published by 
// Scientific American in 1970. It consists of a grid of cells which, based on a 
// few mathematical rules, can live, die or multiply. Depending on the initial 
// conditions, the cells form various patterns throughout the course of the game.
//
// Rules:
// - For a space that is populated:
//   - Each cell with one or no neighbors dies, as if by solitude.
//   - Each cell with four or more neighbors dies, as if by overpopulation.
//   - Each cell with two or three neighbors survives.
// - For a space that is empty or unpopulated:
//   - Each cell with three neighbors becomes populated.
//
// Connect an SSD1306 128x64 Pixels I2C OLED to the MCU pins according to config.h.
//
// References:
// -----------
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
#include "system.h"               // system functions
#include "i2c_soft.h"             // software I2C functions

#define GAME_START    0xACE1DFEE  // define 32-bit game start code

uint8_t page1[1024], page2[1024]; // double screen buffer
uint8_t *src, *dst, *tmp;         // pointers to screen buffers

// ===================================================================================
// SSD1306 128x64 Pixels OLED Definitions
// ===================================================================================

// OLED definitions
#define OLED_ADDR         0x78    // OLED write address (0x3C << 1)
#define OLED_CMD_MODE     0x00    // set command mode
#define OLED_DAT_MODE     0x40    // set data mode

// OLED initialisation sequence
const uint8_t OLED_INIT_CMD[] = {
  0xA8, 0x3F,                     // set multiplex ratio  
  0x8D, 0x14,                     // set DC-DC enable  
  0x20, 0x00,                     // set horizontal addressing mode
  0x21, 0x00, 0x7F,               // set start and end column
  0x22, 0x00, 0x3F,               // set start and end page
  0xDA, 0x12,                     // set com pins
  0xA1, 0xC8,                     // flip screen
  0xAF                            // display on
};

// ===================================================================================
// Pseudo Random Number Generator
// ===================================================================================
uint32_t random(uint32_t max) {
  static uint32_t rnval = GAME_START;
  rnval = rnval << 16 | (rnval << 1 ^ rnval << 2) >> 16;
  return(rnval % max);
}

// ===================================================================================
// Conway's Game of Life
// ===================================================================================

// Get pixel from source screen buffer
uint8_t getpixel(uint8_t xpos, uint8_t ypos) {
  xpos &= 127;
  ypos &= 63;
  return((src[((uint16_t)ypos >> 3) * 128 + xpos] >> (ypos & 7)) & 1);
}

// Set pixel on destination screen buffer
void setpixel(uint8_t xpos, uint8_t ypos) {
  dst[((uint16_t)ypos >> 3) * 128 + xpos] |= ((uint8_t)1 << (ypos & 7));
}

// Calculate next game step
void calculate(void) {
  tmp = src; src = dst; dst = tmp;
  for(uint16_t i=0; i<1024; i++) dst[i] = 0;
  for(uint8_t x=0; x<128; x++) {
    for(uint8_t y=0; y<64; y++) {
      uint8_t neighbors = getpixel(x-1, y-1) \
                        + getpixel(x  , y-1) \
                        + getpixel(x+1, y-1) \
                        + getpixel(x-1, y  ) \
                        + getpixel(x+1, y  ) \
                        + getpixel(x-1, y+1) \
                        + getpixel(x  , y+1) \
                        + getpixel(x+1, y+1);
      if((getpixel(x,y) == 1) && (neighbors >= 2) && (neighbors <= 3)) setpixel(x,y);
      if((getpixel(x,y) == 0) && (neighbors == 3)) setpixel(x,y);
    }
  }
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup random start screen
  src = page1; dst = page2;
  for(uint16_t i=1024; i; i--) setpixel(random(128), random(64));
  
  // Init OLED
  I2C_init();                             // initialize I2C first
  DLY_ms(50);                             // wait for OLED to boot up
  I2C_start(OLED_ADDR);                   // start transmission to OLED
  I2C_write(OLED_CMD_MODE);               // set command mode
  I2C_writeBuffer((uint8_t*)OLED_INIT_CMD, sizeof(OLED_INIT_CMD)); // send init sequence

  // Loop
  while(1) {
    calculate();                          // calculate next game step
    I2C_start(OLED_ADDR);                 // start transmission to OLED
    I2C_write(OLED_DAT_MODE);             // set data mode
    I2C_writeBuffer(dst, 1024);           // send screen buffer using DMA
  }
}
