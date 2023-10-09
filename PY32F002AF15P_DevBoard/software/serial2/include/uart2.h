// ===================================================================================
// Basic USART2 Functions for PY32F0xx  (no buffer, no interrupt, no DMA)     * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART2_init()             Init USART2 with default BAUD rate (115200)
// UART2_setBaud(n)         Set BAUD rate
// UART2_setDataBits(n)     Set number of data bits (n = 8, 9)
// UART2_setStopBits(n)     Set number of stop bits (n = 1, 2)
// UART2_setNoParity()      Set no parity bit
// UART2_setOddParity()     Set parity bit, odd
// UART2_setEvenParity()    Set parity bit, even
//
// UART2_ready()            Check if UART is ready to write
// UART2_available()        Check if there is something to read
// UART2_completed()        Check if transmission is completed
//
// UART2_read()             Read character via UART
// UART2_write(c)           Send character via UART
//
// UART2_enable()           Enable USART
// UART2_disable()          Disable USART
// UART2_TX_enable()        Enable transmitter
// UART2_TX_disable()       Disable transmitter
// UART2_RX_enable()        Enable receiver
// UART2_RX_disable()       Disable receiver
//
// If print functions are activated (see below, print.h must be included):
// UART2_printD(n)          Print decimal value
// UART2_printW(n)          Print 32-bit hex word value
// UART2_printH(n)          Print 16-bit hex half-word value
// UART2_printB(n)          Print  8-bit hex byte value
// UART2_printS(s)          Print string
// UART2_print(s)           Print string (alias)
// UART2_println(s)         Print string with newline
// UART2_newline()          Send newline
//
// USART2 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART2_MAP    0     1     2     3     4     5     6     7     8     9
// TX-pin      PA0   PA2   PA4   PA7   PA9   PA14  PB6   PF1   PF3   No mapping
// RX-pin      PA1   PA3   PA5   PA8   PA10  PA15  PB7   PF0   PF2   No mapping
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "py32f0xx.h"

// UART parameters
#define UART2_BAUD             115200    // default UART baud rate
#define UART2_MAP              1         // UART pin mapping (see above)
#define UART2_PRINT            0         // 1 = include print functions (needs print.h)

// UART macros
#define UART2_ready()          (USART2->SR & USART_SR_TXE)   // ready to write
#define UART2_available()      (USART2->SR & USART_SR_RXNE)  // ready to read
#define UART2_completed()      (USART2->SR & USART_SR_TC)    // transmission completed

#define UART2_enable()         USART2->CR1 |= USART_CR1_UE   // enable USART
#define UART2_disable()        USART2->CR1 &= ~USART_CR1_UE  // disable USART
#define UART2_TX_enable()      USART2->CR1 |= USART_CR1_TE   // enable transmitter
#define UART2_TX_disable()     USART2->CR1 &= ~USART_CR1_TE  // disable transmitter
#define UART2_RX_enable()      USART2->CR1 |= USART_CR1_RE   // enable receiver
#define UART2_RX_disable()     USART2->CR1 &= ~USART_CR1_RE  // disable receiver

#define UART2_setBAUD(n)       USART2->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate
#define UART2_setDataBits(n)   (n==9 ? (USART2->CR1 |= USART_CR1_M) : (USART2->CR1 &= ~USART_CR1_M))
#define UART2_setStopBits(n)   (n==2 ? (USART2->CR2 |= USART_CR2_STOP) : (USART2->CR2 &= ~USART_CR2_STOP))
#define UART2_setEvenParity()  {USART2->CR1 |= USART_CR1_PCE; USART2->CR1 &= ~USART_CR1_PS;}
#define UART2_setOddParity()   {USART2->CR1 |= USART_CR1_PCE; USART2->CR1 |=  USART_CR1_PS;}
#define UART2_setNoParity()    USART2->CR1 &= ~USART_CR1_PCE

// UART functions
void UART2_init(void);                    // init UART with default BAUD rate
char UART2_read(void);                    // read character via UART
void UART2_write(const char c);           // send character via UART

// Additional print functions (if activated, see above)
#if UART2_PRINT == 1
#include "print.h"
#define UART2_printD(n)   printD(n, UART2_write)  // print decimal as string
#define UART2_printW(n)   printW(n, UART2_write)  // print word as string
#define UART2_printH(n)   printH(n, UART2_write)  // print half-word as string
#define UART2_printB(n)   printB(n, UART2_write)  // print byte as string
#define UART2_printS(s)   printS(s, UART2_write)  // print string
#define UART2_println(s)  println(s, UART2_write) // print string with newline
#define UART2_print       UART2_printS            // alias
#define UART2_newline()   UART2_write('\n')       // send newline
#endif

#ifdef __cplusplus
};
#endif
