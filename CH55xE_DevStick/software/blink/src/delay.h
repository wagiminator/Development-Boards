// ===================================================================================
// Delay Functions for CH551, CH552 and CH554                                 * v1.1 *
// ===================================================================================

#pragma once
#include <stdint.h>
#include "ch554.h"

void DLY_us(uint16_t n);   // delay in units of us
void DLY_ms(uint16_t n);   // delay in units of ms
