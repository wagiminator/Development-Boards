// ===================================================================================
// Basic UART Functions (SERCOM USART) for SAMD21  (no buffer, no interrupt)  * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART_setBAUD(n)          Set BAUD rate
// UART_setDataBits(n)      Set number of data bits (n = 5, 6, 7, 8, 9)
// UART_setStopBits(n)      Set number of stop bits (n = 1, 2)
// UART_setNoParity()       Set no parity bit
// UART_setOddParity()      Set parity bit, odd
// UART_setEvenParity()     Set parity bit, even
//
// UART_ready()             Check if UART is ready to write
// UART_available()         Check if there is something to read
// UART_completed()         Check if transmission is completed
//
// UART_read()              Read character via UART
// UART_write(c)            Send character via UART
//
// UART_enable()            Enable USART
// UART_disable()           Disable USART
// UART_TX_enable()         Enable transmitter
// UART_TX_disable()        Disable transmitter
// UART_RX_enable()         Enable receiver
// UART_RX_disable()        Disable receiver
// UART_LP_enable()         Enable operation in STANDBY (clock must run in standby)
// UART_LP_disable()        Disable operation in STANDBY
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// UART_printf(f, ...)      printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// UART_printD(n)           Print decimal value
// UART_printW(n)           Print 32-bit hex word value
// UART_printH(n)           Print 16-bit hex half-word value
// UART_printB(n)           Print  8-bit hex byte value
// UART_printS(s)           Print string
// UART_print(s)            Print string (alias)
// UART_println(s)          Print string with newline
// UART_newline()           Send newline
//
// UART pin mapping (set below in UART parameters):
// ------------------------------------------------
// UART_MAP  0     1     2     3     4     5     6     7     8     9     10    
// SERCOM    0     0     1     2     2     3     4     4     5     5     1     
// TX-pin    PA10  PA4   PA16  PA8   PA14  PA22  PB8   PB10  PB22  PB2   PA0   No mapping
// RX-pin    PA11  PA5   PA17  PA9   PA15  PA23  PB9   PB11  PB23  PB3   PA1   No mapping
// Device    EGJ   EGJ   EGJ   EGJ   EGJ   EGJ   GJ    GJ    GJ    GJ    EGJ   
//
// Notes:
// ------
// - Device row: mapping available on SAMD21E (E), SAMD21G (G) and SAMD21J (J).
// - PA0/PA1 are the pins for the 32.768kHz crystal (XOSC32K).
// - The SERCOM is clocked by GCLK0 (F_CPU), max BAUD rate is F_CPU / 16.
// - The fractional BAUD rate generator is used (BAUD error < 0.5% for most rates).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define UART_BAUD             115200      // default UART baud rate
#define UART_MAP              0           // UART pin mapping (see above)
#define UART_PRINT            0           // 1: include print functions (needs print.h)

// UART pin mapping (SERCOM, TX/RX pin, peripheral function, TX/RX pad)
#if   UART_MAP == 0
  #define UART_SERCOM        0
  #define UART_TX_PIN        10          // PA10
  #define UART_RX_PIN        11          // PA11
  #define UART_TX_PMUX       2           // C
  #define UART_RX_PMUX       2           // C
  #define UART_TXPO          1           // TX on PAD2
  #define UART_RXPO          3           // RX on PAD3
#elif UART_MAP == 1
  #define UART_SERCOM        0
  #define UART_TX_PIN        4           // PA4
  #define UART_RX_PIN        5           // PA5
  #define UART_TX_PMUX       3           // D
  #define UART_RX_PMUX       3           // D
  #define UART_TXPO          0           // TX on PAD0
  #define UART_RXPO          1           // RX on PAD1
#elif UART_MAP == 2
  #define UART_SERCOM        1
  #define UART_TX_PIN        16          // PA16
  #define UART_RX_PIN        17          // PA17
  #define UART_TX_PMUX       2           // C
  #define UART_RX_PMUX       2           // C
  #define UART_TXPO          0           // TX on PAD0
  #define UART_RXPO          1           // RX on PAD1
#elif UART_MAP == 3
  #define UART_SERCOM        2
  #define UART_TX_PIN        8           // PA8
  #define UART_RX_PIN        9           // PA9
  #define UART_TX_PMUX       3           // D
  #define UART_RX_PMUX       3           // D
  #define UART_TXPO          0           // TX on PAD0
  #define UART_RXPO          1           // RX on PAD1
#elif UART_MAP == 4
  #define UART_SERCOM        2
  #define UART_TX_PIN        14          // PA14
  #define UART_RX_PIN        15          // PA15
  #define UART_TX_PMUX       2           // C
  #define UART_RX_PMUX       2           // C
  #define UART_TXPO          1           // TX on PAD2
  #define UART_RXPO          3           // RX on PAD3
