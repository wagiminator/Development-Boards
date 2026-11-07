// ===================================================================================
// Delay Functions for CH551, CH552 and CH554                                 * v1.2 *
// ===================================================================================
//
// Functions available:
// --------------------
// DLY_us(n)                delay n microseconds (n = 0..65535)
// DLY_ms(n)                delay n milliseconds (n = 0..65535)
//
// Notes:
// ------
// - DLY_us() uses a cycle-counted loop. At F_CPU < 6MHz the resolution is 4us
//   (3MHz), 16us (750kHz) or 64us (187.5kHz). Interrupts extend the delay.
// - DLY_ms() uses the 1ms timer of the touch-key module, which runs independently of
//   the system clock and interrupts. It must not be used while the touch-key module
//   is set to 2ms cycle (TOUCH_cycle_2ms()).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

void DLY_us(uint16_t n);   // delay in units of us
void DLY_ms(uint16_t n);   // delay in units of ms
