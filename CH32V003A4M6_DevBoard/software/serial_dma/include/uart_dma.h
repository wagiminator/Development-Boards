// ===================================================================================
// UART with DMA RX Buffer for CH32V003                                       * v1.0 *
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
// UART_printS(s)           Send string via UART
// UART_printD(n)           Send decimal value as string
// UART_printL(n)           Send hex long value as string
// UART_printW(n)           Send hex word value as string
// UART_printB(n)           Send hex byte value as string
// UART_newline()           Send newline
//
// UART remap settings (set below in UART parameters):
// ---------------------------------------------------
// UART_REMAP   TX-pin  RX-pin
//         0     PD5     PD6
//         1     PD0     PD1
//         2     PD6     PD5
//         3     PC0     PC1
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
#define UART_RX_BUF_SIZE  64              // UART RX buffer size

// UART macros
#define UART_ready()      (USART1->STATR & USART_STATR_TC)    // ready to write
#define UART_setBAUD(n)   USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

// UART functions
void UART_init(void);                     // init UART with default BAUD rate
void UART_write(const char c);            // send character via UART
void UART_print(const char* str);         // send string via UART
void UART_println(const char* str);       // send string with newline via UART
void UART_printD(uint32_t value);         // send decimal value as string
void UART_printL(uint32_t value);         // send hex long value as string
void UART_printW(uint16_t value);         // send hex word value as string
void UART_printB(uint8_t value);          // send hex byte value as string
char UART_read(void);                     // read character via UART
#define UART_newline() UART_write('\n')   // send newline
#define UART_printS UART_print            // alias
uint8_t UART_available(void);             // check if there is something to read

#ifdef __cplusplus
};
#endif
