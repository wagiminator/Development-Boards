// ===================================================================================
// Millis Functions for CH551, CH552 and CH554                                * v1.0 *
// ===================================================================================
//
// Millisecond counter using TIMER2 in 16-bit auto-reload mode (no drift).
//
// Functions available:
// --------------------
// MIL_init()               init and start millis counter
// MIL_read()               read millis counter (32-bit)
// MIL_read16()             read lower 16 bits of millis counter (faster)
//
// Notes:
// ------
// - The header must be included in the file containing main(), so that SDCC knows
//   the interrupt service routine.
// - TIMER2 is not available for other purposes (e.g. tim2 library, UART0 BAUD).
// - Sets bTMR_CLK in T2MOD (fast timer clock), as the UART libraries do.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

void MIL_init(void);                        // init and start millis counter
uint32_t MIL_read(void);                    // read millis counter
uint16_t MIL_read16(void);                  // read lower 16 bits of millis counter
void MIL_interrupt(void) __interrupt(INT_NO_TMR2);
