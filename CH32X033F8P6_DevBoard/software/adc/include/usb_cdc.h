// ===================================================================================
// Basic USB CDC Functions for CH32X035/X034/X033                             * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// CDC_init()               setup USB CDC
// CDC_read()               read single character from receive buffer
// CDC_write(c)             write single character to transmit buffer
// CDC_flush()              flush transmit buffer
// CDC_writeflush(c)        write & flush character
// CDC_newline()            newline and flush
//
// CDC_available()          check number of bytes in the receive buffer
// CDC_ready()              check if transmit buffer is ready to be written
//
// CDC_getDTR()             get DTR flag
// CDC_getRTS()             get RTS flag
// CDC_getBAUD()            get BAUD rate
// CDC_getParity()          get parity (0:none, 1:odd, 2:even, 3:mark, 4:space)
// CDC_getStopBits()        get number of stop bits (0:1bit, 1:1.5bits, 2:2bits)
// CDC_getDataBits()        get number of data bits (5, 6, 7, 8, or 16)
//
// If print functions are activated (see below, print.h must be included):
// -----------------------------------------------------------------------
// CDC_printf(f, ...)       printf (supports %s, %c, %d, %u, %x, %b, %02d, %%)
// CDC_printD(n)            print decimal value
// CDC_printW(n)            print 32-bit hex word value
// CDC_printH(n)            print 16-bit hex half-word value
// CDC_printB(n)            print  8-bit hex byte value
// CDC_printS(s)            print string
// CDC_print(s)             print string (alias)
// CDC_println(s)           print string with newline and flush
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "usb_descr.h"
#include "usb_handler.h"

// ===================================================================================
// CDC Parameters
// ===================================================================================
#define CDC_PRINT       1         // 1: include print functions (needs print.h)

// ===================================================================================
// CDC Functions
// ===================================================================================
void CDC_init(void);              // setup USB-CDC
void CDC_flush(void);             // flush OUT buffer
char CDC_read(void);              // read single character from IN buffer
void CDC_write(char c);           // write single character to OUT buffer
uint8_t CDC_available(void);      // check number of bytes in the IN buffer
uint8_t CDC_ready(void);          // check if OUT buffer is ready to be written

#define CDC_writeflush(c)         {CDC_write(c);CDC_flush();}     // write & flush char
#define CDC_newline()             {CDC_write('\n'); CDC_flush();} // newline and flush

// ===================================================================================
// CDC Control Line State
// ===================================================================================
extern volatile uint8_t CDC_controlLineState;               // control line state
#define CDC_DTR_flag    (CDC_controlLineState & 1)          // DTR flag
#define CDC_RTS_flag    ((CDC_controlLineState >> 1) & 1)   // RTS flag
#define CDC_getDTR()    (CDC_DTR_flag)                      // get DTR flag
#define CDC_getRTS()    (CDC_RTS_flag)                      // get RTS flag

// ===================================================================================
// CDC Line Coding
// ===================================================================================
typedef struct {
  uint32_t baudrate;              // baud rate
  uint8_t  stopbits;              // number of stopbits (0:1bit,1:1.5bits,2:2bits)
  uint8_t  parity;                // parity (0:none,1:odd,2:even,3:mark,4:space)
  uint8_t  databits;              // number of data bits (5,6,7,8 or 16)
} CDC_LINE_CODING_TYPE;

extern CDC_LINE_CODING_TYPE CDC_lineCoding;
#define CDC_getBAUD()       (CDC_lineCoding.baudrate)
#define CDC_getParity()     (CDC_lineCoding.parity)
#define CDC_getStopBits()   (CDC_lineCoding.stopbits)
#define CDC_getDataBits()   (CDC_lineCoding.databits)

// ===================================================================================
// Additional Print Functions (if activated, see above)
// ===================================================================================
#if CDC_PRINT == 1
#include "print.h"
#define CDC_printD(n)         printD(CDC_write, n)    // print decimal as string
#define CDC_printW(n)         printW(CDC_write, n)    // print word as string
#define CDC_printH(n)         printH(CDC_write, n)    // print half-word as string
#define CDC_printB(n)         printB(CDC_write, n)    // print byte as string
#define CDC_printS(s)         printS(CDC_write, s)    // print string
#define CDC_println(s)        {println(CDC_write, s); CDC_flush();}
#define CDC_print             CDC_printS              // alias
#define CDC_printf(f, ...)    {printF(CDC_write, f, ##__VA_ARGS__); CDC_flush();}
#endif

#ifdef __cplusplus
}
#endif
