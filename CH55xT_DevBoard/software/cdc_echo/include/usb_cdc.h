// ===================================================================================
// USB CDC Functions for CH55x
// ===================================================================================

#pragma once

#include "ch554.h"
#include "usb.h"
#include "usb_descr.h"
#include "usb_handler.h"

void CDC_flush(void);
char CDC_read(void);
void CDC_print(char* str);
void CDC_println(char* str);
uint8_t CDC_available(void);
