// ===================================================================================
// Delay Functions for CH551, CH552 and CH554                                 * v1.2 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "delay.h"

// ===================================================================================
// Delay Loop Parameters
// ===================================================================================
// One loop iteration takes DLY_NOPS + 5 cycles. A taken djnz takes 4 cycles plus one
// cycle each if its own address or the target address is odd. With an odd number of
// nops, exactly one of both addresses is odd, regardless of where the linker places
// the function, so djnz always takes 5 cycles.
#if   F_CPU == 24000000
  #define DLY_NOPS    19      // 24 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 16000000
  #define DLY_NOPS    11      // 16 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 12000000
  #define DLY_NOPS    7       // 12 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 6000000
  #define DLY_NOPS    1       //  6 cycles = 1us
  #define DLY_SHIFT   0
#elif F_CPU == 3000000
  #define DLY_NOPS    7       // 12 cycles = 4us
  #define DLY_SHIFT   2
#elif F_CPU == 750000
  #define DLY_NOPS    7       // 12 cycles = 16us
  #define DLY_SHIFT   4
#elif F_CPU == 187500
  #define DLY_NOPS    7       // 12 cycles = 64us
  #define DLY_SHIFT   6
#else
  #error Unsupported F_CPU for delay functions
#endif

// ===================================================================================
// Delay in Units of us
// ===================================================================================
void DLY_us(uint16_t n) {
  n;                          // stop unreferenced argument warning
  __asm
    .rept DLY_SHIFT           ; divide n for low system clock
    clr  c
    mov  a, dph
    rrc  a
    mov  dph, a
    mov  a, dpl
    rrc  a
    mov  dpl, a
    .endm
    mov  a, dpl
    orl  a, dph
    jz   99$                  ; n == 0 -> return
    mov  r6, dpl              ; r6 <- low byte
    mov  r7, dph              ; r7 <- high byte
    mov  a, r6
    jz   02$                  ; low byte != 0 -> one more outer loop
    inc  r7
  02$:
  01$:
    .rept DLY_NOPS
    nop
    .endm
    djnz r6, 01$              ; inner loop
    djnz r7, 01$              ; outer loop
  99$:
  __endasm;
}

// ===================================================================================
// Delay in Units of ms (using the 1ms timer of the touch-key module)
// ===================================================================================
void DLY_ms(uint16_t n) {
  while(n) {
    while(!(TKEY_CTRL & bTKC_IF));  // wait for end of timer cycle
    while(TKEY_CTRL & bTKC_IF);     // wait for start of next cycle
    --n;
  }
}
