// ===================================================================================
// Basic UART Functions for CH32V003  (no buffer, no interrupt, no DMA)       * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART_setBaud(n)          Set BAUD rate
// UART_setDataBits(n)      Set number of data bits (n = 8, 9)
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
//
// If print functions are activated (see below, print.h must be included):
// UART_printD(n)           Print decimal value
// UART_printW(n)           Print 32-bit hex word value
// UART_printH(n)           Print 16-bit hex half-word value
// UART_printB(n)           Print  8-bit hex byte value
// UART_printS(s)           Print string
// UART_print(s)            Print string (alias)
// UART_println(s)          Print string with newline
// UART_newline()           Send newline
//
// USART1 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART_MAP   0     1     2     3
// TX-pin    PD5   PD0   PD6   PC0
// RX-pin    PD6   PD1   PD5   PC1
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v003.h"

// UART Parameters
#define UART_BAUD             115200          // default UART baud rate
#define UART_MAP              0               // UART pin mapping (see above)

// UART Macros
#define UART_ready()          (USART1->STATR & USART_STATR_TXE)   // ready to write
#define UART_available()      (USART1->STATR & USART_STATR_RXNE)  // ready to read
#define UART_completed()      (USART1->STATR & USART_STATR_TC)    // transmission completed

#define UART_enable()         USART1->CTLR1 |= USART_CTLR1_UE     // enable USART
#define UART_disable()        USART1->CTLR1 &= ~USART_CTLR1_UE    // disable USART
#define UART_TX_enable()      USART1->CTLR1 |= USART_CTLR1_TE     // enable transmitter
#define UART_TX_disable()     USART1->CTLR1 &= ~USART_CTLR1_TE    // disable transmitter
#define UART_RX_enable()      USART1->CTLR1 |= USART_CTLR1_RE     // enable receiver
#define UART_RX_disable()     USART1->CTLR1 &= ~USART_CTLR1_RE    // disable receiver

#define UART_setBAUD(n)       USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate
#define UART_setDataBits(n)   (n==9 ? (USART1->CTLR1 |= USART_CTLR1_M) : (USART1->CTLR1 &= ~USART_CTLR1_M))
#define UART_setStopBits(n)   (n==2 ? (USART1->CTLR2 |= ((uint32_t)1<<13) : (USART1->CTLR2 &= ~((uint32_t)1<<13)))
#define UART_setEvenParity()  {USART1->CTLR1 |= USART_CTLR1_PCE; USART1->CTLR1 &= ~USART_CTLR1_PS;}
#define UART_setOddParity()   {USART1->CTLR1 |= USART_CTLR1_PCE; USART1->CTLR1 |=  USART_CTLR1_PS;}
#define UART_setNoParity()    USART1->CTLR1 &= ~USART_CTLR1_PCE

// UART Functions
void UART_init(void);                             // init UART with default BAUD rate
char UART_read(void);                             // read character via UART
void UART_write(const char c);                    // send character via UART

// Additional print functions (if activated, see above)
#if UART_PRINT == 1
#include "print.h"
#define UART_printD(n)    printD(n, UART_write)   // print decimal as string
#define UART_printW(n)    printW(n, UART_write)   // print word as string
#define UART_printH(n)    printH(n, UART_write)   // print half-word as string
#define UART_printB(n)    printB(n, UART_write)   // print byte as string
#define UART_printS(s)    printS(s, UART_write)   // print string
#define UART_println(s)   println(s, UART_write)  // print string with newline
#define UART_print        UART_printS             // alias
#define UART_newline()    UART_write('\n')        // send newline
#endif

#ifdef __cplusplus
};
#endif
