// ===================================================================================
// Basic Serial Debug Functions for SAMD21 (SERCOM USART, TX only)           * v1.0 *
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
// DEBUG_TX  0     1     2     3     4     5     6     7     8     9     
// SERCOM    0     0     1     2     2     3     4     4     5     5     
// TX-pin    PA10  PA4   PA16  PA8   PA14  PA22  PB8   PB10  PB22  PB2   No mapping
// Device    EGJ   EGJ   EGJ   EGJ   EGJ   EGJ   GJ    GJ    GJ    GJ    
//
// Notes:
// ------
// - Device row: mapping available on SAMD21E (E), SAMD21G (G) and SAMD21J (J).
// - Don't use the same SERCOM for DEBUG and other libraries (e.g. uart) at the same 
//   time. The default mapping (2: SERCOM1, PA16) is chosen to not conflict with the
//   default mappings of the uart (SERCOM0), spi (SERCOM2) and i2c (SERCOM3) library.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// DEBUG parameters
#define DEBUG_ENABLE      1               // enable serial DEBUG (0:no, 1:yes)
#define DEBUG_TX          0               // UART TX pin mapping (see above)
#define DEBUG_BAUD        115200          // default UART baud rate

// DEBUG pin mapping (SERCOM, TX pin, peripheral function, TX pad)
#if   DEBUG_TX == 0
  #define DEBUG_SERCOM      0
  #define DEBUG_TX_PIN      10          // PA10
  #define DEBUG_PMUX        2           // C
  #define DEBUG_TXPO        1           // PAD2
#elif DEBUG_TX == 1
  #define DEBUG_SERCOM      0
  #define DEBUG_TX_PIN      4           // PA4
  #define DEBUG_PMUX        3           // D
  #define DEBUG_TXPO        0           // PAD0
#elif DEBUG_TX == 2
  #define DEBUG_SERCOM      1
  #define DEBUG_TX_PIN      16          // PA16
  #define DEBUG_PMUX        2           // C
  #define DEBUG_TXPO        0           // PAD0
#elif DEBUG_TX == 3
  #define DEBUG_SERCOM      2
  #define DEBUG_TX_PIN      8           // PA8
  #define DEBUG_PMUX        3           // D
  #define DEBUG_TXPO        0           // PAD0
#elif DEBUG_TX == 4
  #define DEBUG_SERCOM      2
  #define DEBUG_TX_PIN      14          // PA14
  #define DEBUG_PMUX        2           // C
  #define DEBUG_TXPO        1           // PAD2
#elif DEBUG_TX == 5
  #define DEBUG_SERCOM      3
  #define DEBUG_TX_PIN      22          // PA22
  #define DEBUG_PMUX        2           // C
  #define DEBUG_TXPO        0           // PAD0
#elif DEBUG_TX == 6
  #define DEBUG_SERCOM      4
  #define DEBUG_TX_PIN      40          // PB8
  #define DEBUG_PMUX        3           // D
  #define DEBUG_TXPO        0           // PAD0
#elif DEBUG_TX == 7
  #define DEBUG_SERCOM      4
  #define DEBUG_TX_PIN      42          // PB10
  #define DEBUG_PMUX        3           // D
  #define DEBUG_TXPO        1           // PAD2
#elif DEBUG_TX == 8
  #define DEBUG_SERCOM      5
  #define DEBUG_TX_PIN      54          // PB22
  #define DEBUG_PMUX        3           // D
  #define DEBUG_TXPO        1           // PAD2
#elif DEBUG_TX == 9
  #define DEBUG_SERCOM      5
  #define DEBUG_TX_PIN      34          // PB2
  #define DEBUG_PMUX        3           // D
  #define DEBUG_TXPO        0           // PAD0
#else
  #define DEBUG_SERCOM      1           // pin must be set by the application
  #define DEBUG_TXPO        1
#endif

#if (DEBUG_TX == 6) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (DEBUG_TX == 7) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (DEBUG_TX == 8) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (DEBUG_TX == 9) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif

#if (DEBUG_SERCOM > 3) && defined(SAMD21E)
  #error SERCOM4 and SERCOM5 are not available on SAMD21E
#endif

#if   DEBUG_SERCOM == 0
  #define DEBUG_SER          SERCOM0
  #define DEBUG_GCLK_ID      SERCOM0_GCLK_ID_CORE
  #define DEBUG_APBMASK      PM_APBCMASK_SERCOM0
  #define DEBUG_IRQn         SERCOM0_IRQn
  #define DEBUG_IRQHandler   SERCOM0_IRQHandler
  #define DEBUG_DMAC_ID_TX   SERCOM0_DMAC_ID_TX
#elif DEBUG_SERCOM == 1
  #define DEBUG_SER          SERCOM1
  #define DEBUG_GCLK_ID      SERCOM1_GCLK_ID_CORE
  #define DEBUG_APBMASK      PM_APBCMASK_SERCOM1
  #define DEBUG_IRQn         SERCOM1_IRQn
  #define DEBUG_IRQHandler   SERCOM1_IRQHandler
  #define DEBUG_DMAC_ID_TX   SERCOM1_DMAC_ID_TX
#elif DEBUG_SERCOM == 2
  #define DEBUG_SER          SERCOM2
  #define DEBUG_GCLK_ID      SERCOM2_GCLK_ID_CORE
  #define DEBUG_APBMASK      PM_APBCMASK_SERCOM2
  #define DEBUG_IRQn         SERCOM2_IRQn
  #define DEBUG_IRQHandler   SERCOM2_IRQHandler
  #define DEBUG_DMAC_ID_TX   SERCOM2_DMAC_ID_TX
#elif DEBUG_SERCOM == 3
  #define DEBUG_SER          SERCOM3
  #define DEBUG_GCLK_ID      SERCOM3_GCLK_ID_CORE
  #define DEBUG_APBMASK      PM_APBCMASK_SERCOM3
  #define DEBUG_IRQn         SERCOM3_IRQn
  #define DEBUG_IRQHandler   SERCOM3_IRQHandler
  #define DEBUG_DMAC_ID_TX   SERCOM3_DMAC_ID_TX
#elif DEBUG_SERCOM == 4
  #define DEBUG_SER          SERCOM4
  #define DEBUG_GCLK_ID      SERCOM4_GCLK_ID_CORE
  #define DEBUG_APBMASK      PM_APBCMASK_SERCOM4
  #define DEBUG_IRQn         SERCOM4_IRQn
  #define DEBUG_IRQHandler   SERCOM4_IRQHandler
  #define DEBUG_DMAC_ID_TX   SERCOM4_DMAC_ID_TX
#elif DEBUG_SERCOM == 5
  #define DEBUG_SER          SERCOM5
  #define DEBUG_GCLK_ID      SERCOM5_GCLK_ID_CORE
  #define DEBUG_APBMASK      PM_APBCMASK_SERCOM5
  #define DEBUG_IRQn         SERCOM5_IRQn
  #define DEBUG_IRQHandler   SERCOM5_IRQHandler
  #define DEBUG_DMAC_ID_TX   SERCOM5_DMAC_ID_TX
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
