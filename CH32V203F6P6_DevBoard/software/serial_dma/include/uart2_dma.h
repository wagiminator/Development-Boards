// ===================================================================================
// UART2 with DMA RX Buffer for CH32V203                                      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART2_init()             Init UART with default BAUD rate (115200)
// UART2_setBaud(n)         Set BAUD rate
//
// UART2_ready()            Check if UART is ready to write
// UART2_available()        Check if there is something to read
//
// UART2_read()             Read character via UART
// UART2_write(c)           Send character via UART
// UART2_print(s)           Send string via UART
// UART2_println(s)         Send string with newline
// UART2_printS(s)          Send string via UART
// UART2_printD(n)          Send decimal value as string
// UART2_printL(n)          Send hex long value as string
// UART2_printW(n)          Send hex word value as string
// UART2_printB(n)          Send hex byte value as string
// UART2_newline()          Send newline
//
// UART2 remap settings (set below in UART parameters):
// ----------------------------------------------------
// UART2_REMAP  TX-pin  RX-pin
//         0     PA2     PA3
//         1     PD5     PD6 *
// * not available on most CH32V203
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v203.h"

// UART parameters
#define UART2_BAUD         115200         // default UART baud rate
#define UART2_REMAP        0              // UART pin remapping (see above)
#define UART2_RX_BUF_SIZE  64             // UART RX buffer size

// UART macros
#define UART2_ready()     (USART2->STATR & USART_STATR_TXE)   // ready to write
#define UART2_setBAUD(n)  USART2->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate

// UART functions
void UART2_init(void);                    // init UART with default BAUD rate
void UART2_write(const char c);           // send character via UART
void UART2_print(const char* str);        // send string via UART
void UART2_println(const char* str);      // send string with newline via UART
void UART2_printD(uint32_t value);        // send decimal value as string
void UART2_printL(uint32_t value);        // send hex long value as string
void UART2_printW(uint16_t value);        // send hex word value as string
void UART2_printB(uint8_t value);         // send hex byte value as string
char UART2_read(void);                    // read character via UART
#define UART2_newline() UART2_write('\n') // send newline
#define UART2_printS UART2_print          // alias
uint8_t UART2_available(void);            // check if there is something to read

#ifdef __cplusplus
};
#endif
