// ===================================================================================
// Basic NeoPixel Functions using Software Bit-banging for CH32V003           * v1.0 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs.
//
// Functions available:
// --------------------
// NEO_init()               init NeoPixels
// NEO_clearAll()           clear all pixels and update
// NEO_clearPixel(p)        clear pixel p
// NEO_writeColor(p,r,g,b)  write RGB color to pixel p
// NEO_writeHue(p,h,b)      write hue (h=0..191) and brightness (b=0..2) to pixel p
// NEO_update()             update pixels string (write buffer to pixels)
//
// NEO_sendByte(d)          send one data byte to pixels string
// NEO_latch()              latch the data sent
//
// Notes:
// ------
// - Works with most 800kHz addressable LEDs (NeoPixels).
// - Set pin, number of pixels, and pixel type in the parameters below!
// - System clock frequency must be 48MHz, 24MHz, 12MHz, or 8MHz.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "gpio.h"

// ===================================================================================
// NeoPixel Definitions
// ===================================================================================
#ifndef PIN_NEO
  #define PIN_NEO       PC1   // define NeoPixel pin (PC0 - PC4 recommended)
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
