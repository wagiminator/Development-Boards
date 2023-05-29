// ===================================================================================
// Basic UART Functions (RX only) for CH32V003                                * v1.0 *
// ===================================================================================

#pragma once
#include <stdint.h>
#include "ch32v003.h"

#ifdef __cplusplus
extern "C" {
#endif

// UART parameters
#define UART_BAUD         115200          // default UART baud rate
#define UART_REMAP        0               // UART pin remapping (see bottom)

// UART macros
#define UART_available()  (USART1->STATR & USART_STATR_RXNE)  // ready to read
#define UART_setBAUD(n)   USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

// UART functions
void UART_init(void);                     // init UART with default BAUD rate
char UART_read(void);                     // read character via UART

// UART_REMAP   TX-pin  RX-pin
//         0     PD5     PD6
//         1     PD0     PD1
//         2     PD6     PD5
//         3     PC0     PC1

#ifdef __cplusplus
};
#endif
