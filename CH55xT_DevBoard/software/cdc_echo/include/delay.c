// ===================================================================================
// Delay Functions for CH55x
// ===================================================================================

#include "ch554.h"
#include "delay.h"

// ===================================================================================
// Delay in Units of us
// ===================================================================================
void _delay_us(uint16_t n) {      // delay in us
  #ifdef FREQ_SYS
    #if FREQ_SYS <= 6000000
      n >>= 2;
    #endif
    #if FREQ_SYS <= 3000000
      n >>= 2;
    #endif
    #if FREQ_SYS <= 750000
      n >>= 4;
    #endif
  #endif

  while(n) {                      // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
    SAFE_MOD++;                   // 2 Fsys cycles, for higher Fsys, add operation here
    #ifdef FREQ_SYS
      #if FREQ_SYS >= 14000000
        SAFE_MOD++;
      #endif
      #if FREQ_SYS >= 16000000
        SAFE_MOD++;
      #endif
      #if FREQ_SYS >= 18000000
        SAFE_MOD++;
      #endif
      #if FREQ_SYS >= 20000000
        SAFE_MOD++;
      #endif
      #if FREQ_SYS >= 22000000
        SAFE_MOD++;
      #endif
      #if FREQ_SYS >= 24000000
        SAFE_MOD++;
      #endif
      #if FREQ_SYS >= 26000000
        SAFE_MOD++;
      #endif
      #if FREQ_SYS >= 28000000
        SAFE_MOD++;
      #endif
      #if FREQ_SYS >= 30000000
        SAFE_MOD++;
      #endif
      #if FREQ_SYS >= 32000000
		    SAFE_MOD++;
      #endif
    #endif
		n--;
  }
}

// ===================================================================================
// Delay in Units of ms
// ===================================================================================
void _delay_ms(uint16_t n) {             // delay in ms
  while(n) {
    _delay_us(1000);
    n--;
  }
}                                         
