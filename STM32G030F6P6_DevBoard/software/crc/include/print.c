// ===================================================================================
// Basic PRINT Functions                                                      * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "print.h"

// For BCD conversion
const uint32_t DIVIDER[] = {1, 10, 100, 1000, 10000, 100000, 1000000,
                            10000000, 100000000, 1000000000};

// Print decimal value (BCD conversion by substraction method)
void printD(uint32_t value, void (*putchar) (char c)) {
  uint8_t digits   = 10;                          // print 10 digits
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digits--) {                               // for all digits
    uint8_t digitval = 0;                         // start with digit value 0
    uint32_t divider = DIVIDER[digits];           // read current divider
    while(value >= divider) {                     // if current divider fits into the value
      leadflag = 1;                               // end of leading spaces
      digitval++;                                 // increase digit value
      value -= divider;                           // decrease value by divider
    }
    if(!digits)  leadflag++;                      // least digit has to be printed
    if(leadflag) putchar(digitval + '0');         // print the digit
  }
}

// Convert 4-bit byte nibble into hex character and print it via putchar
void printN(uint8_t nibble, void (*putchar) (char c)) {
  putchar((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert 8-bit byte into hex characters and print it via putchar
void printB(uint8_t value, void (*putchar) (char c)) {
  printN(value >> 4, putchar);
  printN(value & 0x0f, putchar);
}

// Convert 16-bit half-word into hex characters and print it via putchar
void printH(uint16_t value, void (*putchar) (char c)) {
  printB(value >> 8, putchar);
  printB(value, putchar);
}

// Convert 32-bit word into hex characters and print it via putchar
void printW(uint32_t value, void (*putchar) (char c)) {
  printH(value >> 16, putchar);
  printH(value, putchar);
}

// Print string via putchar
void printS(const char* str, void (*putchar) (char c)) {
  while(*str) putchar(*str++);
}

// Print string with newline via putchar
void println(const char* str, void (*putchar) (char c)) {
  while(*str) putchar(*str++);
  putchar('\n');
}
