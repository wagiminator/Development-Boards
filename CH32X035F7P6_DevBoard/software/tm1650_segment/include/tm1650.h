// ===================================================================================
// Basic TM1650 4-Digit 8-Segment LED Display Functions                       * v1.0 *
// ===================================================================================
//
// Collection of the most necessary functions for controlling a 4-digit 8-segment LED
// Display via TM1650 and I2C.
//
// Functions available:
// --------------------
// TM_init()                      init TM1650 4-digit 8-segment display
// TM_write(addr, val)            write value to specified TM1650 address
// TM_setBrightness(bright)       set display brightness (1-8)
// TM_printSegment(digit, code)   print segment code to specified digit
// TM_printDigit(digit, val)      print number to the specified digit
// TM_printH(val)                 print hexadecimal value on display
// TM_printD(val)                 print positive decimal value on display
// TM_print(val, dotpos)          print value with decimal at dotpos
//
// The digits are counted from right to left starting with 0.
//
// 2023 by Stefan Wagner: https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c_soft.h"

#define TM_HW_DIV         0                       // 1: MCU has hardware division
#define TM_BRIGHT         1                       // initial display brightness (1-8)

// TM1650 register addresses
#define TM_ADDR_MODE      0x48                    // set mode
#define TM_ADDR_READ      0x4f                    // read button data
#define TM_ADDR_DIG1      0x68                    // write digit 1 (3)
#define TM_ADDR_DIG2      0x6a                    // write digit 2 (2)
#define TM_ADDR_DIG3      0x6c                    // write digit 3 (1)
#define TM_ADDR_DIG4      0x6e                    // write digit 4 (0)

// TM1650 mode bits
#define TM_8SEGMENTS      (0<<3)                  // set 8 segments
#define TM_7SEGMENTS      (1<<3)                  // set 7 segments
#define TM_NORMAL         (0<<2)                  // set normal mode
#define TM_STANDBY        (1<<2)                  // set standby mode
#define TM_DISPLAY_OFF    (0<<0)                  // turn display off
#define TM_DISPLAY_ON     (1<<0)                  // turn display on
#define TM_BRIGHTNESS(b)  (((b)&7)<<4)            // set brigntness (1-8)

// TM1650 character segment codes
#define TM_CHAR_NEG       0x40                    // "-"
#define TM_CHAR_DEG       0x63                    // "°"
#define TM_CHAR_A         0x77                    // "A"
#define TM_CHAR_C         0x39                    // "C"
#define TM_CHAR_H         0x76                    // "H"
#define TM_CHAR_h         0x74                    // "h"
#define TM_CHAR_L         0x38                    // "L"
#define TM_CHAR_P         0x73                    // "P"
#define TM_CHAR_U         0x3f                    // "U"

// TM1650 conversion table
extern const uint8_t TM_DATA[];                   // number to segment code conversion table

// TM1650 functions
void TM_init(void);                               // init TM1650 4-digit 8-segment display
void TM_write(uint8_t addr, uint8_t value);       // write value to specified TM1650 address
void TM_setBrightness(uint8_t bright);            // set display brightness (1-8)
void TM_printSegment(uint8_t digit, uint8_t code);// print segment code to specified digit
void TM_printDigit(uint8_t digit, uint8_t value); // print number to the specified digit
void TM_printH(uint16_t value);                   // print hexadecimal value on display
void TM_printD(uint16_t value);                   // print positive decimal value on display
void TM_print(int16_t value, uint8_t dotpos);     // print value with decimal at dotpos

#ifdef __cplusplus
};
#endif
