// ===================================================================================
// Delay Functions for STC8H Microcontrollers                                 * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "delay.h"

void DLY_us_h(uint16_t n) {
  n;

  __asm
  
    mov a, dpl
    jz 01$
    inc dph

    01$:
    #if F_CPU >= 3000000
      #if F_CPU % 3000000 >= 1000000
        nop
      #endif
      #if F_CPU % 3000000 >= 2000000
        nop
      #endif
    #endif

    #if F_CPU >= 6000000
      mov r7, #((F_CPU / 3000000) - 1)
      02$:
      djnz r7, 02$
    #endif
    
    djnz dpl, 01$
    djnz dph, 01$

  __endasm;
}

void DLY_ms_h(uint16_t n) {
  n;

  __asm

    mov r5, dpl
    mov r6, dph
    mov a, r5

    jz 03$
    inc r6

    03$:
    #if F_CPU < 3000000
      mov dptr, #(F_CPU / 3000)
    #else
      mov dptr, #1000
    #endif

    lcall _DLY_us_h

    djnz r5, 03$
    djnz r6, 03$

  __endasm;
}
