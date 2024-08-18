// ===================================================================================
// Basic LPUART1 Functions for STM32G0xx  (no buffer, no interrupt, no DMA)   * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// LPUART_init()            Init UART with 8N1 and default BAUD rate (115200)
// LPUART_setBaud(n)        Set BAUD rate
// LPUART_setDataBits(n)    Set number of data bits (n = 7, 8, 9)
// LPUART_setStopBits(n)    Set number of stop bits (n = 1, 2)
// LPUART_setNoParity()     Set no parity bit
// LPUART_setOddParity()    Set parity bit, odd
// LPUART_setEvenParity()   Set parity bit, even
//
// LPUART_ready()           Check if UART is ready to write
// LPUART_available()       Check if there is something to read
// LPUART_completed()       Check if transmission is completed
//
// LPUART_read()            Read character via UART
// LPUART_write(c)          Send character via UART
//
// LPUART_enable()          Enable LPUART
// LPUART_disable()         Disable LPUART
// LPUART_TX_enable()       Enable transmitter
// LPUART_TX_disable()      Disable transmitter
// LPUART_RX_enable()       Enable receiver
// LPUART_RX_disable()      Disable receiver
// LPUART_LP_enable()       Enable wake from low-power
// LPUART_LP_disable()      Disable wake from low-power
// LPUART_FIFO_enable()     Enable FIFO mode
// LPUART_FIFO_disable()    Disable FIFO mode
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// LPUART_printf(f, ...)    printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// LPUART_printD(n)         Print decimal value
// LPUART_printW(n)         Print 32-bit hex word value
// LPUART_printH(n)         Print 16-bit hex half-word value
// LPUART_printB(n)         Print  8-bit hex byte value
// LPUART_printS(s)         Print string
// LPUART_print(s)          Print string (alias)
// LPUART_println(s)        Print string with newline
// LPUART_newline()         Send newline
//
// LPUART1 pin mapping (set below in UART parameters):
// ---------------------------------------------------
// LPUART_MAP   0     1     2
// TX-pin      PA2   PB11  No mapping
// RX-pin      PA3   PB10  No mapping
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32g0xx.h"

// UART parameters
#define LPUART_BAUD             115200    // default UART baud rate
#define LPUART_MAP              0         // UART pin mapping (see above)
#define LPUART_FIFO             1         // 1: use 8-byte FIFO
#define LPUART_LSE              0         // 1: use external 32.768kHz clock (max 9600 BAUD)
#define LPUART_PRINT            0         // 1: include print functions (needs print.h)

// UART macros
#define LPUART_ready()          (LPUART1->ISR & USART_ISR_TXE_TXFNF)  // ready to write
#define LPUART_available()      (LPUART1->ISR & USART_ISR_RXNE_RXFNE) // ready to read
#define LPUART_completed()      (LPUART1->ISR & USART_ISR_TC)         // transmission completed

#define LPUART_enable()         LPUART1->CR1 |= USART_CR1_UE          // enable LPUART
#define LPUART_disable()        LPUART1->CR1 &= ~USART_CR1_UE         // disable LPUART
#define LPUART_TX_enable()      LPUART1->CR1 |= USART_CR1_TE          // enable transmitter
#define LPUART_TX_disable()     LPUART1->CR1 &= ~USART_CR1_TE         // disable transmitter
#define LPUART_RX_enable()      LPUART1->CR1 |= USART_CR1_RE          // enable receiver
#define LPUART_RX_disable()     LPUART1->CR1 &= ~USART_CR1_RE         // disable receiver
#define LPUART_LP_enable()      LPUART1->CR1 |= USART_CR1_UESM        // enable wake from low-power
#define LPUART_LP_disable()     LPUART1->CR1 &= ~USART_CR1_UESM       // disable wake from low-power
#define LPUART_FIFO_enable()    LPUART1->CR1 |= USART_CR1_FIFOEN      // enable FIFO mode
#define LPUART_FIFO_disable()   LPUART1->CR1 |= ~USART_CR1_FIFOEN     // disable FIFO mode

#define LPUART_setDataBits(n)   {LPUART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); \
                                (n==9 ? (LPUART1->CR1 |= USART_CR1_M0) :         \
                                (n==7 ? (LPUART1->CR1 |= USART_CR1_M1) : (0)));  }
#define LPUART_setStopBits(n)   (n==2 ? (LPUART1->CR2 |= ((uint32_t)1<<13) : (LPUART1->CR2 &= ~((uint32_t)1<<13)))
#define LPUART_setEvenParity()  {LPUART1->CR1 |= USART_CR1_PCE; LPUART1->CR1 &= ~USART_CR1_PS;}
#define LPUART_setOddParity()   {LPUART1->CR1 |= USART_CR1_PCE; LPUART1->CR1 |=  USART_CR1_PS;}
#define LPUART_setNoParity()    LPUART1->CR1 &= ~USART_CR1_PCE

// Set BAUD rate
#if LPUART_LSE > 0
  #define LPUART_setBAUD(n)     LPUART1->BRR = (((uint64_t)512 * LSE_VALUE / (n)) + 1) / 2
#else
  #define LPUART_setBAUD(n)     LPUART1->BRR = (((uint64_t)512 * F_CPU / (n)) + 1) / 2
#endif

// UART functions
void LPUART_init(void);                   // init UART with default BAUD rate
char LPUART_read(void);                   // read character via UART
void LPUART_write(const char c);          // send character via UART

// Additional print functions (if activated, see above)
#if LPUART_PRINT == 1
#include "print.h"
#define LPUART_printD(n)        printD(LPUART_write, n)   // print decimal as string
#define LPUART_printW(n)        printW(LPUART_write, n)   // print word as string
#define LPUART_printH(n)        printH(LPUART_write, n)   // print half-word as string
#define LPUART_printB(n)        printB(LPUART_write, n)   // print byte as string
#define LPUART_printS(s)        printS(LPUART_write, s)   // print string
#define LPUART_println(s)       println(LPUART_write, s)  // print string with newline
#define LPUART_print            LPUART_printS             // alias
#define LPUART_newline()        LPUART_write('\n')        // send newline
#define LPUART_printf(f, ...)   printF(LPUART_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
};
#endif
