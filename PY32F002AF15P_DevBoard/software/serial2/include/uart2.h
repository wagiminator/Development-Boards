// ===================================================================================
// Basic USART2 Functions for PY32F0xx  (no buffer, no interrupt, no DMA)     * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART2_init()             Init USART2 with default BAUD rate (115200)
// UART2_setBaud(n)         Set BAUD rate
//
// UART2_ready()            Check if UART is ready to write
// UART2_available()        Check if there is something to read
//
// UART2_read()             Read character via UART
// UART2_write(c)           Send character via UART
// UART2_print(s)           Send string via UART
// UART2_println(s)         Send string with newline
// UART2_printS(s)          Send string via UART (alias)
// UART2_printD(n)          Send decimal value as string
// UART2_printW(n)          Send 32-bit hex word value as string
// UART2_printH(n)          Send 16-bit hex half-word value as string
// UART2_printB(n)          Send  8-bit hex byte value as string
// UART2_newline()          Send newline
//
// USART2 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART2_MAP  TX-pin  RX-pin
//       0     PA0     PA1
//       1     PA2     PA3
//       2     PA4     PA5
//       3     PA7     PA8
//       4     PA9     PA10
//       5     PA14    PA15
//       6     PB6     PB7
//       7     PF1     PF0
//       8     PF3     PF2
//       9     no pin mapping
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "py32f0xx.h"

// UART parameters
#define UART2_BAUD         115200         // default UART baud rate
#define UART2_MAP          1              // UART pin mapping (see above)

// UART macros
#define UART2_ready()     (USART2->SR & USART_SR_TXE)         // ready to write
#define UART2_available() (USART2->SR & USART_SR_RXNE)        // ready to read
#define UART2_setBAUD(n)  USART2->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

// UART functions
void UART2_init(void);                    // init UART with default BAUD rate
void UART2_write(const char c);           // send character via UART
void UART2_print(const char* str);        // send string via UART
void UART2_println(const char* str);      // send string with newline via UART
void UART2_printD(uint32_t value);        // send decimal value as string
void UART2_printW(uint32_t value);        // send 32-bit hex word value as string
void UART2_printH(uint16_t value);        // send 16-bit hex half-word value as string
void UART2_printB(uint8_t value);         // send  8-bit hex byte value as string
char UART2_read(void);                    // read character via UART
#define UART2_newline() UART2_write('\n') // send newline
#define UART2_printS UART2_print          // alias

#ifdef __cplusplus
};
#endif
