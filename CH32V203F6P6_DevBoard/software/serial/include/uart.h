// ===================================================================================
// Basic UART Functions for CH32V203  (no buffer, no interrupt, no DMA)       * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UARTx_init()             Init UART with default BAUD rate (115200)
// UARTx_setBAUD(n)         Set BAUD rate
// UARTx_setStopBits(n)     Set number of stop bits (n = 1, 2)
// UARTx_setNoParity()      Set no parity bit
// UARTx_setOddParity()     Set parity bit, odd
// UARTx_setEvenParity()    Set parity bit, even
//
// UARTx_ready()            Check if UART is ready to write
// UARTx_available()        Check if there is something to read
// UARTx_completed()        Check if transmission is completed
//
// UARTx_read()             Read character via UART
// UARTx_write(c)           Send character via UART
//
// UARTx_enable()           Enable UART
// UARTx_disable()          Disable UART
// UARTx_TX_enable()        Enable transmitter
// UARTx_TX_disable()       Disable transmitter
// UARTx_RX_enable()        Enable receiver
// UARTx_RX_disable()       Disable receiver
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// UARTx_printf(f, ...)     printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// UARTx_printD(n)          Print decimal value
// UARTx_printW(n)          Print 32-bit hex word value
// UARTx_printH(n)          Print 16-bit hex half-word value
// UARTx_printB(n)          Print  8-bit hex byte value
// UARTx_printS(s)          Print string
// UARTx_print(s)           Print string (alias)
// UARTx_println(s)         Print string with newline
// UARTx_newline()          Send newline
//
// UARTx remap settings (set below in UART parameters):
// ----------------------------------------------------
//               --- UART1 ---    --- UART2 ---
// UARTx_REMAP   TX-pin RX-pin    TX-pin RX-pin
//         0      PA9    PA10      PA2    PA3
//         1      PB6    PB7       PD5    PD6 *
//         2      PB15   PA8 *
//         3      PA6    PA7 *
// * not available on most CH32V203
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// UART Parameters
// ===================================================================================
#define UART_PRINT        0               // 1: include print functions (needs print.h)

#define UART1_BAUD        115200          // default UART1 baud rate
#define UART1_REMAP       0               // UART1 pin remapping (see above)

#define UART2_BAUD        115200          // default UART2 baud rate
#define UART2_REMAP       0               // UART2 pin remapping (see above)

// ===================================================================================
// UART Macros
// ===================================================================================
#define UART1_ready()         (USART1->STATR & USART_STATR_TXE)   // ready to write
#define UART1_available()     (USART1->STATR & USART_STATR_RXNE)  // ready to read
#define UART1_completed()     (USART1->STATR & USART_STATR_TC)    // transmission completed

#define UART1_enable()        USART1->CTLR1 |=  USART_CTLR1_UE    // enable USART
#define UART1_disable()       USART1->CTLR1 &= ~USART_CTLR1_UE    // disable USART
#define UART1_TX_enable()     USART1->CTLR1 |=  USART_CTLR1_TE    // enable transmitter
#define UART1_TX_disable()    USART1->CTLR1 &= ~USART_CTLR1_TE    // disable transmitter
#define UART1_RX_enable()     USART1->CTLR1 |=  USART_CTLR1_RE    // enable receiver
#define UART1_RX_disable()    USART1->CTLR1 &= ~USART_CTLR1_RE    // disable receiver

#define UART1_setBAUD(n)      USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate
#define UART1_setDataBits(n)  (n==9 ? (USART1->CTLR1 |= USART_CTLR1_M)      : (USART1->CTLR1 &= ~USART_CTLR1_M))
#define UART1_setStopBits(n)  (n==2 ? (USART1->CTLR2 |= USART_CTLR2_STOP_1) : (USART1->CTLR2 &= ~USART_CTLR2_STOP_1))
#define UART1_setEvenParity() {USART1->CTLR1 |= USART_CTLR1_PCE; USART1->CTLR1 &= ~USART_CTLR1_PS;}
#define UART1_setOddParity()  {USART1->CTLR1 |= USART_CTLR1_PCE; USART1->CTLR1 |=  USART_CTLR1_PS;}
#define UART1_setNoParity()   USART1->CTLR1 &= ~USART_CTLR1_PCE

