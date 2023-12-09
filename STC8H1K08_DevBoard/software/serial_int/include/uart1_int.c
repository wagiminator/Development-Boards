// ===================================================================================
// UART1 with RX Interrupt and Ring Buffer for STC8H Microcontrollers         * v1.0 *
// ===================================================================================

#include "uart1_int.h"

__xdata uint8_t  UART1_buffer[64];          // UART1 RX ring buffer
volatile uint8_t UART1_rptr      = 0;       // UART1 RX buffer read pointer
volatile uint8_t UART1_wptr      = 0;       // UART1 RX buffer write pointer
volatile __bit   UART1_readyFlag = 1;       // UART1 ready to write flag

// ===================================================================================
// UART1 Interrupt Service Routine
// ===================================================================================
#pragma save
#pragma nooverlay
void UART1_interrupt(void) {
  if(RI) {                                  // RX complete?
    RI = 0;                                 // clear RX interrupt flag
    UART1_buffer[UART1_wptr++] = SBUF;      // push received byte to buffer...
    UART1_wptr &= 63;                       // increase ring buffer pointer
  }
  if(TI) {                                  // TX complete?
    TI = 0;                                 // clear TX interrupt flag
    UART1_readyFlag = 1;                    // set ready to write flag
  }
}
#pragma restore
