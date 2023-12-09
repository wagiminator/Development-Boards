// ===================================================================================
// Basic UART Functions for STC8H Microcontrollers                            * v1.0 *
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
// UART2_init()             Init UART2 with 8N1 and default BAUD rate
// UART2_setBaud(n)         Set BAUD rate
// UART2_ready()            Check if UART is ready to write
// UART2_available()        Check if there is something to read
// UART2_read()             Read character via UART
// UART2_write(c)           Send character via UART
//
// UART3_init()             Init UART3 with 8N1 and default BAUD rate
// UART3_setBaud(n)         Set BAUD rate
// UART3_ready()            Check if UART is ready to write
// UART3_available()        Check if there is something to read
// UART3_read()             Read character via UART
// UART3_write(c)           Send character via UART
//
// UART4_init()             Init UART4 with 8N1 and default BAUD rate
// UART4_setBaud(n)         Set BAUD rate
// UART4_ready()            Check if UART is ready to write
// UART4_available()        Check if there is something to read
// UART4_read()             Read character via UART
// UART4_write(c)           Send character via UART
//
// UART1 pin mapping (set below in UART parameters):
// -------------------------------------------------
// UART1    MAP0   MAP1   MAP2   MAP3
// RX-pin   P3.0   P3.6   P1.6   P4.3
// TX-pin   P3.1   P3.7   P1.7   P4.4
//
// UART2 pin mapping (set below in UART parameters):
// -------------------------------------------------
// UART2    MAP0   MAP1
// RX-pin   P1.0   P4.6
// TX-pin   P1.1   P4.7
//
// UART3 pin mapping (set below in UART parameters):
// -------------------------------------------------
// UART3    MAP0   MAP1
// RX-pin   P0.0   P5.0
// TX-pin   P0.1   P5.1
//
// UART4 pin mapping (set below in UART parameters):
// -------------------------------------------------
// UART4    MAP0   MAP1
// RX-pin   P0.2   P5.2
// TX-pin   P0.3   P5.3
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

// ===================================================================================
// UART Parameters
// ===================================================================================
#define UART1_MAP           0               // UART1 port pin map (0 - 3)
#define UART1_TIMER         1               // UART1 timer (1 or 2)
#define UART1_BAUD          115200          // UART1 default baud rate

#define UART2_MAP           0               // UART2 port pin map (0 or 1)
#define UART2_BAUD          115200          // UART2 default baud rate

#define UART3_MAP           0               // UART3 port pin map (0 or 1)
#define UART3_BAUD          115200          // UART3 default baud rate

#define UART4_MAP           0               // UART4 port pin map (0 or 1)
#define UART4_BAUD          115200          // UART4 default baud rate

// ===================================================================================
// UART1 Functions and Macros
// ===================================================================================

// UART1 state macros
#define UART1_ready()       (TI)        // UART1 ready to write?
#define UART1_available()   (RI)        // UART1 received data byte?

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

  SCON = 0x52;                          // RX/TX, 8 bits, variable baudrate, TI flag
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
}

// UART1 read single byte
inline uint8_t UART1_read(void) {
  while(!RI);                           // wait for incoming data byte
  RI = 0;                               // clear receive complete flag
  return SBUF;                          // return the received byte
}

// UART1 write single byte
inline void UART1_write(uint8_t data) {
  while(!TI);                           // wait for last data byte to be transmitted
  TI = 0;                               // clear transmit complete flag
  SBUF = data;                          // start transmitting data byte
}

// ===================================================================================
// UART2 Functions and Macros
// ===================================================================================

// UART2 state macros
#define UART2_ready()       (S2CON & 0x02)  // UART2 ready to write?
#define UART2_available()   (S2CON & 0x01)  // UART2 received data byte?

// UART2 BAUD rate settings
#define UART2_BAUD_SET  (65536 - (((F_CPU / UART2_BAUD) + 2) / 4))

#define UART2_setBAUD(baud) {                          \
  AUXR &= ~0x10;                                       \
  T2L = (65536 - (((F_CPU / baud) + 2) / 4)) & 0xff;   \
  T2H = (65536 - (((F_CPU / baud) + 2) / 4)) >> 8;     \
  AUXR |= 0x10;                                        \
}

// UART2 setup
inline void UART2_init(void) {
  // Setup pins
  #if   UART2_MAP == 0
    P1M0  &= ~((1<<0) | (1<<1));
    P1M1  &= ~((1<<0) | (1<<1));
  #elif UART2_MAP == 1
    P_SW2 |= 0x01;
    P4M0  &= ~((1<<6) | (1<<7));
    P4M1  &= ~((1<<6) | (1<<7));
  #endif

  S2CON = 0x12;                         // RX/TX, 8 bits, set transmit complete flag
  AUXR &= 0xe3;                         // stop timer2
  T2L   = UART2_BAUD_SET & 0xff;        // initial timer value according to BAUD
  T2H   = UART2_BAUD_SET >> 8;          // initial timer value according to BAUD
  AUXR |= 0x14;                         // start timer2 in 1T mode as baudrate generator
}

