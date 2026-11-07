// ===================================================================================
// Millis Functions for CH551, CH552 and CH554                                * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "millis.h"

// TIMER2 clock = Fsys (bTMR_CLK = 1 is also used by the UART libraries for TIMER1)
#define MIL_TICKS       (F_CPU / 1000)      // timer ticks per millisecond
#define MIL_RELOAD      (uint16_t)(65536 - MIL_TICKS)

static volatile uint32_t MIL_counter = 0;   // millis counter

// Init and start millis counter
void MIL_init(void) {
  T2CON  = 0;                               // timer mode, auto-reload, stopped
  T2MOD |= bTMR_CLK | bT2_CLK;              // TIMER2 clock = Fsys
  RCAP2L = (uint8_t)MIL_RELOAD;             // set reload value
  RCAP2H = (uint8_t)(MIL_RELOAD >> 8);
  TL2    = RCAP2L;                          // set start value
  TH2    = RCAP2H;
  ET2    = 1;                               // enable TIMER2 interrupt
  TR2    = 1;                               // start TIMER2
  EA     = 1;                               // enable global interrupts
}

// Read millis counter
uint32_t MIL_read(void) {
  uint32_t result;
  ET2 = 0;                                  // atomic read
  result = MIL_counter;
  ET2 = 1;
  return result;
}

// Read lower 16 bits of millis counter
uint16_t MIL_read16(void) {
  uint16_t result;
  ET2 = 0;
  result = (uint16_t)MIL_counter;
  ET2 = 1;
  return result;
}

// TIMER2 interrupt service routine (every millisecond)
void MIL_interrupt(void) __interrupt(INT_NO_TMR2) {
  TF2 = 0;                                  // clear interrupt flag
  MIL_counter++;                            // increase millis counter
}
