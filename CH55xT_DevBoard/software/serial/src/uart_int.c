// ===================================================================================
// UART with RX Interrupt and Ring Buffer for CH551, CH552 and CH554          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart_int.h"

#if (UART_BUF_SIZE & (UART_BUF_SIZE - 1)) || (UART_BUF_SIZE > 128)
  #error UART_BUF_SIZE must be a power of 2 (max 128)
#endif

__xdata uint8_t UART_buffer[UART_BUF_SIZE]; // RX ring buffer
volatile __idata uint8_t UART_rptr = 0;     // RX buffer read pointer
volatile __idata uint8_t UART_wptr = 0;     // RX buffer write pointer

// Init UART
void UART_init(void) {
#if UART_PORT == 0
  #if UART_ALTER == 1
  PIN_FUNC |= bUART0_PIN_X;                 // UART0 set alternate pins
  #endif
  SCON   = 0x50;                            // UART0 mode 1 (8 bits), RX enable
  RCLK   = 0;                               // UART0 receive clock:  TIMER1
  TCLK   = 0;                               // UART0 transmit clock: TIMER1
  PCON  |= SMOD;                            // UART0 fast BAUD rate
  TMOD   = TMOD & ~(bT1_GATE | bT1_CT | MASK_T1_MOD) | bT1_M1; // TIMER1 8-bit auto-reload
  T2MOD |= bTMR_CLK | bT1_CLK;              // TIMER1 fast clock selection
  TH1    = UART_BAUD_SET;                   // TIMER1 configure for BAUD rate
  TR1    = 1;                               // TIMER1 start
  TI     = 1;                               // UART0 set transmit complete flag
  ES     = 1;                               // UART0 enable interrupt
#elif UART_PORT == 1
  #if UART_ALTER == 1
  PIN_FUNC |= bUART1_PIN_X;                 // UART1 set alternate pins
  #endif
  SCON1  = 0x30;                            // UART1 8 bits, fast mode (U1SMOD), RX enable (U1REN)
  SBAUD1 = UART_BAUD_SET;                   // UART1 set BAUD rate
  U1TI   = 1;                               // UART1 set transmit complete flag
  IE_UART1 = 1;                             // UART1 enable interrupt
#else
  #error Wrong UART_PORT
#endif
  EA = 1;                                   // enable global interrupts
}

// Read byte from buffer
char UART_read(void) {
  char c;
  while(!UART_available());                 // wait for data
  c = UART_buffer[UART_rptr];
  UART_rptr = (UART_rptr + 1) & (UART_BUF_SIZE - 1);
  return c;
}

// Write byte
void UART_write(char c) {
#if UART_PORT == 0
  while(!TI);
  TI = 0;
  SBUF = c;
#else
  while(!U1TI);
  U1TI = 0;
  SBUF1 = c;
#endif
}

// Interrupt service routine (only RX, TX flag is polled)
#if UART_PORT == 0
void UART_interrupt(void) __interrupt(INT_NO_UART0) {
  if(RI) {
    uint8_t next = (UART_wptr + 1) & (UART_BUF_SIZE - 1);
    RI = 0;
    if(next != UART_rptr) {                 // buffer not full?
      UART_buffer[UART_wptr] = SBUF;
      UART_wptr = next;
    }
  }
}
#else
void UART_interrupt(void) __interrupt(INT_NO_UART1) {
  if(U1RI) {
    uint8_t next = (UART_wptr + 1) & (UART_BUF_SIZE - 1);
    U1RI = 0;
    if(next != UART_rptr) {                 // buffer not full?
      UART_buffer[UART_wptr] = SBUF1;
      UART_wptr = next;
    }
  }
}
#endif
