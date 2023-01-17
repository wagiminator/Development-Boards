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
// TOUCH_read(PIN)          read touchkey state (see below)
// TOUCH_sample(PIN)        get raw sample value
//
// TOUCH_pressed(PIN)       check if touchkey has just been pressed
// TOUCH_on(PIN)            check if touchkey is still pressed
// TOUCH_released(PIN)      check if touchkey has just been released
// TOUCH_off(PIN)           check if touchkey is still released
//
// TOUCH_enable()           turn on capacitance detection
// TOUCH_disable()          turn on capacitance detection
// TOUCH_cycle_1ms()        set sampling period 1ms
// TOUCH_cycle_2ms()        set sampling period 2ms
//
// Touch key states:
// -----------------
// TOUCH_PRESSED            touchkey has just been pressed
// TOUCH_ON                 touchkey is still pressed
// TOUCH_RELEASED           touchkey has just been released
// TOUCH_OFF                touchkey ist sill released
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

#define TOUCH_disable()     TKEY_CTRL &= 0xF8
#define TOUCH_enable()      TKEY_CTRL = TKEY_CTRL & 0xF8 | 7
#define TOUCH_cycle_1ms()   TKEY_CTRL &= ~bTKC_2MS
#define TOUCH_cycle_2ms()   TKEY_CTRL |= bTKC_2MS

#define TOUCH_start(PIN)    PIN_input(PIN)
#define TOUCH_sample(PIN)   TOUCH_h_sample(TOUCH_channel(PIN))
#define TOUCH_read(PIN)     TOUCH_h_read(TOUCH_channel(PIN))

#define TOUCH_pressed(PIN)  (TOUCH_read(PIN) == TOUCH_PRESSED)
#define TOUCH_on(PIN)       (TOUCH_read(PIN) == TOUCH_ON)
#define TOUCH_released(PIN) (TOUCH_read(PIN) == TOUCH_RELEASED)
#define TOUCH_off(PIN)      (TOUCH_read(PIN) == TOUCH_OFF)

enum {TOUCH_PRESSED, TOUCH_ON, TOUCH_RELEASED, TOUCH_OFF};

// Helper functions
uint16_t TOUCH_h_sample(uint8_t channel);         // get one sample
uint8_t TOUCH_h_read(uint8_t channel);            // read touch key state
