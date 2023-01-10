// ===================================================================================
// System Clock Functions for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>
#include "ch554.h"

// ===================================================================================
// Configure System Clock
// ===================================================================================
inline void CLK_config(void) {
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;                              // enter safe mode
  
  #if FREQ_SYS == 32000000
    __asm__("orl _CLOCK_CFG, #0b00000111");     // 32MHz
  #elif FREQ_SYS == 24000000
    __asm__("anl _CLOCK_CFG, #0b11111000");
    __asm__("orl _CLOCK_CFG, #0b00000110");     // 24MHz	
  #elif FREQ_SYS == 16000000
    __asm__("anl _CLOCK_CFG, #0b11111000");
    __asm__("orl _CLOCK_CFG, #0b00000101");     // 16MHz	
  #elif FREQ_SYS == 12000000
    __asm__("anl _CLOCK_CFG, #0b11111000");
    __asm__("orl _CLOCK_CFG, #0b00000100");     // 12MHz
  #elif FREQ_SYS == 6000000
    __asm__("anl _CLOCK_CFG, #0b11111000");
    __asm__("orl _CLOCK_CFG, #0b00000011");     // 6MHz	
  #elif FREQ_SYS == 3000000
    __asm__("anl _CLOCK_CFG, #0b11111000");
    __asm__("orl _CLOCK_CFG, #0b00000010");     // 3MHz	
  #elif FREQ_SYS == 750000
    __asm__("anl _CLOCK_CFG, #0b11111000");
    __asm__("orl _CLOCK_CFG, #0b00000001");     // 750kHz	
  #elif FREQ_SYS == 187500
    __asm__("anl _CLOCK_CFG, #0b11111000");     // 187.5kHz		
  #else
    #warning FREQ_SYS invalid or not set
  #endif

  SAFE_MOD = 0x00;                              // terminate safe mode
}

// ===================================================================================
// Set external clock
// ===================================================================================
inline void CLK_external(void) {
  SAFE_MOD = 0x55;
  SAFE_MOD = 0xAA;                              // enter safe mode
  CLOCK_CFG |=  bOSC_EN_XT;                     // enable external crystal
  CLOCK_CFG &= ~bOSC_EN_INT;                    // turn off the internal crystal
  SAFE_MOD = 0x00;                              // terminate safe mode
}
