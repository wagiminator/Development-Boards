// ===================================================================================
// NeoPixel (Addressable LED) Functions for STC8H Microcontrollers             * v1.0 *
// ===================================================================================
//
// Basic control functions for 800kHz addressable LEDs (NeoPixel). A simplified 
// protocol is used which should work with most LEDs.
//
// The following must be defined in config.h:
// PIN_NEO   - pin connected to DATA-IN of the pixel strip (via a ~330 ohms resistor).
// NEO_GRB   - type of pixel: NEO_GRB or NEO_RGB
// NEO_COUNT - total number of pixels
// System clock frequency must be at least 6 MHz, timing is calculated from F_CPU.
//
// Further information:     https://github.com/wagiminator/ATtiny13-NeoController
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "gpio.h"
#include "delay.h"
#include "config.h"

#define NEO_init()  PIN_low(PIN_NEO);PIN_output(PIN_NEO)              // init NeoPixels
#define NEO_latch() DLY_us(281)                                       // latch colors

void NEO_sendByte(uint8_t data);                                      // send a single byte to the pixels
void NEO_clearAll(void);                                              // clear all pixels
void NEO_update(void);                                                // write buffer to pixels
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b);  // write color to pixel in buffer
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright);        // hue (0..191), brightness (0..2)
void NEO_clearPixel(uint8_t pixel);                                   // clear one pixel in buffer
