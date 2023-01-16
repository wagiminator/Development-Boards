// ===================================================================================
// Bootloader Functions for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>
#include "ch554.h"

inline void BOOT_now(void) {
  USB_CTRL = 0;
  EA       = 0;
  TMOD     = 0;
  __asm__("lcall #0x3800");
}
