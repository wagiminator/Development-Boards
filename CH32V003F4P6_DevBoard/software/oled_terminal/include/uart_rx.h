// ===================================================================================
// Basic UART Functions (RX only) for CH32V003                                * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with default BAUD rate (115200)
// UART_setBaud(n)          Set BAUD rate
// UART_available()         Check if there is something to read
// UART_read()              Read character via UART
//
// UART remap settings (set below in UART parameters):
// ---------------------------------------------------
// UART_REMAP   RX-pin
//         0     PD6
//         1     PD1
//         2     PD5
//         3     PC1
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v003.h"

// UART parameters
#define UART_BAUD         115200          // default UART baud rate
#define UART_REMAP        0               // UART pin remapping (see above)

// UART macros
#define UART_available()  (USART1->STATR & USART_STATR_RXNE)  // ready to read
#define UART_setBAUD(n)   USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

// UART functions
void UART_init(void);                     // init UART with default BAUD rate
char UART_read(void);                     // read character via UART

#ifdef __cplusplus
};
#endif
