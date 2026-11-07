// ===================================================================================
// Millis Functions for SAMD11 (using timer/counter TC1 or TC2)               * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "millis.h"

#if MIL_TIMER == 1
  #define MIL_TC          TC1
  #define MIL_APBMASK     PM_APBCMASK_TC1
  #define MIL_IRQn        TC1_IRQn
  #define MIL_IRQHandler  TC1_IRQHandler
#else
  #define MIL_TC          TC2
  #define MIL_APBMASK     PM_APBCMASK_TC2
  #define MIL_IRQn        TC2_IRQn
  #define MIL_IRQHandler  TC2_IRQHandler
#endif

#define MIL_sync()        while(MIL_TC->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY)

volatile uint32_t MIL_millis = 0;   // millis counter

// Init millis counter
void MIL_init(void) {
  PM->APBCMASK.reg |= MIL_APBMASK;              // enable TC module clock
  GCLK_setCH(TC1_GCLK_ID, GCLK_SYS);            // GCLK0 -> TC1/TC2
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
