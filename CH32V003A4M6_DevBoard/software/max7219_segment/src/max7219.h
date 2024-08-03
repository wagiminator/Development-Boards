// ===================================================================================
// Basic MAX7219/7221 Segment/Matrix LED Display Functions                    * v0.1 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling a segment/matrix LED
// Display via MAX7219/7221 and SPI.
//
// MAX7219 Display Functions available:
// ------------------------------------
// MAX_send(reg, data)      // write data to MAX7219 register
// MAX_off()                // put MAX7219 to shutdown
// MAX_on()                 // turn on MAX7219
// MAX_setBrightness(b)     // set display brightness (0-15)
//
// Segment Display Functions available:
// ------------------------------------
// SEG_init(v)              // init MAX7219 for segment display
// SEG_printD(val)          // print an integer value on segment display
// SEG_print(val, dotpos)   // print value with decimal at dotpos on segment display
//
// The digits are counted from right to left starting with 0.
//
// Matrix Display Functions available:
// -----------------------------------
// not yet implemented
//
// 2023 by Stefan Wagner: https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "gpio.h"
#include "spi_tx.h"

#define MAX_PIN_CS        PC4                   // define CS pin
#define MAX_BRIGHT        5                     // initial display brightness (0-15)

#define SEG_COUNT         8                     // number of digits of segment display

// MAX7219 register addresses
#define MAX_ADDR_NOP      0x00                  // no operation
#define MAX_ADDR_DIG0     0x01                  // write digit 0
#define MAX_ADDR_DIG1     0x02                  // write digit 1
#define MAX_ADDR_DIG2     0x03                  // write digit 2
#define MAX_ADDR_DIG3     0x04                  // write digit 3
#define MAX_ADDR_DIG4     0x05                  // write digit 4
#define MAX_ADDR_DIG5     0x06                  // write digit 5
#define MAX_ADDR_DIG6     0x07                  // write digit 6
#define MAX_ADDR_DIG7     0x08                  // write digit 7
#define MAX_ADDR_MODE     0x09                  // decode mode
#define MAX_ADDR_BRIGHT   0x0a                  // intensity
#define MAX_ADDR_LIMIT    0x0b                  // scan limit
#define MAX_ADDR_SHTDWN   0x0c                  // shutdown
#define MAX_ADDR_TEST     0x0f                  // display test

// MAX7219 character segment codes
#define SEG_CHAR_NEG      0x40                  // "-"
#define SEG_CHAR_DEG      0x63                  // "°"
#define SEG_CHAR_A        0x77                  // "A"
#define SEG_CHAR_C        0x39                  // "C"
#define SEG_CHAR_H        0x76                  // "H"
#define SEG_CHAR_h        0x74                  // "h"
#define SEG_CHAR_L        0x38                  // "L"
#define SEG_CHAR_P        0x73                  // "P"
#define SEG_CHAR_U        0x3f                  // "U"

// MAX7219 functions and macros
void MAX_send(uint8_t reg, uint8_t data);       // MAX7219 send command

#define MAX_setBrightness(b)  MAX_send(MAX_ADDR_BRIGHT, b);
#define MAX_off()             MAX_send(MAX_ADDR_SHTDWN, 0);
#define MAX_on()              MAX_send(MAX_ADDR_SHTDWN, 1);

// MAX7219 segment display functions
void SEG_init(void);                            // init MAX7219 for segment display
void SEG_printD(uint32_t value);                // print a decimal value on segment display
void SEG_print(int32_t value, uint8_t dotpos);  // print value with decimal at dotpos on segment display

#ifdef __cplusplus
};
#endif
