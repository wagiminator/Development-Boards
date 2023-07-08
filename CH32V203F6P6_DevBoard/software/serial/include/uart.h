// ===================================================================================
// Basic UART Functions for CH32V203  (no buffer, no interrupt, no DMA)       * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UARTx_init()             Init UART with default BAUD rate (115200)
// UARTx_setBaud(n)         Set BAUD rate
//
// UARTx_ready()            Check if UART is ready to write
// UARTx_available()        Check if there is something to read
//
// UARTx_read()             Read character via UART
// UARTx_write(c)           Send character via UART
// UARTx_print(s)           Send string via UART
// UARTx_println(s)         Send string with newline
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

#include "ch32v203.h"

// UART parameters
#define UART1_BAUD        115200          // default UART1 baud rate
#define UART1_REMAP       0               // UART1 pin remapping (see above)

#define UART2_BAUD        115200          // default UART2 baud rate
#define UART2_REMAP       0               // UART2 pin remapping (see above)

// UART macros
#define UART1_ready()     (USART1->STATR & USART_STATR_TC)    // ready to write
#define UART1_available() (USART1->STATR & USART_STATR_RXNE)  // ready to read
#define UART1_setBAUD(n)  USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

#define UART2_ready()     (USART2->STATR & USART_STATR_TC)    // ready to write
#define UART2_available() (USART2->STATR & USART_STATR_RXNE)  // ready to read
#define UART2_setBAUD(n)  USART2->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

// UART functions
void UART1_init(void);                    // init UART with default BAUD rate
void UART1_write(const char c);           // send character via UART
void UART1_print(const char* str);        // send string via UART
void UART1_println(const char* str);      // send string with newline via UART
char UART1_read(void);                    // read character via UART

void UART2_init(void);                    // init UART with default BAUD rate
void UART2_write(const char c);           // send character via UART
void UART2_print(const char* str);        // send string via UART
void UART2_println(const char* str);      // send string with newline via UART
char UART2_read(void);                    // read character via UART

#define UART1_newline() UART1_write('\n') // send newline
#define UART2_newline() UART2_write('\n') // send newline

#ifdef __cplusplus
};
#endif
