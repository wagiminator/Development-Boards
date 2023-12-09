// ===================================================================================
// Basic Serial Debug Functions for STC8H Microcontrollers                    * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// DEBUG_init()             Init serial DEBUG with default BAUD rate (115200)
// DEBUG_write(c)           Send character
// DEBUG_printf(s, ...)     Uses printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
//
// UART TX pin mapping (set below in DEBUG parameters):
// ----------------------------------------------------
//          MAP 0   MAP 1   MAP 2    MAP 3
// PORT 1   P3.1    P3.7    P1.7     P4.4
// PORT 2   P1.1    P4.7
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator


#pragma once
#include <stdio.h>
#include <stdint.h>
#include "stc8h.h"

// DEBUG parameters
#define DEBUG_ENABLE    1               // enable serial DEBUG (0:no, 1:yes)
#define DEBUG_PORT      1               // UART port (1 or 2)
#define DEBUG_MAP       0               // UART port pin map (0 - 3)
#define DEBUG_TIMER     1               // UART timer (1 or 2) for port 1
#define DEBUG_BAUD      115200          // UART baud rate

// DEBUG calculate BAUD rate setting
#define DEBUG_BAUD_SET  (65536 - (((F_CPU / DEBUG_BAUD) + 2) / 4))

// DEBUG functions and macros
#if DEBUG_ENABLE > 0
  inline void DEBUG_init(void) {
  #if DEBUG_PORT == 1
    #if DEBUG_MAP > 0
    P_SW1 |= DEBUG_MAP << 6;            // UART1 set alternate pins
    #endif
    #if DEBUG_MAP == 1
    P3M0  &= ~(1<<7);
    P3M1  &= ~(1<<7);
    #elif DEBUG_MAP == 2
    P1M0  &= ~(1<<7);
    P1M1  &= ~(1<<7);
    #elif DEBUG_MAP == 3
    P4M0  &= ~(1<<4);
    P4M1  &= ~(1<<4);
    #endif
    SCON = 0x42;                        // TX only, 8 bits, variable baudrate, TI flag
    #if DEBUG_TIMER == 1
    AUXR = 0x40;                        // timer1 in 1T mode as baudrate generator
    TL1  = DEBUG_BAUD_SET & 0xff;       // initial timer value according to BAUD
    TH1  = DEBUG_BAUD_SET >> 8;         // initial timer value according to BAUD
    TR1  = 1;                           // timer1 start run
    #elif DEBUG_TIMER == 2
    AUXR = 0x15;                        // timer2 in 1T mode as baudrate generator
    T2L  = DEBUG_BAUD_SET & 0xff;       // initial timer value according to BAUD
    T2H  = DEBUG_BAUD_SET >> 8;         // initial timer value according to BAUD
    #endif
  #elif DEBUG_PORT == 2
    #if   DEBUG_MAP == 0                // UART2 set alternate pins
    P1M0  &= ~(1<<1);
    P1M1  &= ~(1<<1);
    #elif DEBUG_MAP == 1
    P_SW2 |= 0x01;
    P4M0  &= ~(1<<7);
    P4M1  &= ~(1<<7);
    #endif
    S2CON = 0x02;                       // TX only, 8 bits, set transmit complete flag
    AUXR |= 0x14;                       // timer2 in 1T mode as baudrate generator
    T2L   = DEBUG_BAUD_SET & 0xff;       // initial timer value according to BAUD
    T2H   = DEBUG_BAUD_SET >> 8;        // initial timer value according to BAUD
  #endif
  }

  // UART functions
  void DEBUG_write(const char c);             // send character via UART
  void DEBUG_printf(const char *format, ...); // printf

#else
  #define DEBUG_init()
  #define DEBUG_write(x)
  #define DEBUG_printf(f, ...)
#endif  // DEBUG_ENABLE
