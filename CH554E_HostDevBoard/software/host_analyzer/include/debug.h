// ===================================================================================
// Serial Debug Functions for CH551, CH552 and CH554                          * v1.0 *
// ===================================================================================
//
// Basic UART TX debug functions. Use printf commands!
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdio.h>
#include <stdint.h>
#include "ch554.h"

// DEBUG parameters
#define DEBUG_ENABLE    1                   // enable serial DEBUG (0:no, 1:yes)
#define DEBUG_PORT      1                   // UART port (0 or 1)
#define DEBUG_ALTER     0                   // UART port alternate pins (0:no, 1:yes)
#define DEBUG_BAUD      9600                // UART baud rate

// DEBUG printf configuration
//#define printf printf_tiny
//#define ALWAYS_PRINT_UNSIGNED

// DEBUG calculate BAUD rate setting
#define DEBUG_BAUD_SET  (uint8_t)(256 - (((2 * FREQ_SYS / 16 / DEBUG_BAUD) + 1) / 2))

// DEBUG setup
inline void DEBUG_init(void) {
#if DEBUG_PORT == 0
  #if DEBUG_ALTER == 1
  PIN_FUNC |= bUART0_PIN_X;                 // UART0 set alternate pins
  #endif
  SM0    = 0;                               // UART0 8 data bits
  SM1    = 1;                               // UART0 BAUD rate by timer
  SM2    = 0;                               // UART0 no multi-device comm
  TCLK   = 0;                               // UART0 transmit clock: TIMER1
  PCON  |= SMOD;                            // UART0 fast BAUD rate
  TMOD   = TMOD & ~bT1_GATE & ~bT1_CT & ~MASK_T1_MOD | bT1_M1; // TIMER1 8-bit auto-reload
  T2MOD |= bTMR_CLK | bT1_CLK;              // TIMER1 fast clock selection
  TH1    = DEBUG_BAUD_SET;                  // TIMER1 configure for BAUD rate
  TR1    = 1;                               // TIMER1 start
  TI     = 1;                               // UART0 set transmit complete flag
#elif DEBUG_PORT == 1
  #if DEBUG_ALTER == 1
  PIN_FUNC |= bUART1_PIN_X;                 // UART1 set alternate pins
  #endif
  U1SM0  = 0;                               // UART1 8 data bits
  U1SMOD = 1;                               // UART1 fast mode
  SBAUD1 = DEBUG_BAUD_SET;                  // UART1 set BAUD rate
  U1TI   = 1;                               // UART1 set transmit complete flag
#endif
}

// UART functions for printf
#if SDCC < 370
void putchar(char c);                       // send character (for printf)
#else
int putchar(int c);                         // send character (for printf)
#endif
