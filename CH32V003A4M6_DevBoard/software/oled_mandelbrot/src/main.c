// ===================================================================================
// Project:   OLED Mandelbrot Demo for CH32V003
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
// The Mandelbrot set is a complex set of points that forms a fractal, named after
// mathematician Benoît Mandelbrot. It consists of all complex numbers c for which
// the function z_(n+1) = z_(n)^2 + c does not diverge when iterated from z_(0) = 0.
// The set is typically visualized in the complex plane, displaying intricate,
// self-similar patterns. It exhibits a distinctive shape with a large central "bulb"
// and smaller surrounding "satellite" bulbs, revealing more detailed structures at
// increasing magnification. The Mandelbrot set is an iconic example of how simple
// mathematical rules can generate incredibly complex and beautiful patterns.
//
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
#include "system.h"               // system functions
#include "i2c_tx.h"               // I2C functions

// ===================================================================================
// SSD1306 128x64 Pixels OLED Definitions
// ===================================================================================

// OLED definitions
#define OLED_ADDR         0x78    // OLED write address (0x3C << 1)
#define OLED_CMD_MODE     0x00    // set command mode
#define OLED_DAT_MODE     0x40    // set data mode

// OLED initialisation sequence
const uint8_t OLED_INIT_CMD[] = {
  OLED_CMD_MODE,                  // set commandmode
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
// Mandelbrot Set Functions
// ===================================================================================
#define MANDEL_WIDTH      128     // display width
#define MANDEL_HEIGHT     64      // display hight
#define MANDEL_MAX_ITER   50      // max iterations (50 .. 65525)
#define MANDEL_BOUND      1024    // limit for iteration termination (min 1024)
#define MANDEL_SCALE      256     // scale factor (recommended: 2 * MANDEL_WIDTH)

// Draw Mandelbrot on display
void MANDEL_draw(void) {
  I2C_start(OLED_ADDR);
  I2C_write(OLED_DAT_MODE);
  for(int32_t y = 0; y < MANDEL_HEIGHT; y += 8) {
    int32_t real = (0 - 4 * MANDEL_WIDTH / 5) * MANDEL_SCALE * 3 / MANDEL_WIDTH;
    for(int32_t x = 0; x < MANDEL_WIDTH; x++, real += MANDEL_SCALE * 3 / MANDEL_WIDTH) {
      int32_t imag = (y - MANDEL_HEIGHT / 2) * MANDEL_SCALE * 2 / MANDEL_HEIGHT;
      uint8_t slice = 0;
      for(int32_t i = 8; i; i--, imag += MANDEL_SCALE * 2 / MANDEL_HEIGHT) {
        int32_t z_real = 0;
        int32_t z_imag = 0;
        int32_t iter = 0;
        slice >>= 1;
        while(iter < MANDEL_MAX_ITER) {
          int32_t z_real_sq = (z_real * z_real) / MANDEL_SCALE;
          int32_t z_imag_sq = (z_imag * z_imag) / MANDEL_SCALE;
          if(z_real_sq + z_imag_sq > MANDEL_BOUND) {
            slice |= 0x80;
            break;
          }
          int32_t z_temp = z_real_sq - z_imag_sq + real;
          z_imag = z_real * z_imag / (MANDEL_SCALE / 2) + imag;
          z_real = z_temp;
          iter++;
        }
      }
      I2C_write(slice);
    }
  }
  I2C_stop();
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Init OLED
  I2C_init();
  DLY_ms(50);
  I2C_sendBuffer(OLED_ADDR, (uint8_t*)OLED_INIT_CMD, sizeof(OLED_INIT_CMD));

  // Draw Mandelbrot Set on OLED
  MANDEL_draw();

  // Loop
  while(1);
}
