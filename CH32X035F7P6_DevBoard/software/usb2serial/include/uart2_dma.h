// ===================================================================================
// USART2 with DMA RX Buffer for CH32X035/X034/X033                           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART2_init()             Init UART with default BAUD rate (115200)
// UART2_setBAUD(n)         Set BAUD rate
// UART2_setStopBits(n)     Set number of stop bits (n = 1, 2)
// UART2_setNoParity()      Set no parity bit
// UART2_setOddParity()     Set parity bit, odd
// UART2_setEvenParity()    Set parity bit, even
//
// UART2_ready()            Check if UART is ready to write
// UART2_available()        Check if there is something to read
// UART2_completed()        Check if transmission is completed
//
// UART2_read()             Read character via UART
// UART2_write(c)           Send character via UART
//
// UART2_enable()           Enable UART
// UART2_disable()          Disable UART
// UART2_TX_enable()        Enable transmitter
// UART2_TX_disable()       Disable transmitter
// UART2_RX_enable()        Enable receiver
// UART2_RX_disable()       Disable receiver
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// UART2_printf(f, ...)     printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// UART2_printD(n)          Print decimal value
// UART2_printW(n)          Print 32-bit hex word value
// UART2_printH(n)          Print 16-bit hex half-word value
// UART2_printB(n)          Print  8-bit hex byte value
// UART2_printS(s)          Print string
// UART2_print(s)           Print string (alias)
// UART2_println(s)         Print string with newline
// UART2_newline()          Send newline
//
// UARTx remap settings (set below in UART parameters):
// ----------------------------------------------------
// UART2_REMAP    0     1     2     3     4
// TX-pin        PA2   PA20  PA15  PC0   PA15
// RX-pin        PA3   PA19  PA16  PC1   PA16
// CTS-pin       PA0   PA1   PA17  PC2   PA17  (*)
// RTS-pin       PA1   PA2   PA21  PC3   PC3   (*)
// CK-pin        PA4   PA23  PA22  PB20  PA22  (*)
// (*) not used
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// UART2 Parameters
// ===================================================================================
#define UART2_PRINT           0         // 1: include print functions (needs print.h)
#define UART2_REMAP           0         // UART2 pin remapping (see above)
#define UART2_BAUD            115200    // default UART2 baud rate
#define UART2_RX_BUF_SIZE     64        // UART RX buffer size

// ===================================================================================
// UART2 Macros
// ===================================================================================
#define UART2_ready()         (USART2->STATR & USART_STATR_TXE)   // ready to write
#define UART2_completed()     (USART2->STATR & USART_STATR_TC)    // transmission completed

#define UART2_enable()        USART2->CTLR1 |=  USART_CTLR1_UE    // enable USART
#define UART2_disable()       USART2->CTLR1 &= ~USART_CTLR1_UE    // disable USART
#define UART2_TX_enable()     USART2->CTLR1 |=  USART_CTLR1_TE    // enable transmitter
#define UART2_TX_disable()    USART2->CTLR1 &= ~USART_CTLR1_TE    // disable transmitter
#define UART2_RX_enable()     USART2->CTLR1 |=  USART_CTLR1_RE    // enable receiver
#define UART2_RX_disable()    USART2->CTLR1 &= ~USART_CTLR1_RE    // disable receiver

#define UART2_setBAUD(n)      USART2->BRR = ((2*F_CPU/(n))+1)/2;  // set BAUD rate
#define UART2_setDataBits(n)  (n==9 ? (USART2->CTLR1 |= USART_CTLR1_M)      : (USART2->CTLR1 &= ~USART_CTLR1_M))
#define UART2_setStopBits(n)  (n==2 ? (USART2->CTLR2 |= USART_CTLR2_STOP_1) : (USART2->CTLR2 &= ~USART_CTLR2_STOP_1))
#define UART2_setEvenParity() {USART2->CTLR1 |= USART_CTLR1_PCE; USART2->CTLR1 &= ~USART_CTLR1_PS;}
#define UART2_setOddParity()  {USART2->CTLR1 |= USART_CTLR1_PCE; USART2->CTLR1 |=  USART_CTLR1_PS;}
#define UART2_setNoParity()   USART2->CTLR1 &= ~USART_CTLR1_PCE

// ===================================================================================
// UART2 Functions
// ===================================================================================
void UART2_init(void);                    // init UART with default BAUD rate
char UART2_read(void);                    // read character via UART
void UART2_write(const char c);           // send character via UART
uint8_t UART2_available(void);            // check if there is something to read

// ===================================================================================
// Additional Print Functions (if activated, see above)
// ===================================================================================
#if UART2_PRINT == 1

#include "print.h"

#define UART2_printD(n)       printD(UART2_write, n)    // print decimal as string
#define UART2_printW(n)       printW(UART2_write, n)    // print word as string
#define UART2_printH(n)       printH(UART2_write, n)    // print half-word as string
#define UART2_printB(n)       printB(UART2_write, n)    // print byte as string
#define UART2_printS(s)       printS(UART2_write, s)    // print string
#define UART2_println(s)      println(UART2_write, s)   // print string with newline
#define UART2_print           UART2_printS              // alias
#define UART2_newline()       UART2_write('\n')         // send newline
#define UART2_printf(f, ...)  printF(UART2_write, f, ##__VA_ARGS__)

#endif // UART2_PRINT == 1

#ifdef __cplusplus
};
#endif