#elif UART_MAP == 5
  #define UART_SERCOM        3
  #define UART_TX_PIN        22          // PA22
  #define UART_RX_PIN        23          // PA23
  #define UART_TX_PMUX       2           // C
  #define UART_RX_PMUX       2           // C
  #define UART_TXPO          0           // TX on PAD0
  #define UART_RXPO          1           // RX on PAD1
#elif UART_MAP == 6
  #define UART_SERCOM        4
  #define UART_TX_PIN        40          // PB8
  #define UART_RX_PIN        41          // PB9
  #define UART_TX_PMUX       3           // D
  #define UART_RX_PMUX       3           // D
  #define UART_TXPO          0           // TX on PAD0
  #define UART_RXPO          1           // RX on PAD1
#elif UART_MAP == 7
  #define UART_SERCOM        4
  #define UART_TX_PIN        42          // PB10
  #define UART_RX_PIN        43          // PB11
  #define UART_TX_PMUX       3           // D
  #define UART_RX_PMUX       3           // D
  #define UART_TXPO          1           // TX on PAD2
  #define UART_RXPO          3           // RX on PAD3
#elif UART_MAP == 8
  #define UART_SERCOM        5
  #define UART_TX_PIN        54          // PB22
  #define UART_RX_PIN        55          // PB23
  #define UART_TX_PMUX       3           // D
  #define UART_RX_PMUX       3           // D
  #define UART_TXPO          1           // TX on PAD2
  #define UART_RXPO          3           // RX on PAD3
#elif UART_MAP == 9
  #define UART_SERCOM        5
  #define UART_TX_PIN        34          // PB2
  #define UART_RX_PIN        35          // PB3
  #define UART_TX_PMUX       3           // D
  #define UART_RX_PMUX       3           // D
  #define UART_TXPO          0           // TX on PAD0
  #define UART_RXPO          1           // RX on PAD1
#elif UART_MAP == 10
  #define UART_SERCOM        1
  #define UART_TX_PIN        0           // PA0
  #define UART_RX_PIN        1           // PA1
  #define UART_TX_PMUX       3           // D
  #define UART_RX_PMUX       3           // D
  #define UART_TXPO          0           // TX on PAD0
  #define UART_RXPO          1           // RX on PAD1
#else
  #define UART_SERCOM        0           // pins must be set by the application
  #define UART_TXPO          1
  #define UART_RXPO          3
#endif

#if (UART_MAP == 6) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (UART_MAP == 7) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (UART_MAP == 8) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif
#if (UART_MAP == 9) && !(defined(SAMD21G) || defined(SAMD21J))
  #error Pin mapping not available on this device
#endif

#if (UART_SERCOM > 3) && defined(SAMD21E)
  #error SERCOM4 and SERCOM5 are not available on SAMD21E
#endif

#if   UART_SERCOM == 0
  #define UART_SER           SERCOM0
  #define UART_GCLK_ID       SERCOM0_GCLK_ID_CORE
  #define UART_APBMASK       PM_APBCMASK_SERCOM0
  #define UART_IRQn          SERCOM0_IRQn
  #define UART_IRQHandler    SERCOM0_IRQHandler
  #define UART_DMAC_ID_TX    SERCOM0_DMAC_ID_TX
#elif UART_SERCOM == 1
  #define UART_SER           SERCOM1
  #define UART_GCLK_ID       SERCOM1_GCLK_ID_CORE
  #define UART_APBMASK       PM_APBCMASK_SERCOM1
  #define UART_IRQn          SERCOM1_IRQn
  #define UART_IRQHandler    SERCOM1_IRQHandler
  #define UART_DMAC_ID_TX    SERCOM1_DMAC_ID_TX
#elif UART_SERCOM == 2
  #define UART_SER           SERCOM2
  #define UART_GCLK_ID       SERCOM2_GCLK_ID_CORE
  #define UART_APBMASK       PM_APBCMASK_SERCOM2
  #define UART_IRQn          SERCOM2_IRQn
  #define UART_IRQHandler    SERCOM2_IRQHandler
  #define UART_DMAC_ID_TX    SERCOM2_DMAC_ID_TX
#elif UART_SERCOM == 3
  #define UART_SER           SERCOM3
  #define UART_GCLK_ID       SERCOM3_GCLK_ID_CORE
  #define UART_APBMASK       PM_APBCMASK_SERCOM3
  #define UART_IRQn          SERCOM3_IRQn
  #define UART_IRQHandler    SERCOM3_IRQHandler
  #define UART_DMAC_ID_TX    SERCOM3_DMAC_ID_TX
