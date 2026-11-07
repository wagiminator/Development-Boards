// ===================================================================================
// Basic MAX7219/7221 Segment/Matrix LED Display Functions                    * v1.0 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling a segment/matrix LED
// Display via MAX7219/7221 and SPI.
//
// MAX7219 Display Functions available:
// ------------------------------------
// MAX_send(reg, data)      write data to MAX7219 register
// MAX_off()                put MAX7219 to shutdown
// MAX_on()                 turn on MAX7219
// MAX_setBrightness(b)     set display brightness (0-15)
//
// Segment Display Functions available:
// ------------------------------------
// SEG_init()               init MAX7219 for segment display (BCD decode)
// SEG_clear()              clear all digits
// SEG_printD(val)          print a positive decimal value (with leading zeros)
// SEG_print(val, dotpos)   print value with decimal point at dotpos (no leading zeros)
//
// Matrix Display Functions available (8x8 LED matrix):
// ----------------------------------------------------
// MTX_init()               init MAX7219 for matrix display (no decode)
// MTX_clear()              clear matrix
// MTX_setRow(r, bits)      set row r (0-7) to bit pattern (bit 0 = first column)
// MTX_drawBitmap(*p)       draw 8x8 bitmap (8 bytes, one byte per row)
//
// Notes:
// ------
// - The digits are counted from right to left starting with 0.
// - dotpos > SEG_COUNT - 1 means no decimal point.
// - SPI: mode 0, max. 10MHz, MSB first. DIN = MOSI, CLK = SCK, LOAD/CS = MAX_PIN_CS.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "spi.h"                                // choose your SPI library

// MAX7219 parameters
#define MAX_PIN_CS        P11                   // define CS pin
#define MAX_BRIGHT        5                     // initial display brightness (0-15)
#define MAX_INIT_SPI      1                     // 1: init SPI with SEG_init()/MTX_init()
#define SEG_COUNT         8                     // number of digits of segment display

// MAX7219 register addresses
#define MAX_ADDR_NOP      0x00                  // no operation
#define MAX_ADDR_DIG0     0x01                  // write digit 0 (... 0x08: digit 7)
#define MAX_ADDR_MODE     0x09                  // decode mode
#define MAX_ADDR_BRIGHT   0x0a                  // intensity
#define MAX_ADDR_LIMIT    0x0b                  // scan limit
#define MAX_ADDR_SHTDWN   0x0c                  // shutdown
#define MAX_ADDR_TEST     0x0f                  // display test

// MAX7219 code B font characters (BCD decode mode)
#define SEG_CODE_NEG      0x0a                  // "-"
#define SEG_CODE_E        0x0b                  // "E"
#define SEG_CODE_H        0x0c                  // "H"
#define SEG_CODE_L        0x0d                  // "L"
#define SEG_CODE_P        0x0e                  // "P"
#define SEG_CODE_BLANK    0x0f                  // " "
#define SEG_CODE_DOT      0x80                  // decimal point (add to code)

// MAX7219 functions and macros
void MAX_send(uint8_t reg, uint8_t data);       // MAX7219 send command
#define MAX_setBrightness(b)  MAX_send(MAX_ADDR_BRIGHT, (b))
#define MAX_off()             MAX_send(MAX_ADDR_SHTDWN, 0)
#define MAX_on()              MAX_send(MAX_ADDR_SHTDWN, 1)

// MAX7219 segment display functions
void SEG_init(void);                            // init MAX7219 for segment display
void SEG_clear(void);                           // clear all digits
void SEG_printD(uint32_t value);                // print a decimal value
void SEG_print(int32_t value, uint8_t dotpos);  // print value with decimal at dotpos

// MAX7219 matrix display functions
void MTX_init(void);                            // init MAX7219 for matrix display
void MTX_clear(void);                           // clear matrix
void MTX_drawBitmap(const uint8_t* bmp);        // draw 8x8 bitmap
#define MTX_setRow(r, bits)   MAX_send(MAX_ADDR_DIG0 + (r), (bits))

#ifdef __cplusplus
}
#endif
