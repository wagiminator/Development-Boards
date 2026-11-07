// ===================================================================================
// Basic Serial Debug Functions for megaAVR 0-Series                          * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// DEBUG_init()             Init serial DEBUG (USART TX)   with default BAUD rate (115200)
// DEBUG_setBaud(n)         Set BAUD rate (normal speed mode)
//
// DEBUG_write(c)           Send character
// DEBUG_print(s)           Send string
// DEBUG_println(s)         Send string with newline
// DEBUG_printS(s)          Send string (alias)
// DEBUG_printD(n)          Send decimal value as string
// DEBUG_printW(n)          Send 32-bit word hex value as string
// DEBUG_printH(n)          Send 16-bit half-word hex value as string
// DEBUG_printB(n)          Send  8-bit byte hex value as string
// DEBUG_newline()          Send newline
// DEBUG_printf(s, ...)     Uses printf (supports %s, %c, %d, %u, %x, %b, %02d, %%, %ld)
//
// USART TX pin mapping (set below in DEBUG parameters):
// -----------------------------------------------------
// DEBUG_NUM  0          1          2          3 (40/48-pin only)
// DEBUG_TX   0    1     0    1     0    1     0    1
// TX-pin    PA0  PA4   PC0  PC4   PF0  PF4   PB0  PB4
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// DEBUG parameters
#define DEBUG_ENABLE      1                 // enable serial DEBUG (0:no, 1:yes)
#define DEBUG_NUM         0                 // USART number (0, 1, 2, 3)
#define DEBUG_TX          0                 // UART TX pin mapping (see above)
#define DEBUG_BAUD        115200            // default UART baud rate

// Select USART peripheral (internal)
#define DEBUG_CAT(a, b)   a ## b
#define DEBUG_XCAT(a, b)  DEBUG_CAT(a, b)
#define DEBUG_USART       DEBUG_XCAT(USART, DEBUG_NUM)

// DEBUG functions
#if DEBUG_ENABLE > 0
  void DEBUG_init(void);                    // init UART with default BAUD rate
  void DEBUG_write(const char c);           // send character via UART
  void DEBUG_print(const char* str);        // send string via UART
  void DEBUG_println(const char* str);      // send string with newline via UART
  void DEBUG_printD(uint16_t value);        // send decimal value as string
  void DEBUG_printW(uint32_t value);        // send hex word value as string
  void DEBUG_printH(uint16_t value);        // send hex half-word value as string
  void DEBUG_printB(uint8_t value);         // send hex byte value as string
  void DEBUG_printf(const char *format, ...); // use printf (requires more memory)
  #define DEBUG_setBaud(n)  DEBUG_USART.BAUD = (((F_CPU * 8) / (n)) + 1) / 2
  #define DEBUG_newline()   DEBUG_write('\n')
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
  #define DEBUG_setBaud(n)
  #define DEBUG_newline()
#endif

#define DEBUG_setBAUD     DEBUG_setBaud     // alias
#define DEBUG_printS      DEBUG_print       // alias for print
#define DEBUG_out         DEBUG_println     // default DEBUG function

#ifdef __cplusplus
}
#endif
