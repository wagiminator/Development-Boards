// ===================================================================================
// Millis Functions for megaAVR 0-Series                                      * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "millis.h"

static volatile uint32_t MIL_counter = 0; // millis counter

#if MIL_TIMER == 0

#if (F_CPU / 1000) > 65536
  #error F_CPU too high for millis with TCB0
#endif

// Init millis counter (TCB)
void MIL_init(void) {
  TCB0.CTRLB   = TCB_CNTMODE_INT_gc;    // periodic interrupt mode
  TCB0.CCMP    = (F_CPU / 1000) - 1;    // set TOP value (period)
  TCB0.CTRLA   = TCB_ENABLE_bm;         // enable timer/counter
  TCB0.INTCTRL = TCB_CAPT_bm;           // enable periodic interrupt
  INT_enable();                         // global interrupt enable
}

// Timer interrupt service routine (every millisecond)
ISR(TCB0_INT_vect) {
  TCB0.INTFLAGS = TCB_CAPT_bm;          // clear interrupt flag
  MIL_counter++;                        // increase millis counter
}

#else

static uint16_t MIL_acc = 0;            // fractional millisecond accumulator

// Init millis counter (PIT)
void MIL_init(void) {
  RTC_setLSI();                         // RTC clock: internal 32.768kHz
  PIT_enable(32);                       // enable PIT, period = 32 / 32768 = 1/1024 s
  PIT_INT_enable();                     // enable PIT interrupt
  INT_enable();                         // enable interrupts
}

// PIT interrupt service routine (every 1000/1024 milliseconds)
ISR(RTC_PIT_vect) {
  PIT_FLAG_clear();                     // clear PIT interrupt flag
  MIL_acc += 1000;                      // add 1000/1024 ms (exact on average)
  if(MIL_acc >= 1024) {
    MIL_acc -= 1024;
    MIL_counter++;
  }
}

#endif    // MIL_TIMER

// Read millis counter value
uint32_t MIL_read(void) {
  uint32_t result;
  INT_ATOMIC_BLOCK {                              // atomic read
    result = MIL_counter;                         // get millis counter value
  }
  return result;                                  // return value
}
