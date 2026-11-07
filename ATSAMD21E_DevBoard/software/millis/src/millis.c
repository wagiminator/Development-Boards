// ===================================================================================
// Millis Functions for SAMD21 (using timer/counter TC3, TC4 or TC5)          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "millis.h"

#if   MIL_TIMER == 4
  #define MIL_TC          TC4
  #define MIL_APBMASK     PM_APBCMASK_TC4
  #define MIL_GCLK_ID     TC4_GCLK_ID
  #define MIL_IRQn        TC4_IRQn
  #define MIL_IRQHandler  TC4_IRQHandler
#elif MIL_TIMER == 5
  #define MIL_TC          TC5
  #define MIL_APBMASK     PM_APBCMASK_TC5
  #define MIL_GCLK_ID     TC5_GCLK_ID
  #define MIL_IRQn        TC5_IRQn
  #define MIL_IRQHandler  TC5_IRQHandler
#else
  #define MIL_TC          TC3
  #define MIL_APBMASK     PM_APBCMASK_TC3
  #define MIL_GCLK_ID     TC3_GCLK_ID
  #define MIL_IRQn        TC3_IRQn
  #define MIL_IRQHandler  TC3_IRQHandler
#endif

#define MIL_sync()        while(MIL_TC->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY)

volatile uint32_t MIL_millis = 0;   // millis counter

// Init millis counter
void MIL_init(void) {
  PM->APBCMASK.reg |= MIL_APBMASK;              // enable TC module clock
  GCLK_setCH(MIL_GCLK_ID, GCLK_SYS);            // GCLK0 -> TC (shared with TCC2/TC3 or TC4/TC5)
  MIL_TC->COUNT16.CTRLA.reg = TC_CTRLA_SWRST;   // reset timer
  while(MIL_TC->COUNT16.CTRLA.reg & TC_CTRLA_SWRST);
  MIL_TC->COUNT16.CTRLA.reg = TC_CTRLA_MODE_COUNT16       // 16-bit counter
                            | TC_CTRLA_WAVEGEN_MFRQ       // TOP = CC0
                            | TC_CTRLA_PRESCALER_DIV8;    // F_CPU / 8
  MIL_TC->COUNT16.CC[0].reg = F_CPU / 8 / 1000 - 1;       // 1ms period
  MIL_sync();
  MIL_millis = 0;                               // start at zero
  MIL_TC->COUNT16.INTENSET.reg = TC_INTENSET_OVF;         // enable overflow interrupt
  NVIC_EnableIRQ(MIL_IRQn);                     // enable the TC IRQ
  MIL_TC->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE; // start timer
  MIL_sync();
}

// Read millis counter value
uint32_t MIL_read(void) {
  return MIL_millis;
}

// Reset millis counter value
void MIL_reset(void) {
  MIL_millis = 0;
}

// Interrupt service routine
void MIL_IRQHandler(void) {
  MIL_TC->COUNT16.INTFLAG.reg = TC_INTFLAG_OVF; // clear interrupt flag
  MIL_millis++;                                 // increase millis counter
}
