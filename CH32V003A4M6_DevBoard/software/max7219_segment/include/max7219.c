// ===================================================================================
// Basic MAX7219/7221 Segment/Matrix LED Display Functions                    * v0.1 *
// ===================================================================================
// 2023 by Stefan Wagner: https://github.com/wagiminator

#include "max7219.h"

#if SEG_COUNT == 8
  #define SEG_DIVIDER 10000000
#elif SEG_COUNT == 7
  #define SEG_DIVIDER 1000000
#elif SEG_COUNT == 6
  #define SEG_DIVIDER 100000
#elif SEG_COUNT == 5
  #define SEG_DIVIDER 10000
#elif SEG_COUNT == 4
  #define SEG_DIVIDER 1000
#elif SEG_COUNT == 3
  #define SEG_DIVIDER 100
#elif SEG_COUNT == 2
  #define SEG_DIVIDER 10
#else
  #define SEG_DIVIDER 1
#endif

// MAX7219 send command
void MAX_send(uint8_t reg, uint8_t data) {
  PIN_low(MAX_PIN_CS);                            // set CS low  -> select device
  SPI_write(reg);                                 // send address byte
  SPI_write(data);                                // send data byte
  while(SPI_busy());                              // wait for end of transmission
  PIN_high(MAX_PIN_CS);                           // set CS high -> latch the bytes
}

// Init MAX7219 for segment display
void SEG_init(void) {
  PIN_high(MAX_PIN_CS);                           // set CS low (unselect device)
  PIN_output(MAX_PIN_CS);                         // set CS pin to output
  SPI_init();                                     // init SPI
  MAX_send(MAX_ADDR_MODE,   0xff);                // set BCD decode for all digits
  MAX_send(MAX_ADDR_BRIGHT, MAX_BRIGHT);          // set intensity (0x00 .. 0x0f)
  MAX_send(MAX_ADDR_LIMIT,  SEG_COUNT-1);         // set scan limit (number of digits)
  MAX_send(MAX_ADDR_SHTDWN, 0x01);                // shutdown mode off
  MAX_send(MAX_ADDR_TEST,   0x00);                // display test off
}

// Print a decimal value on segment display
void SEG_printD(uint32_t value) {
  for(uint8_t i=1; i<=SEG_COUNT; i++, value /= 10) MAX_send(i, value % 10);
}

// Print value with decimal at dotpos on segment display
void SEG_print(int32_t value, uint8_t dotpos) {
  uint8_t digit = SEG_COUNT;                      // total number of digits
  uint32_t divider = SEG_DIVIDER;                 // current divider
  if(value < 0) {                                 // negativ value?
    MAX_send(SEG_COUNT, 10);                      // -> write "-" on left side
    value = -value;                               // -> make it a positive value
    digit--;                                      // -> one digit less
    divider /= 10;                                // -> current divider
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
      if(digit) MAX_send(digit+1, digitval|0x80); // -> print digit with dot ...
      else MAX_send(1, digitval);                 // ... or without dot if rightmost digit
    }
    else if(leadflag) MAX_send(digit+1, digitval);// print digit ...
    else MAX_send(digit+1, 0x0f);                 // ... or leading space
  }
}
