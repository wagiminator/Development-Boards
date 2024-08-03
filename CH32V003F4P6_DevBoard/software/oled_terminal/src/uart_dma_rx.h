// ===================================================================================
// UART with DMA (RX only) for CH32V003                                       * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with default BAUD rate (115200)
// UART_available()         Check if there is something to read
// UART_read()              Read character via UART
//
// UART_setBaud(n)          Set BAUD rate
// UART_setStopBits(n)      Set number of stop bits (n = 1, 2)
// UART_setNoParity()       Set no parity bit
// UART_setOddParity()      Set parity bit, odd
// UART_setEvenParity()     Set parity bit, even
//
// UART_enable()            Enable USART
// UART_disable()           Disable USART
//
// USART1 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART_MAP   0     1     2     3
// RX-pin    PD6   PD1   PD5   PC1
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v003.h"

// UART Parameters
#define UART_BAUD             115200      // default UART baud rate
#define UART_MAP              0           // UART pin remapping (see above)
#define UART_RX_BUF_SIZE      64          // UART RX buffer size

// UART Functions
void UART_init(void);                     // init UART with default BAUD rate
char UART_read(void);                     // read character via UART
uint8_t UART_available(void);             // check if there is something to read

// UART Macros
#define UART_setBAUD(n)       USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate
#define UART_setStopBits(n)   (n==2 ? (USART1->CTLR2 |= ((uint32_t)1<<13) : (USART1->CTLR2 &= ~((uint32_t)1<<13)))
#define UART_setEvenParity()  {USART1->CTLR1 |= USART_CTLR1_PCE; USART1->CTLR1 &= ~USART_CTLR1_PS;}
#define UART_setOddParity()   {USART1->CTLR1 |= USART_CTLR1_PCE; USART1->CTLR1 |=  USART_CTLR1_PS;}
#define UART_setNoParity()    USART1->CTLR1 &= ~USART_CTLR1_PCE
#define UART_enable()         USART1->CTLR1 |= USART_CTLR1_UE     // enable USART
#define UART_disable()        USART1->CTLR1 &= ~USART_CTLR1_UE    // disable USART

#ifdef __cplusplus
};
#endif
