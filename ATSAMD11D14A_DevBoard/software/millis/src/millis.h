// ===================================================================================
// Millis Functions for SAMD11 (using timer/counter TC1 or TC2)               * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// MIL_init()               init and start millis counter at zero
// MIL_read()               read current millis counter value (32-bit)
// MIL_reset()              reset millis counter to zero
//
// Notes:
// ------
// The millis (MIL) functions use a timer/counter (TC1 or TC2, set below) and its
// interrupt, so the delay (DLY) functions, which use the SysTick, continue to work
// properly. The selected timer can no longer be used for other purposes.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// MIL parameters
#define MIL_TIMER     2           // timer/counter to be used (1: TC1, 2: TC2)

#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

void MIL_init(void);
void MIL_reset(void);
uint32_t MIL_read(void);

#ifdef __cplusplus
};
#endif
