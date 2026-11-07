// ===================================================================================
// UART with RX Interrupt and Ring Buffer for CH551, CH552 and CH554          * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              init UART (8N1) with RX interrupt
// UART_available()         check if received data is in the buffer
// UART_read()              read data byte from buffer (waits for data)
// UART_write(c)            write data byte (waits until UART is ready)
// UART_ready()             check if UART is ready to write
//
// Pins (UART_ALTER 0/1):
// ----------------------
// UART_PORT 0: RXD P3.0 / P1.2, TXD P3.1 / P1.3 (uses TIMER1 as BAUD rate generator)
// UART_PORT 1: RXD1 P1.6 / P3.4, TXD1 P1.7 / P3.2 (not available on CH551)
//
// Notes:
// ------
// - The header must be included in the file containing main(), so that SDCC knows
//   the interrupt service routine.
// - Received bytes are dropped if the buffer is full.
// - UART_write() can be used with the print functions (print.h).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

// UART parameters
#define UART_PORT       1                   // UART port (0 or 1)
#define UART_ALTER      0                   // UART alternate pins (0:no, 1:yes)
#define UART_BAUD       115200                // UART baud rate
#define UART_BUF_SIZE   16                  // RX buffer size (power of 2, max 128)

// BAUD rate calculation and check (error should be < 2%)
#define UART_BAUD_DIV   (((F_CPU / 8 / UART_BAUD) + 1) / 2)
#define UART_BAUD_SET   (uint8_t)(256 - UART_BAUD_DIV)
#if UART_BAUD_DIV < 1 || UART_BAUD_DIV > 255
  #error UART_BAUD not possible with this F_CPU
#elif ((F_CPU / 16 / UART_BAUD_DIV) > (UART_BAUD + UART_BAUD / 50)) || \
      ((F_CPU / 16 / UART_BAUD_DIV) < (UART_BAUD - UART_BAUD / 50))
  #warning UART_BAUD error > 2% with this F_CPU
#endif

// UART variables
extern volatile __idata uint8_t UART_rptr;  // RX buffer read pointer
extern volatile __idata uint8_t UART_wptr;  // RX buffer write pointer

// UART functions
void UART_init(void);                       // init UART
char UART_read(void);                       // read byte from buffer
void UART_write(char c);                    // write byte

#define UART_available()  (UART_rptr != UART_wptr)
#if UART_PORT == 0
  #define UART_ready()    (TI)
  void UART_interrupt(void) __interrupt(INT_NO_UART0);
#else
  #define UART_ready()    (U1TI)
  void UART_interrupt(void) __interrupt(INT_NO_UART1);
#endif
