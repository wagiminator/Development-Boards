// ===================================================================================
// Basic USB CDC Functions for CH551, CH552 and CH554                         * v1.5 *
// ===================================================================================
//
// Functions available:
// --------------------
// CDC_init()               init USB-CDC
// CDC_available()          get number of bytes in the IN buffer
// CDC_ready()              check if OUT buffer is ready to be written
// CDC_read()               read single character from IN buffer
// CDC_write(c)             write single character to OUT buffer
// CDC_writeflush(c)        write single character to OUT buffer and flush
// CDC_print(s)             write string to OUT buffer
// CDC_println(s)           write string with newline to OUT buffer and flush
// CDC_flush()              flush OUT buffer
// CDC_getDTR()             get DTR flag
// CDC_getRTS()             get RTS flag
// CDC_getBAUD()            get BAUD rate
//
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"
#include "usb_descr.h"
#include "usb_handler.h"

// ===================================================================================
// CDC Variables
// ===================================================================================
extern volatile __xdata uint8_t CDC_readByteCount;// number of data bytes in IN buffer
extern volatile __bit CDC_writeBusyFlag;     // flag of whether upload pointer is busy

// ===================================================================================
// CDC Functions
// ===================================================================================
void CDC_flush(void);             // flush OUT buffer
char CDC_read(void);              // read single character from IN buffer
void CDC_write(char c);           // write single character to OUT buffer
void CDC_print(char* str);        // write string to OUT buffer
void CDC_println(char* str);      // write string with newline to OUT buffer and flush

#define CDC_init                  USB_init                      // setup USB-CDC
#define CDC_available()           (CDC_readByteCount)           // ready to be read
#define CDC_ready()               (!CDC_writeBusyFlag)          // ready to be written
#define CDC_writeflush(c)         {CDC_write(c);CDC_flush();}   // write & flush char

// ===================================================================================
// CDC Control Line State
// ===================================================================================
extern volatile __xdata uint8_t CDC_controlLineState;           // control line state
#define CDC_DTR_flag    (CDC_controlLineState & 1)              // DTR flag
#define CDC_RTS_flag    ((CDC_controlLineState >> 1) & 1)       // RTS flag
#define CDC_getDTR()    (CDC_DTR_flag)                          // get DTR flag
#define CDC_getRTS()    (CDC_RTS_flag)                          // get RTS flag

// ===================================================================================
// CDC Line Coding
// ===================================================================================
typedef struct _CDC_LINE_CODING_TYPE {
  uint32_t baudrate;              // baud rate
  uint8_t  stopbits;              // number of stopbits (0:1bit,1:1.5bits,2:2bits)
  uint8_t  parity;                // parity (0:none,1:odd,2:even,3:mark,4:space)
  uint8_t  databits;              // number of data bits (5,6,7,8 or 16)
} CDC_LINE_CODING_TYPE, *PCDC_LINE_CODING_TYPE;

extern __xdata CDC_LINE_CODING_TYPE CDC_lineCoding;
#define CDC_getBAUD()   (CDC_lineCoding.baudrate)
