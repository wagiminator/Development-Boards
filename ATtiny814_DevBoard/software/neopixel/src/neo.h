// ===================================================================================
// NeoPixel (Addressable LED) Functions for tinyAVR 0/1-Series                * v1.0 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (WS2812B, SK6812, ...) using
// cycle-accurate bit-banging. Interrupts are disabled while sending a byte.
//
// Functions available:
// --------------------
// NEO_init()               init NeoPixel data pin
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
// - Connect NEO_PIN (define below) to DIN of the pixels string.
// - F_CPU must be at least 8MHz.
// - Set number of pixels and pixel type in the parameters below!
// - Max. 255 pixels (8-bit pixel index), each pixel needs 3 bytes of SRAM.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"

// ===================================================================================
// NeoPixel Parameters
// ===================================================================================
#define NEO_PIN         PA6   // pin connected to DIN of the pixels string
#define NEO_GRB               // type of pixels: NEO_GRB or NEO_RGB
#define NEO_COUNT       8     // total number of pixels in the string
#define NEO_LATCH_TIME  281   // latch time in microseconds

// ===================================================================================
// NeoPixel Functions and Macros
// ===================================================================================
#define NEO_init()      do {PIN_low(NEO_PIN); PIN_output(NEO_PIN);} while(0)
#define NEO_latch()     DLY_us(NEO_LATCH_TIME)

void NEO_sendByte(uint8_t data);
void NEO_update(void);
void NEO_clearAll(void);
void NEO_clearPixel(uint8_t pixel);
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright);

#ifdef __cplusplus
};
#endif
