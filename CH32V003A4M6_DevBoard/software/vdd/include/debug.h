// ===================================================================================
// Basic Serial Debug Functions for CH32V003                                  * v1.0 *
// ===================================================================================

#pragma once
#include <stdint.h>
#include <stdio.h>
#include "ch32v003.h"

#ifdef __cplusplus
extern "C" {
#endif

// DEBUG parameters
#define DEBUG_ENABLE      1               // enable serial DEBUG (0:no, 1:yes)
#define DEBUG_BAUD        115200          // default UART baud rate

// DEBUG macros
#define DEBUG_setBAUD(n)  USART1->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate
#define DEBUG_out         DEBUG_println   // default DEBUG function

// DEBUG functions
void DEBUG_init(void);                    // init UART with default BAUD rate
void DEBUG_write(const char c);           // send character via UART
void DEBUG_print(const char* str);        // send string via UART
void DEBUG_println(const char* str);      // send string with newline via UART
void DEBUG_printD(uint32_t value);        // send decimal value as string
void DEBUG_printH(uint32_t value);        // send hex long value as string
void DEBUG_printW(uint16_t value);        // send hex word value as string
void DEBUG_printB(uint8_t value);         // send hex byte value as string
int putchar(int c);                       // send character (for printf)
#define DEBUG_printS  DEBUG_print         // alias for print

#ifdef __cplusplus
};
#endif
