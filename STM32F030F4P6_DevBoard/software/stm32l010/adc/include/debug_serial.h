// ===================================================================================
// Basic Serial Debug Functions for STM32L01x, STM32L021, STM32L031           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// DEBUG_init()             Init serial DEBUG (8N1, BAUD: 115200)
// DEBUG_setBaud(n)         Set BAUD rate
//
// DEBUG_write(c)           Send character
// DEBUG_print(s)           Send string
// DEBUG_println(s)         Send string with newline
// DEBUG_printS(s)          Send string (alias)
// DEBUG_printD(n)          Send decimal value as string
// DEBUG_printW(n)          Send 32-bit hex word value as string
// DEBUG_printH(n)          Send 16-bit hex half-word value as string
// DEBUG_printB(n)          Send 8-bit hex byte value as string
// DEBUG_newline()          Send newline
// DEBUG_printf(s, ...)     Uses printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
//
// USART2 TX pin mapping (set below in DEBUG parameters):
// ------------------------------------------------------
// DEBUG_TX   0     1     2     3     4
// TX-pin    PA2   PA9   PA14  PB6   no mapping
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// DEBUG parameters
#define DEBUG_ENABLE      1               // enable serial DEBUG (0:no, 1:yes)
#define DEBUG_TX          1               // UART TX pin mapping (see above)
#define DEBUG_BAUD        115200          // default UART baud rate

// DEBUG functions and macros
#if DEBUG_ENABLE > 0
  void DEBUG_init(void);                  // init UART with default BAUD rate
  void DEBUG_write(const char c);         // send character via UART
  void DEBUG_print(const char* str);      // send string via UART
  void DEBUG_println(const char* str);    // send string with newline via UART
  void DEBUG_printD(uint32_t value);      // send decimal value as string
  void DEBUG_printW(uint32_t value);      // send 32-bit hex word value as string
  void DEBUG_printH(uint16_t value);      // send 16-bit hex half-word value as string
  void DEBUG_printB(uint8_t value);       // send 8-bit hex byte value as string
  void DEBUG_printf(const char *format, ...); // use printf (requires more memory)
#else
  #define DEBUG_init()
  #define DEBUG_write(x)
  #define DEBUG_print(x)
  #define DEBUG_println(x)
  #define DEBUG_printD(x)
  #define DEBUG_printW(x)
  #define DEBUG_printH(x)
  #define DEBUG_printB(x)
  #define DEBUG_printf(f, ...)
#endif

#define DEBUG_setBAUD(n)  USART2->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate
#define DEBUG_newline()   DEBUG_write('\n')                   // send newline
#define DEBUG_printS      DEBUG_print                         // alias for print

#ifdef __cplusplus
};
#endif
