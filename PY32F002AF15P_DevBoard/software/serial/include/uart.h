// ===================================================================================
// Basic USART1 Functions for PY32F0xx  (no buffer, no interrupt, no DMA)     * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with default BAUD rate (115200)
// UART_setBaud(n)          Set BAUD rate
//
// UART_ready()             Check if UART is ready to write
// UART_available()         Check if there is something to read
//
// UART_read()              Read character via UART
// UART_write(c)            Send character via UART
// UART_print(s)            Send string via UART
// UART_println(s)          Send string with newline
// UART_newline()           Send newline
//
// UART remap settings (set below in UART parameters):
// ---------------------------------------------------
// UART_REMAP   TX-pin  RX-pin
//         0     PA2     PA3
//         1     PA7     PA8
//         2     PA9     PA10
//         3     PA14    PA15
//         4     PB6     PB7
//         5     PF1     PF0
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "py32f0xx.h"

// UART parameters
#define UART_BAUD         115200          // default UART baud rate
#define UART_REMAP        0               // UART pin remapping (see above)

// UART macros
#define UART_ready()      (USART1->SR & USART_SR_TXE)         // ready to write
#define UART_available()  (USART1->SR & USART_SR_RXNE)        // ready to read
#define UART_setBAUD(n)   USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

// UART functions
void UART_init(void);                     // init UART with default BAUD rate
void UART_write(const char c);            // send character via UART
void UART_print(const char* str);         // send string via UART
void UART_println(const char* str);       // send string with newline via UART
char UART_read(void);                     // read character via UART
#define UART_newline() UART_write('\n')   // send newline

#ifdef __cplusplus
};
#endif
