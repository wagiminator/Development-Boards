// ===================================================================================
// Delay Functions for CH551, CH552 and CH554                                 * v1.1 *
// ===================================================================================

#include "delay.h"

// ===================================================================================
// Delay in Units of us
// ===================================================================================
void DLY_us(uint16_t n) {           // delay in us
  #ifdef F_CPU
    #if F_CPU <= 6000000
      n >>= 2;
    #endif
    #if F_CPU <= 3000000
      n >>= 2;
    #endif
    #if F_CPU <= 750000
      n >>= 4;
    #endif
  #endif

  while(n) {                        // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
    __asm__("inc _SAFE_MOD");       // 2 Fsys cycles, for higher Fsys, add operation here
    #ifdef F_CPU
      #if F_CPU >= 14000000
        __asm__("inc _SAFE_MOD");
      #endif
      #if F_CPU >= 16000000
        __asm__("inc _SAFE_MOD");
      #endif
      #if F_CPU >= 18000000
        __asm__("inc _SAFE_MOD");
      #endif
      #if F_CPU >= 20000000
        __asm__("inc _SAFE_MOD");
      #endif
      #if F_CPU >= 22000000
        __asm__("inc _SAFE_MOD");
      #endif
      #if F_CPU >= 24000000
        __asm__("inc _SAFE_MOD");
      #endif
      #if F_CPU >= 26000000
        __asm__("inc _SAFE_MOD");
      #endif
      #if F_CPU >= 28000000
        __asm__("inc _SAFE_MOD");
      #endif
      #if F_CPU >= 30000000
        __asm__("inc _SAFE_MOD");
      #endif
      #if F_CPU >= 32000000
        __asm__("inc _SAFE_MOD");
      #endif
    #endif
    --n;
  }
}

// ===================================================================================
// Delay in Units of ms
// ===================================================================================
void DLY_ms(uint16_t n) {           // delay in ms
  while(n) {
    while(!(TKEY_CTRL & bTKC_IF));
    while(TKEY_CTRL & bTKC_IF);
    --n;
  }
}
