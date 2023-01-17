// ===================================================================================
// Touch Key Functions for CH551, CH552 and CH554
// ===================================================================================
//
// Simple touch key control functions without using baseline detection and interrupts.
//
// The following must be defined in config.h:
// TOUCH_TH_LOW  - key pressed detection threshold  - low  hysteresis value
// TOUCH_TH_HIGH - key released detection threshold - high hysteresis value
// Use touchraw firmware to find out threshold values.
//
// Functions available:
// --------------------
// TOUCH_start(PIN)         start touch on PIN
// TOUCH_read(PIN)          read touch key state (0=released, 1= pressed)
// TOUCH_sample(PIN)        get raw sample value
//
// TOUCH_enable()           turn on capacitance detection
// TOUCH_disable()          turn on capacitance detection
// TOUCH_cycle_1ms()        set sampling period 1ms
// TOUCH_cycle_2ms()        set sampling period 2ms
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"
#include "gpio.h"
#include "config.h"

#define TOUCH_channel(PIN) \
  ((PIN == P10) ? (0) : \
  ((PIN == P11) ? (1) : \
  ((PIN == P14) ? (2) : \
  ((PIN == P15) ? (3) : \
  ((PIN == P16) ? (4) : \
  ((PIN == P17) ? (5) : \
(0)))))))

#define TOUCH_disable()   TKEY_CTRL &= 0xF8       // turn off the capacitance detection
#define TOUCH_enable()    TKEY_CTRL = TKEY_CTRL & 0xF8 | 7 // turn on capacitance detection
#define TOUCH_cycle_1ms() TKEY_CTRL &= ~bTKC_2MS  // set sampling period 1ms
#define TOUCH_cycle_2ms() TKEY_CTRL |= bTKC_2MS   // set sampling period 2ms

#define TOUCH_start(PIN)  PIN_input(PIN)
#define TOUCH_sample(PIN) TOUCH_h_sample(TOUCH_channel(PIN))
#define TOUCH_read(PIN)   TOUCH_h_read(TOUCH_channel(PIN))

// Helper functions
uint16_t TOUCH_h_sample(uint8_t channel);         // get one sample
__bit TOUCH_h_read(uint8_t channel);              // read touch key state
