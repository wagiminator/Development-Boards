// ===================================================================================
// UART1 with RX Interrupt and Ring Buffer for STC8H Microcontrollers         * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART1_init()             Init UART1 with 8N1 and default BAUD rate
// UART1_setBaud(n)         Set BAUD rate
// UART1_ready()            Check if UART is ready to write
// UART1_available()        Check if there is something to read
// UART1_read()             Read character via UART
// UART1_write(c)           Send character via UART
//
// UART1 pin mapping (set below in UART parameters):
// -------------------------------------------------
// UART1    MAP0   MAP1   MAP2   MAP3
// RX-pin   P3.0   P3.6   P1.6   P4.3
// TX-pin   P3.1   P3.7   P1.7   P4.4
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

// ===================================================================================
// UART1 Parameters
// ===================================================================================
#define UART1_MAP           0               // UART1 port pin map (0 - 3)
#define UART1_TIMER         1               // UART1 timer (1 or 2)
#define UART1_BAUD          115200          // UART1 default baud rate

// ===================================================================================
// UART1 Functions and Macros
// ===================================================================================

// UART1 state macros
#define UART1_ready()     (UART1_readyFlag) // ready to send data?
#define UART1_available() (UART1_rptr != UART1_wptr)  // something in buffer?

// UART1 variables
extern __xdata uint8_t  UART1_buffer[];     // UART1 RX ring buffer
extern volatile uint8_t UART1_rptr;         // UART1 RX buffer read pointer
extern volatile uint8_t UART1_wptr;         // UART1 RX buffer write pointer
extern volatile __bit   UART1_readyFlag;    // UART1 ready to write flag

// UART1 BAUD rate setting
#define UART1_BAUD_SET  (65536 - (((F_CPU / UART1_BAUD) + 2) / 4))

#if UART1_TIMER == 1
  #define UART1_setBAUD(baud) {                        \
    TR1 = 0;                                           \
    TL1 = (65536 - (((F_CPU / baud) + 2) / 4)) & 0xff; \
    TH1 = (65536 - (((F_CPU / baud) + 2) / 4)) >> 8;   \
    TR1 = 1;                                           \
  }
#elif UART1_TIMER == 2
  #define UART1_setBAUD(baud) {                        \
    AUXR &= ~0x10;                                     \
    T2L = (65536 - (((F_CPU / baud) + 2) / 4)) & 0xff; \
    T2H = (65536 - (((F_CPU / baud) + 2) / 4)) >> 8;   \
    AUXR |= 0x10;                                      \
  }
#endif

// UART1 setup
inline void UART1_init(void) {
  // Setup pins
  #if UART1_MAP > 0
    P_SW1 |= DEBUG_MAP << 6;
  #endif
  #if UART1_MAP == 1
    P3M0  &= ~((1<<6) | (1<<7));
    P3M1  &= ~((1<<6) | (1<<7));
  #elif UART1_MAP == 2
    P1M0  &= ~((1<<6) | (1<<7));
    P1M1  &= ~((1<<6) | (1<<7));
  #elif UART1_MAP == 3
    P4M0  &= ~((1<<3) | (1<<4));
    P4M1  &= ~((1<<3) | (1<<4));
  #endif

  SCON = 0x50;                          // RX/TX, 8 bits, variable baudrate
  #if UART1_TIMER == 1
    AUXR &= 0x9e;
    AUXR |= 0x40;                       // timer1 in 1T mode as baudrate generator
    TL1   = UART1_BAUD_SET & 0xff;      // initial timer value according to BAUD
    TH1   = UART1_BAUD_SET >> 8;        // initial timer value according to BAUD
    TR1   = 1;                          // start timer1
  #elif UART1_TIMER == 2
    AUXR &= 0xe3;                       // stop timer2
    T2L   = UART1_BAUD_SET & 0xff;      // initial timer value according to BAUD
    T2H   = UART1_BAUD_SET >> 8;        // initial timer value according to BAUD
    AUXR |= 0x15;                       // start timer2 in 1T mode as baudrate generator
  #endif

  ES = 1;                               // enable UART1 interrupt
  EA = 1;                               // enable global interrupt
}

// UART1 read single byte
inline uint8_t UART1_read(void) {
  uint8_t result;
  while(!UART1_available());            // wait for incoming data byte
  result = UART1_buffer[UART1_rptr++];  // get data byte from buffer
  UART1_rptr &= 63;                     // increase ring buffer pointer
  return result;                        // return the received byte
}

// UART1 write single byte
inline void UART1_write(uint8_t data) {
  while(!UART1_ready());                // wait for ready to write
  UART1_readyFlag = 0;                  // clear ready to write flag
  SBUF = data;                          // start transmitting data byte
}
