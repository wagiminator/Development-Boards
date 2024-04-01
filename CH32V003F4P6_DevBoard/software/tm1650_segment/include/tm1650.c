// ===================================================================================
// Basic TM1650 4-Digit 8-Segment LED Display Functions                       * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner: https://github.com/wagiminator

#include "tm1650.h"

// Number to segment code conversion table
const uint8_t TM_DATA[] = {
  0x3f, 0x06, 0x5b, 0x4f, 0x66, 0x6d, 0x7d, 0x07,
  0x7f, 0x6f, 0x77, 0x7c, 0x39, 0x5e, 0x79, 0x71
};

// Write value to specified TM1650 address
void TM_write(uint8_t addr, uint8_t value) {
  I2C_start(addr);
  I2C_write(value);
  I2C_stop();
}

// Set display brightness
void TM_setBrightness(uint8_t bright) {
  TM_write(TM_ADDR_MODE, TM_BRIGHTNESS(bright) | TM_DISPLAY_ON);
}

// Init TM1650 4-digit 8-segment display
void TM_init(void) {
  I2C_init();
  TM_write(TM_ADDR_MODE, TM_BRIGHTNESS(TM_BRIGHT) | TM_DISPLAY_ON);
}

// Print segment code to the specified digit (0-3, counting from right)
void TM_printSegment(uint8_t digit, uint8_t code) {
  TM_write(TM_ADDR_DIG4 - (digit << 1), code);
}

// Print number to the specified digit (0-3, counting from right)
void TM_printDigit(uint8_t digit, uint8_t value) {
  TM_write(TM_ADDR_DIG4 - (digit << 1), TM_DATA[value]);
}

// Print hexadecimal value
void TM_printH(uint16_t value) {
  for(uint8_t i=0; i<4; i++) {
    TM_printDigit(i, value & 0xf);
    value >>= 4;
  }
}

#if TM_HW_DIV == 0                                // no hardware division

// For BCD conversion
const uint16_t TM_DIVIDER[] = {1, 10, 100, 1000};

// Print decimal value (BCD conversion by substraction method)
void TM_printD(uint16_t value) {
  uint8_t digit    = 4;                           // print 4 digits
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digit--) {                                // for all digits digits
    uint8_t digitval = 0;                         // start with digit value 0
    uint16_t divider = TM_DIVIDER[digit];         // read current divider
    while(value >= divider) {                     // if current divider fits into the value:
      leadflag = 1;                               // -> end of leading spaces
      digitval++;                                 // -> increase digit value
      value -= divider;                           // -> decrease value by divider
    }
    if(!digit) leadflag++;                        // least digit has to be printed
    if(leadflag) TM_printDigit(digit, digitval);  // print digit ...
    else TM_printSegment(digit, 0);               // ... or leading space
  }
}

// Print value with decimal at dotpos (BCD conversion by substraction method)
void TM_print(int16_t value, uint8_t dotpos) {
  uint8_t digit = 4;                              // print 4 digits
  if(value < 0) {                                 // negativ value?
    TM_write(TM_ADDR_DIG1, TM_CHAR_NEG);          // -> write "-" on left side
    value = -value;                               // -> make it a positive value
    digit--;                                      // -> one digit less
  }
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digit--) {                                // for all digits digits
    uint8_t digitval = 0;                         // start with digit value 0
    uint16_t divider = TM_DIVIDER[digit];         // read current divider
    while(value >= divider) {                     // if current divider fits into the value:
      leadflag++;                                 // -> end of leading spaces
      digitval++;                                 // -> increase digit value
      value -= divider;                           // -> decrease value by divider
    }
    if(digit == dotpos) {                         // digit with the dot?
      leadflag++;                                 // -> end of leading spaces
      if(digit) TM_printSegment(digit, TM_DATA[digitval] | 0x80); // -> print digit with dot ...
      else TM_printDigit(0, digitval);            // ... or without dot if rightmost digit
    }
    else if(leadflag) TM_printDigit(digit, digitval); // print digit ...
    else TM_printSegment(digit, 0);               // ... or leading space
  }
}

#else                                             // hardware division

// Print decimal value (using multiplication/division)
void TM_printD(uint16_t value) {
  uint8_t digit = 4;                              // print 4 digits
  uint8_t digitval;                               // current digit value
  uint8_t leadflag = 0;                           // flag for leading spaces
  uint16_t divider = 1000;                        // current divider
  while(digit--) {                                // for all digits
    digitval = value / divider;                   // calculate digit value
    value    = value % divider;                   // set value to division remainder
    divider /= 10;                                // calculate next divider
    if(digitval) leadflag++;                      // end of leading spaces
    if(!digit)   leadflag++;                      // least digit has to be printed
    if(leadflag) TM_printDigit(digit, digitval);  // print digit ...
    else TM_printSegment(digit, 0);               // ... or leading space
  }
}

// Print value with decimal at dotpos (using multiplication/division)
void TM_print(int16_t value, uint8_t dotpos) {
  uint8_t digit = 4;                              // print 4 digits
  uint16_t divider = 1000;                        // current divider
  if(value < 0) {                                 // negativ value?
    TM_write(TM_ADDR_DIG1, TM_CHAR_NEG);          // -> write "-" on left side
    value = -value;                               // -> make it a positive value
    digit--;                                      // -> one digit less
    divider = 100;                                // -> current divider
  }
  uint8_t digitval;                               // current digit value
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digit--) {                                // for all digits
    digitval = value / divider;                   // calculate digit value
    value    = value % divider;                   // set value to division remainder
    divider /= 10;                                // calculate next divider
    if(digitval) leadflag++;                      // end of leading spaces
    if(digit == dotpos) {                         // digit with the dot?
      leadflag++;                                 // -> end of leading spaces
      if(digit) TM_printSegment(digit, TM_DATA[digitval] | 0x80); // -> print digit with dot ...
      else TM_printDigit(0, digitval);            // ... or without dot if rightmost digit
    }
    else if(leadflag) TM_printDigit(digit, digitval); // print digit ...
    else TM_printSegment(digit, 0);               // ... or leading space
  }
}

#endif  // TM_HW_DIV
