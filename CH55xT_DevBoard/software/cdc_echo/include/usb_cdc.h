// ===================================================================================
// USB CDC Functions for CH55x
// ===================================================================================

#pragma once

#include <stdint.h>
#include "usb_handler.h"

void CDC_flush(void);             // flush OUT buffer
char CDC_read(void);              // read single character from IN buffer
void CDC_write(char c);           // write single character to OUT buffer
void CDC_print(char* str);        // write string to OUT buffer
void CDC_println(char* str);      // write string with newline to OUT buffer
uint8_t CDC_available(void);      // check if something is in the IN buffer
uint8_t CDC_getDTR(void);         // get DTR flag
uint8_t CDC_getRTS(void);         // get RTS flag