#elif UART_SERCOM == 4
  #define UART_SER           SERCOM4
  #define UART_GCLK_ID       SERCOM4_GCLK_ID_CORE
  #define UART_APBMASK       PM_APBCMASK_SERCOM4
  #define UART_IRQn          SERCOM4_IRQn
  #define UART_IRQHandler    SERCOM4_IRQHandler
  #define UART_DMAC_ID_TX    SERCOM4_DMAC_ID_TX
#elif UART_SERCOM == 5
  #define UART_SER           SERCOM5
  #define UART_GCLK_ID       SERCOM5_GCLK_ID_CORE
  #define UART_APBMASK       PM_APBCMASK_SERCOM5
  #define UART_IRQn          SERCOM5_IRQn
  #define UART_IRQHandler    SERCOM5_IRQHandler
  #define UART_DMAC_ID_TX    SERCOM5_DMAC_ID_TX
#endif

// UART macros
#define UART_ready()          (UART_SER->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE)
#define UART_available()      (UART_SER->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_RXC)
#define UART_completed()      (UART_SER->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC)

#define UART_sync()           while(UART_SER->USART.SYNCBUSY.reg)
#define UART_enable()         (UART_SER->USART.CTRLA.reg |=  SERCOM_USART_CTRLA_ENABLE, UART_wait())
#define UART_disable()        (UART_SER->USART.CTRLA.reg &= ~SERCOM_USART_CTRLA_ENABLE, UART_wait())
#define UART_TX_enable()      (UART_SER->USART.CTRLB.reg |=  SERCOM_USART_CTRLB_TXEN,   UART_wait())
#define UART_TX_disable()     (UART_SER->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_TXEN,   UART_wait())
#define UART_RX_enable()      (UART_SER->USART.CTRLB.reg |=  SERCOM_USART_CTRLB_RXEN,   UART_wait())
#define UART_RX_disable()     (UART_SER->USART.CTRLB.reg &= ~SERCOM_USART_CTRLB_RXEN,   UART_wait())
static inline void UART_wait(void) { UART_sync(); }
#define UART_LP_enable()      UART_setCTRLA(SERCOM_USART_CTRLA_RUNSTDBY, SERCOM_USART_CTRLA_RUNSTDBY)
#define UART_LP_disable()     UART_setCTRLA(SERCOM_USART_CTRLA_RUNSTDBY, 0)

#define UART_setDataBits(n)   UART_setCTRLB(SERCOM_USART_CTRLB_CHSIZE_Msk, \
                              SERCOM_USART_CTRLB_CHSIZE(((n) == 9) ? 1 : ((n) & 7)))
#define UART_setStopBits(n)   UART_setCTRLB(SERCOM_USART_CTRLB_SBMODE, \
                              ((n) == 2) ? SERCOM_USART_CTRLB_SBMODE : 0)
#define UART_setNoParity()    UART_setCTRLA(SERCOM_USART_CTRLA_FORM_Msk, 0)
#define UART_setEvenParity()  (UART_setCTRLB(SERCOM_USART_CTRLB_PMODE, 0), \
                               UART_setCTRLA(SERCOM_USART_CTRLA_FORM_Msk, SERCOM_USART_CTRLA_FORM(1)))
#define UART_setOddParity()   (UART_setCTRLB(SERCOM_USART_CTRLB_PMODE, SERCOM_USART_CTRLB_PMODE), \
                               UART_setCTRLA(SERCOM_USART_CTRLA_FORM_Msk, SERCOM_USART_CTRLA_FORM(1)))

// UART functions
void UART_init(void);                     // init UART with default BAUD rate
char UART_read(void);                     // read character via UART
void UART_write(const char c);            // send character via UART
void UART_setBAUD(uint32_t baud);         // set BAUD rate
void UART_setCTRLA(uint32_t mask, uint32_t val);  // change CTRLA (USART disabled)
void UART_setCTRLB(uint32_t mask, uint32_t val);  // change CTRLB (USART disabled)

// Additional print functions (if activated, see above)
#if UART_PRINT == 1
#include "print.h"
#define UART_printD(n)        printD(UART_write, n)   // print decimal as string
#define UART_printW(n)        printW(UART_write, n)   // print word as string
#define UART_printH(n)        printH(UART_write, n)   // print half-word as string
#define UART_printB(n)        printB(UART_write, n)   // print byte as string
#define UART_printS(s)        printS(UART_write, s)   // print string
#define UART_println(s)       println(UART_write, s)  // print string with newline
#define UART_print            UART_printS             // alias
#define UART_newline()        UART_write('\n')        // send newline
#define UART_printf(f, ...)   printF(UART_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif
