// ===================================================================================
// Basic NeoPixel Functions using Hardware-SPI for CH32V203                   * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// NEO_init()               init hardware-SPI for NeoPixels on MOSI-pin
// NEO_clearAll()           clear all pixels and update
// NEO_clearPixel(p)        clear pixel p
// NEO_writeColor(p,r,g,b)  write RGB color to pixel p
// NEO_writeHue(p,h,b)      write hue (h=0..191) and brightness (b=0..2) to pixel p
// NEO_update()             update pixels string (write buffer to pixels)
//
// NEO_sendByte(d)          send one data byte via hardware-SPI to pixels string
// NEO_latch()              latch the data sent
//
// SPI MOSI pin mapping (set below in NeoPixel parameters):
// --------------------------------------------------------
// NEO_MAP     0      1      2
// Module    SPI1   SPI1   SPI2 (*)
// MOSI-pin   PA7    PB5    PB15
// (*) SPI2 is only available on CH32V203C8 (and RB).
//
// Notes:
// ------
// - Connect MOSI-pin (define below) to DIN of the pixels string.
// - Works with most 800kHz addressable LEDs (NeoPixels).
// - Set number of pixels and pixel type in the parameters below!
// - Each NeoPixel bit is coded as one SPI byte. Supported system frequencies:
//   12/24/48/96MHz (SPI 6MHz), 72/144MHz (SPI 9MHz), 120MHz (SPI 7.5MHz).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// NeoPixel Definitions
// ===================================================================================
#ifndef NEO_MAP
  #define NEO_MAP       0     // MOSI pin mapping (see above)
#endif

#ifndef NEO_COUNT
  #define NEO_COUNT     8     // total number of pixels in the string
#endif

#define NEO_GRB               // type of pixels: NEO_GRB or NEO_RGB
#define NEO_LATCH_TIME  281   // latch time in microseconds

// ===================================================================================
// NeoPixel Functions and Macros
// ===================================================================================
#define NEO_latch()     DLY_us(NEO_LATCH_TIME)
void NEO_init(void);
void NEO_sendByte(uint8_t data);
void NEO_update(void);
void NEO_clearAll(void);
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright);
void NEO_clearPixel(uint8_t pixel);

#ifdef __cplusplus
};
#endif
