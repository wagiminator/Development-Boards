// ===================================================================================
// Basic USB CDC Functions for CH551, CH552 and CH554                         * v1.2 *
// ===================================================================================

#pragma once
#include <stdint.h>

// ===================================================================================
// CDC Functions
// ===================================================================================
void CDC_init(void);              // setup USB-CDC
void CDC_flush(void);             // flush OUT buffer
char CDC_read(void);              // read single character from IN buffer
void CDC_write(char c);           // write single character to OUT buffer
void CDC_print(char* str);        // write string to OUT buffer
void CDC_println(char* str);      // write string with newline to OUT buffer and flush
uint8_t CDC_available(void);      // check number of bytes in the IN buffer
__bit CDC_ready(void);            // check if OUT buffer is ready to be written
#define CDC_writeflush(c)         {CDC_write(c);CDC_flush();}   // write & flush char

// ===================================================================================
// CDC Control Line State
// ===================================================================================
extern volatile __xdata uint8_t CDC_controlLineState;       // control line state
#define CDC_DTR_flag    (CDC_controlLineState & 1)          // DTR flag
#define CDC_RTS_flag    ((CDC_controlLineState >> 1) & 1)   // RTS flag
#define CDC_getDTR()    (CDC_DTR_flag)                      // get DTR flag
#define CDC_getRTS()    (CDC_RTS_flag)                      // get RTS flag

// ===================================================================================
// CDC Line Coding
// ===================================================================================
typedef struct _CDC_LINE_CODING_TYPE {
  uint32_t baudrate;              // baud rate
  uint8_t  stopbits;              // number of stopbits (0:1bit,1:1.5bits,2:2bits)
  uint8_t  parity;                // parity (0:none,1:odd,2:even,3:mark,4:space)
  uint8_t  databits;              // number of data bits (5,6,7,8 or 16)
} CDC_LINE_CODING_TYPE, *PCDC_LINE_CODING_TYPE;

extern __xdata CDC_LINE_CODING_TYPE CDC_lineCodingB;
#define CDC_lineCoding  ((PCDC_LINE_CODING_TYPE)CDC_lineCodingB)
#define CDC_getBAUD()   (CDC_lineCoding->baudrate)
