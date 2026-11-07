// ===================================================================================
// Basic Serial Debug Functions for SAMD11 (SERCOM USART, TX only)           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// DEBUG_init()             Init serial DEBUG (8N1, BAUD: 115200)
// DEBUG_setBAUD(n)         Set BAUD rate
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
// UART TX pin mapping (set below in DEBUG parameters):
// ----------------------------------------------------
// DEBUG_TX   0     1     2     3     4     5     6
// SERCOM     0     0     1     1     1     0     1
// TX-pin     PA4   PA14  PA8   PA24  PA30  PA10  PA22  No mapping
//
// Notes:
// ------
// - Mappings 5 and 6 are only available on SAMD11D.
// - PA24 is a USB pin (D-), PA30 is SWCLK.
// - Don't use the same SERCOM for DEBUG and other libraries (e.g. uart) at the same 
//   time. The default mapping (2: SERCOM1, PA8) is chosen to not conflict with the 
//   default mapping of the uart library (SERCOM0, PA4/PA5).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// DEBUG parameters
#define DEBUG_ENABLE      1               // enable serial DEBUG (0:no, 1:yes)
#define DEBUG_TX          2               // UART TX pin mapping (see above)
#define DEBUG_BAUD        115200          // default UART baud rate

// DEBUG pin mapping (SERCOM, TX pin, peripheral function, TX pad)
#if   DEBUG_TX == 0
  #define DEBUG_SERCOM      0
  #define DEBUG_TX_PIN      4
  #define DEBUG_PMUX        2             // C
  #define DEBUG_TXPO        1             // PAD2
#elif DEBUG_TX == 1
  #define DEBUG_SERCOM      0
  #define DEBUG_TX_PIN      14
  #define DEBUG_PMUX        2             // C
  #define DEBUG_TXPO        0             // PAD0
#elif DEBUG_TX == 2
  #define DEBUG_SERCOM      1
  #define DEBUG_TX_PIN      8
  #define DEBUG_PMUX        2             // C
  #define DEBUG_TXPO        1             // PAD2
#elif DEBUG_TX == 3
  #define DEBUG_SERCOM      1
  #define DEBUG_TX_PIN      24
  #define DEBUG_PMUX        2             // C
  #define DEBUG_TXPO        1             // PAD2
#elif DEBUG_TX == 4
  #define DEBUG_SERCOM      1
  #define DEBUG_TX_PIN      30
  #define DEBUG_PMUX        2             // C
  #define DEBUG_TXPO        0             // PAD0
#elif DEBUG_TX == 5
  #define DEBUG_SERCOM      0
  #define DEBUG_TX_PIN      10
  #define DEBUG_PMUX        2             // C
  #define DEBUG_TXPO        1             // PAD2
#elif DEBUG_TX == 6
  #define DEBUG_SERCOM      1
  #define DEBUG_TX_PIN      22
  #define DEBUG_PMUX        2             // C
  #define DEBUG_TXPO        0             // PAD0
#else
  #define DEBUG_SERCOM      1             // pin must be set by the application
  #define DEBUG_TXPO        1
#endif

#if defined(SAMD11C) && (DEBUG_TX >= 5) && (DEBUG_TX <= 6)
  #error Pin mapping not available on SAMD11C
#endif

#if   DEBUG_SERCOM == 0
  #define DEBUG_SER         SERCOM0
  #define DEBUG_GCLK_ID     SERCOM0_GCLK_ID_CORE
  #define DEBUG_APBMASK     PM_APBCMASK_SERCOM0
#else
  #define DEBUG_SER         SERCOM1
  #define DEBUG_GCLK_ID     SERCOM1_GCLK_ID_CORE
  #define DEBUG_APBMASK     PM_APBCMASK_SERCOM1
#endif

// DEBUG functions and macros
#if DEBUG_ENABLE > 0
  void DEBUG_init(void);                  // init UART with default BAUD rate
  void DEBUG_setBAUD(uint32_t baud);      // set BAUD rate
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
  #define DEBUG_setBAUD(n)
  #define DEBUG_write(x)
  #define DEBUG_print(x)
  #define DEBUG_println(x)
  #define DEBUG_printD(x)
  #define DEBUG_printW(x)
  #define DEBUG_printH(x)
  #define DEBUG_printB(x)
  #define DEBUG_printf(f, ...)
#endif

#define DEBUG_newline()   DEBUG_write('\n')                   // send newline
#define DEBUG_printS      DEBUG_print                         // alias for print

#ifdef __cplusplus
};
#endif