#define UART2_ready()         (USART2->STATR & USART_STATR_TXE)   // ready to write
#define UART2_available()     (USART2->STATR & USART_STATR_RXNE)  // ready to read
#define UART2_completed()     (USART2->STATR & USART_STATR_TC)    // transmission completed

#define UART2_enable()        USART2->CTLR1 |=  USART_CTLR1_UE    // enable USART
#define UART2_disable()       USART2->CTLR1 &= ~USART_CTLR1_UE    // disable USART
#define UART2_TX_enable()     USART2->CTLR1 |=  USART_CTLR1_TE    // enable transmitter
#define UART2_TX_disable()    USART2->CTLR1 &= ~USART_CTLR1_TE    // disable transmitter
#define UART2_RX_enable()     USART2->CTLR1 |=  USART_CTLR1_RE    // enable receiver
#define UART2_RX_disable()    USART2->CTLR1 &= ~USART_CTLR1_RE    // disable receiver

#define UART2_setBAUD(n)      USART2->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate
#define UART2_setDataBits(n)  (n==9 ? (USART2->CTLR1 |= USART_CTLR1_M)      : (USART2->CTLR1 &= ~USART_CTLR1_M))
#define UART2_setStopBits(n)  (n==2 ? (USART2->CTLR2 |= USART_CTLR2_STOP_1) : (USART2->CTLR2 &= ~USART_CTLR2_STOP_1))
#define UART2_setEvenParity() {USART2->CTLR1 |= USART_CTLR1_PCE; USART2->CTLR1 &= ~USART_CTLR1_PS;}
#define UART2_setOddParity()  {USART2->CTLR1 |= USART_CTLR1_PCE; USART2->CTLR1 |=  USART_CTLR1_PS;}
#define UART2_setNoParity()   USART2->CTLR1 &= ~USART_CTLR1_PCE

// ===================================================================================
// UART Functions
// ===================================================================================
void UART1_init(void);                    // init UART with default BAUD rate
char UART1_read(void);                    // read character via UART
void UART1_write(const char c);           // send character via UART

void UART2_init(void);                    // init UART with default BAUD rate
char UART2_read(void);                    // read character via UART
void UART2_write(const char c);           // send character via UART

// ===================================================================================
// Additional Print Functions (if activated, see above)
// ===================================================================================
#if UART_PRINT == 1

#include "print.h"

#define UART1_printD(n)       printD(UART1_write, n)    // print decimal as string
#define UART1_printW(n)       printW(UART1_write, n)    // print word as string
#define UART1_printH(n)       printH(UART1_write, n)    // print half-word as string
#define UART1_printB(n)       printB(UART1_write, n)    // print byte as string
#define UART1_printS(s)       printS(UART1_write, s)    // print string
#define UART1_println(s)      println(UART1_write, s)   // print string with newline
#define UART1_print           UART1_printS              // alias
#define UART1_newline()       UART1_write('\n')         // send newline
#define UART1_printf(f, ...)  printF(UART1_write, f, ##__VA_ARGS__)

#define UART2_printD(n)       printD(UART2_write, n)    // print decimal as string
#define UART2_printW(n)       printW(UART2_write, n)    // print word as string
#define UART2_printH(n)       printH(UART2_write, n)    // print half-word as string
#define UART2_printB(n)       printB(UART2_write, n)    // print byte as string
#define UART2_printS(s)       printS(UART2_write, s)    // print string
#define UART2_println(s)      println(UART2_write, s)   // print string with newline
#define UART2_print           UART2_printS              // alias
#define UART2_newline()       UART2_write('\n')         // send newline
#define UART2_printf(f, ...)  printF(UART2_write, f, ##__VA_ARGS__)

#endif // UART_PRINT = 1

#ifdef __cplusplus
};
#endif
