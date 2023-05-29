// ===================================================================================
// Basic UART Functions for CH32V003                                          * v1.0 *
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
#define UART_ready()      (USART1->STATR & USART_STATR_TC)    // ready to write
#define UART_available()  (USART1->STATR & USART_STATR_RXNE)  // ready to read
#define UART_setBAUD(n)   USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

// UART functions
void UART_init(void);                     // init UART with default BAUD rate
void UART_write(const char c);            // send character via UART
void UART_print(const char* str);         // send string via UART
void UART_println(const char* str);       // send string with newline via UART
void UART_printD(uint32_t value);         // send decimal value as string
void UART_printH(uint32_t value);         // send hex long value as string
void UART_printW(uint16_t value);         // send hex word value as string
void UART_printB(uint8_t value);          // send hex byte value as string
char UART_read(void);                     // read character via UART

// UART_REMAP   TX-pin  RX-pin
//         0     PD5     PD6
//         1     PD0     PD1
//         2     PD6     PD5
//         3     PC0     PC1

#ifdef __cplusplus
};
#endif
