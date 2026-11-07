// ===================================================================================
// Serial Debug Functions for CH551, CH552 and CH554                          * v1.1 *
// ===================================================================================
//
// Basic UART TX debug functions. Use printf or the lightweight functions below.
//
// Functions available:
// --------------------
// DEBUG_init()             init UART for debug output
// DEBUG_write(c)           send character
// DEBUG_print(s)           send string
// DEBUG_println(s)         send string with newline
// DEBUG_printD(n)          send 16-bit decimal value
// DEBUG_printH(n)          send 16-bit hex value
// DEBUG_printB(n)          send  8-bit hex value
// putchar(c)               send character (used by printf)
//
// Notes:
// ------
// - printf needs 1-2KB of flash. Use printf_tiny (no field width, no long) or the
//   DEBUG_print functions to save memory.
// - DEBUG_PORT 0 uses TIMER1 as BAUD rate generator.
// - Pins (DEBUG_ALTER 0/1): UART0 TXD P3.1/P1.3, UART1 TXD1 P1.7/P3.2.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdio.h>
#include <stdint.h>
#include "ch554.h"

// DEBUG parameters
#define DEBUG_PORT      1                   // UART port (0 or 1)
#define DEBUG_ALTER     0                   // UART port alternate pins (0:no, 1:yes)
#define DEBUG_BAUD      115200                // UART baud rate

// DEBUG printf configuration
//#define printf printf_tiny
//#define ALWAYS_PRINT_UNSIGNED

// DEBUG calculate BAUD rate setting
#define DEBUG_BAUD_SET  (uint8_t)(256 - (((F_CPU / 8 / DEBUG_BAUD) + 1) / 2))

// Check BAUD rate (register must be 1..255, error should be < 2%)
#define DEBUG_BAUD_DIV  (((F_CPU / 8 / DEBUG_BAUD) + 1) / 2)
#if DEBUG_BAUD_DIV < 1 || DEBUG_BAUD_DIV > 255
  #error DEBUG_BAUD not possible with this F_CPU
#elif ((F_CPU / 16 / DEBUG_BAUD_DIV) > (DEBUG_BAUD + DEBUG_BAUD / 50)) || \
      ((F_CPU / 16 / DEBUG_BAUD_DIV) < (DEBUG_BAUD - DEBUG_BAUD / 50))
  #warning DEBUG_BAUD error > 2% with this F_CPU
#endif

// DEBUG setup
inline void DEBUG_init(void) {
#if DEBUG_PORT == 0
  #if DEBUG_ALTER == 1
  PIN_FUNC |= bUART0_PIN_X;                 // UART0 set alternate pins
  #endif
  SM0    = 0;                               // UART0 8 data bits
  SM1    = 1;                               // UART0 BAUD rate by timer
  SM2    = 0;                               // UART0 no multi-device comm
  RCLK   = 0;                               // UART0 receive clock:  TIMER1
  TCLK   = 0;                               // UART0 transmit clock: TIMER1
  PCON  |= SMOD;                            // UART0 fast BAUD rate
  TMOD   = TMOD & ~(bT1_GATE | bT1_CT | MASK_T1_MOD) | bT1_M1; // TIMER1 8-bit auto-reload
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
#else
  #error Wrong DEBUG_PORT
#endif
}

// DEBUG functions
void DEBUG_write(char c);                   // send character
void DEBUG_print(const char* str);          // send string
void DEBUG_println(const char* str);        // send string with newline
void DEBUG_printD(uint16_t value);          // send 16-bit decimal value
void DEBUG_printH(uint16_t value);          // send 16-bit hex value
void DEBUG_printB(uint8_t value);           // send  8-bit hex value

// UART functions for printf
#if SDCC < 370
void putchar(char c);                       // send character (for printf)
#else
int putchar(int c);                         // send character (for printf)
#endif
