// ===================================================================================
// USB CDC Functions for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>

void CDC_init(void);              // setup USB-CDC
void CDC_flush(void);             // flush OUT buffer
char CDC_read(void);              // read single character from IN buffer
void CDC_write(char c);           // write single character to OUT buffer
void CDC_print(char* str);        // write string to OUT buffer
void CDC_println(char* str);      // write string with newline to OUT buffer and flush
uint8_t CDC_available(void);      // check number of bytes in the IN buffer
__bit CDC_ready(void);            // check if OUT buffer is ready to be written
__bit CDC_getDTR(void);           // get DTR flag
__bit CDC_getRTS(void);           // get RTS flag
