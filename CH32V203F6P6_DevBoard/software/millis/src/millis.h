// ===================================================================================
// Millis Functions for CH32V203                                              * v1.0 *
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
// The millis (MIL) functions use the SysTick timer and interrupt. The delay (DLY) 
// functions continue to work properly.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

void MIL_init(void);
void MIL_reset(void);
uint32_t MIL_read(void);

#ifdef __cplusplus
};
#endif
