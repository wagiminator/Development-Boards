// ===================================================================================
// USART with RX Interrupt and Ring Buffer for megaAVR                        * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART_setBaud(n)          Set BAUD rate (normal speed mode)
// UART_setDataBits(n)      Set number of data bits (n = 5, 6, 7, 8)
// UART_setStopBits(n)      Set number of stop bits (n = 1, 2)
// UART_setNoParity()       Set no parity bit
// UART_setOddParity()      Set parity bit, odd
// UART_setEvenParity()     Set parity bit, even
//
// UART_ready()             Check if UART is ready to write
// UART_available()         Check if there is something to read
// UART_completed()         Check if transmission is completed
//
// UART_read()              Read character via UART
// UART_write(c)            Send character via UART
//
// UART_TX_enable()         Enable transmitter
// UART_TX_disable()        Disable transmitter
// UART_RX_enable()         Enable receiver
// UART_RX_disable()        Disable receiver
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// UART_printf(f, ...)      printf (supports %s, %c, %d, %u, %x, %b, %02d, %%, %ld)
// UART_printD(n)           Print decimal value
// UART_printW(n)           Print 32-bit hex word value
// UART_printH(n)           Print 16-bit hex half-word value
// UART_printB(n)           Print  8-bit hex byte value
// UART_printS(s)           Print string
// UART_print(s)            Print string (alias)
// UART_println(s)          Print string with newline
// UART_newline()           Send newline
//
// USART pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART_NUM   0          1          2          3 (40/48-pin only)
// UART_MAP   0    1     0    1     0    1     0    1
// TX-pin    PA0  PA4   PC0  PC4   PF0  PF4   PB0  PB4
// RX-pin    PA1  PA5   PC1  PC5   PF1  PF5   PB1  PB5
// UART_MAP 2: no automatic pin mapping
// PC4/PC5 and PB0..PB5 only on 40/48-pin devices, PF4/PF5 not on 28-pin devices
//
// Notes:
// ------
// - The double-speed mode (CLK2X) is selected automatically if necessary for the
//   desired BAUD rate at low F_CPU.
// - UART_OSCCOMP compensates the frequency error of the internal oscillator using
//   the factory calibration values in the signature row (3V or 5V values).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define UART_NUM              0           // USART number (0, 1, 2, 3)
#define UART_BAUD             115200      // default UART baud rate
#define UART_MAP              0           // UART pin mapping (see above)
#define UART_OSCCOMP          0           // oscillator error compensation (0: off, 3: 3V, 5: 5V)
#define UART_PRINT            0           // 1: include print functions (needs print.h)
#define UART_RX_BUF_SIZE      64          // RX buffer size (power of 2: 4, 8, ..., 128)

// Select USART peripheral (internal)
#define UART_CAT(a, b)        a ## b
#define UART_XCAT(a, b)       UART_CAT(a, b)
#define UART_USART            UART_XCAT(USART, UART_NUM)
#if UART_NUM == 3 && !defined(USART3)
  #error USART3 is only available on 40/48-pin devices
#endif

// UART macros
#define UART_available()      (UART_rptr != UART_wptr)            // something in buffer?
#define UART_ready()          (UART_USART.STATUS & USART_DREIF_bm)    // ready to write
#define UART_completed()      (UART_USART.STATUS & USART_TXCIF_bm)    // transmission completed

#define UART_TX_enable()      UART_USART.CTRLB |=  USART_TXEN_bm      // enable transmitter
#define UART_TX_disable()     UART_USART.CTRLB &= ~USART_TXEN_bm      // disable transmitter
#define UART_RX_enable()      UART_USART.CTRLB |=  USART_RXEN_bm      // enable receiver
#define UART_RX_disable()     UART_USART.CTRLB &= ~USART_RXEN_bm      // disable receiver

#define UART_setBaud(n)       UART_USART.BAUD = (((F_CPU * 8) / (n)) + 1) / 2
#define UART_setBAUD          UART_setBaud                        // alias
#define UART_setDataBits(n)   UART_USART.CTRLC = (UART_USART.CTRLC & 0xf8) | (((n) - 5) & 3)
#define UART_setStopBits(n)   ((n) == 2 ? (UART_USART.CTRLC |= USART_SBMODE_bm) : (UART_USART.CTRLC &= ~USART_SBMODE_bm))
#define UART_setEvenParity()  UART_USART.CTRLC = (UART_USART.CTRLC & ~USART_PMODE_gm) | USART_PMODE_EVEN_gc
#define UART_setOddParity()   UART_USART.CTRLC = (UART_USART.CTRLC & ~USART_PMODE_gm) | USART_PMODE_ODD_gc
#define UART_setNoParity()    UART_USART.CTRLC &= ~USART_PMODE_gm

// UART variables
extern volatile uint8_t UART_rptr;    // UART RX buffer read pointer
extern volatile uint8_t UART_wptr;    // UART RX buffer write pointer

// UART functions
void UART_init(void);                 // init UART with default BAUD rate
char UART_read(void);                 // read character via UART
void UART_write(const char c);        // send character via UART

// Additional print functions (if activated, see above)
#if UART_PRINT == 1
#include "print.h"
#define UART_printD(n)        printD(UART_write, n)   // print decimal as string
#define UART_printW(n)        printW(UART_write, n)   // print word as string
#define UART_printH(n)        printH(UART_write, n)   // print half-word as string
#define UART_printB(n)        printB(UART_write, n)   // print byte as string
#define UART_printS(s)        printS(UART_write, s)   // print string
#define UART_println(s)       println(UART_write, s)  // print string with newline
#define UART_print            UART_printS             // alias
#define UART_newline()        UART_write('\n')        // send newline
#define UART_printf(f, ...)   printF(UART_write, f, ##__VA_ARGS__)
#endif

#ifdef __cplusplus
}
#endif