// UART2 read single byte
inline uint8_t UART2_read(void) {
  while(!(S2CON & 0x01));               // wait for incoming data byte
  S2CON &= ~0x01;                       // clear receive complete flag
  return S2BUF;                         // return the received byte
}

// UART2 write single byte
inline void UART2_write(uint8_t data) {
  while(!(S2CON & 0x02));               // wait for last data byte to be transmitted
  S2CON &= ~0x02;                       // clear transmit complete flag
  S2BUF = data;                         // start transmitting data byte
}

// ===================================================================================
// UART3 Functions and Macros
// ===================================================================================

// UART3 state macros
#define UART3_ready()       (S3CON & 0x02)  // UART3 ready to write?
#define UART3_available()   (S3CON & 0x01)  // UART3 received data byte?

// UART3 BAUD rate settings
#define UART3_BAUD_SET  (65536 - (((F_CPU / UART3_BAUD) + 2) / 4))

#define UART3_setBAUD(baud) {                          \
  T4T3M &= 0xf0;                                       \
  T3L = (65536 - (((F_CPU / baud) + 2) / 4)) & 0xff;   \
  T3H = (65536 - (((F_CPU / baud) + 2) / 4)) >> 8;     \
  T4T3M |= 0x0a;                                       \
}

// UART3 setup
inline void UART3_init(void) {
  // Setup pins
  #if   UART3_MAP == 0
    P0M0  &= ~((1<<0) | (1<<1));
    P0M1  &= ~((1<<0) | (1<<1));
  #elif UART3_MAP == 1
    P_SW2 |= 0x02;
    P5M0  &= ~((1<<0) | (1<<1));
    P5M1  &= ~((1<<0) | (1<<1));
  #endif
  S3CON  = 0x52;                        // RX/TX, 8 bits, set transmit complete flag
  T4T3M &= 0xf0;                        // stop timer 3
  T3L    = UART3_BAUD_SET & 0xff;       // initial timer value according to BAUD
  T3H    = UART3_BAUD_SET >> 8;         // initial timer value according to BAUD
  T4T3M |= 0x0a;                        // start timer3 in 1T mode as baudrate generator
}

// UART3 read single byte
inline uint8_t UART3_read(void) {
  while(!(S3CON & 0x01));               // wait for incoming data byte
  S3CON &= ~0x01;                       // clear receive complete flag
  return S3BUF;                         // return the received byte
}

// UART3 write single byte
inline void UART3_write(uint8_t data) {
  while(!(S3CON & 0x02));               // wait for last data byte to be transmitted
  S3CON &= ~0x02;                       // clear transmit complete flag
  S3BUF = data;                         // start transmitting data byte
}

// ===================================================================================
// UART4 Functions and Macros
// ===================================================================================

// UART4 state macros
#define UART4_ready()       (S4CON & 0x02)  // UART4 ready to write?
#define UART4_available()   (S4CON & 0x01)  // UART4 received data byte?

// UART4 BAUD rate settings
#define UART4_BAUD_SET  (65536 - (((F_CPU / UART4_BAUD) + 2) / 4))

#define UART4_setBAUD(baud) {                          \
  T4T3M &= 0x0f;                                       \
  T4L = (65536 - (((F_CPU / baud) + 2) / 4)) & 0xff;   \
  T4H = (65536 - (((F_CPU / baud) + 2) / 4)) >> 8;     \
  T4T3M |= 0xa0;                                       \
}

// UART4 setup
inline void UART4_init(void) {
  // Setup pins
  #if   UART4_MAP == 0
    P0M0  &= ~((1<<2) | (1<<3));
    P0M1  &= ~((1<<2) | (1<<3));
  #elif UART4_MAP == 1
    P_SW2 |= 0x04;
    P5M0  &= ~((1<<2) | (1<<3));
    P5M1  &= ~((1<<2) | (1<<3));
  #endif
  S4CON  = 0x52;                        // RX/TX, 8 bits, set transmit complete flag
  T4T3M &= 0x0f;                        // stop timer 4
  T4L    = UART4_BAUD_SET & 0xff;       // initial timer value according to BAUD
  T4H    = UART4_BAUD_SET >> 8;         // initial timer value according to BAUD
  T4T3M |= 0xa0;                        // start timer4 in 1T mode as baudrate generator
}

// UART4 read single byte
inline uint8_t UART4_read(void) {
  while(!(S4CON & 0x01));               // wait for incoming data byte
  S4CON &= ~0x01;                       // clear receive complete flag
  return S4BUF;                         // return the received byte
}

// UART4 write single byte
inline void UART4_write(uint8_t data) {
  while(!(S4CON & 0x02));               // wait for last data byte to be transmitted
  S4CON &= ~0x02;                       // clear transmit complete flag
  S4BUF = data;                         // start transmitting data byte
}
