// ===================================================================================
// Millis Functions for tinyAVR 0-Series and 1-Series                         * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// MIL_init()               init and start millis counter
// MIL_read()               read current millis counter value (32-bit)
//
// Notes:
// ------
// - MIL_TIMER 0 uses TCB0, MIL_TIMER 1 uses the PIT of the RTC (also works in STANDBY,
//   accuracy depends on the internal 32kHz oscillator).
//
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// Millis parameters
#define MIL_TIMER           0   // 0: TCB0 - more accurate, 1: PIT (RTC) - less accurate

// Millis functions
void MIL_init(void);
uint32_t MIL_read(void);

#ifdef __cplusplus
};
#endif
