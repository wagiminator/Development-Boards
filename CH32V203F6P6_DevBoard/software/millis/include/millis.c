// ===================================================================================
// Millis Functions for CH32V203                                              * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "millis.h"

volatile uint32_t MIL_millis = 0;   // millis counter

// Init millis counter
void MIL_init(void) {
  STK->CTLR = 0;                    // disable SysTick
  NVIC_EnableIRQ(SysTicK_IRQn);     // enable the SysTick IRQ
  STK->CMPL = DLY_MS_TIME - 1;      // set interval to 1ms
  STK->CMPH = 0;
  STK->CNTL = 0;                    // start at zero
  STK->CNTH = 0;
  STK->CTLR = STK_CTLR_STE          // enable SysTick
            | STK_CTLR_STIE         // enable SysTick compare match interrupt
            | STK_CTLR_STCLK;       // set SysTick clock to F_CPU
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
void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void) {
  uint32_t temp;
  MIL_millis++;                     // increase millis counter
  temp = STK->CMPL;                 // save for 64-bit add
  STK->CMPL += DLY_MS_TIME;         // next interrupt 1ms later
  if(STK->CMPL < temp) STK->CMPH++; // high-word
  STK->SR   = 0;                    // clear interrupt flag
}
