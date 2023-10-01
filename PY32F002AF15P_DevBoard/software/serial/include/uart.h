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
// UART_printS(s)           Send string via UART (alias)
// UART_printD(n)           Send decimal value as string
// UART_printW(n)           Send 32-bit hex word value as string
// UART_printH(n)           Send 16-bit hex half-word value as string
// UART_printB(n)           Send  8-bit hex byte value as string
// UART_newline()           Send newline
//
// USART1 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART_MAP   TX-pin  RX-pin
//       0     PA2     PA3
//       1     PA7     PA8
//       2     PA9     PA10
//       3     PA14    PA13
//       4     PA14    PA15
//       5     PB6     PB7
//       6     PF1     PF0
//       7     no pin mapping
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "py32f0xx.h"

// UART parameters
#define UART_BAUD         115200          // default UART baud rate
#define UART_MAP          0               // UART pin mapping (see above)

// UART macros
#define UART_ready()      (USART1->SR & USART_SR_TXE)         // ready to write
#define UART_available()  (USART1->SR & USART_SR_RXNE)        // ready to read
#define UART_setBAUD(n)   USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

// UART functions
void UART_init(void);                     // init UART with default BAUD rate
void UART_write(const char c);            // send character via UART
void UART_print(const char* str);         // send string via UART
void UART_println(const char* str);       // send string with newline via UART
void UART_printD(uint32_t value);         // send decimal value as string
void UART_printW(uint32_t value);         // send 32-bit hex word value as string
void UART_printH(uint16_t value);         // send 16-bit hex half-word value as string
void UART_printB(uint8_t value);          // send  8-bit hex byte value as string
char UART_read(void);                     // read character via UART
#define UART_newline() UART_write('\n')   // send newline
#define UART_printS UART_print            // alias

#ifdef __cplusplus
};
#endif
