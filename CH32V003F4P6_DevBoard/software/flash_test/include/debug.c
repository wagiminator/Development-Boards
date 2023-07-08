// ===================================================================================
// Basic Serial Debug Functions for CH32V003                                  * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "debug.h"

#if DEBUG_ENABLE > 0

// Init debug interface (UART TX)
void DEBUG_init(void) {
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;
  GPIOD->CFGLR    = (GPIOD->CFGLR & ~((uint32_t)0b1111<<(5<<2)))
                                  |  ((uint32_t)0b1001<<(5<<2));
  USART1->BRR     = ((2 * F_CPU / DEBUG_BAUD) + 1) / 2;
  USART1->CTLR1   = USART_CTLR1_TE | USART_CTLR1_UE;
}

// Send byte via UART
void DEBUG_write(const char c) {
  while(!(USART1->STATR & USART_STATR_TC));
  USART1->DATAR = c;
}

// Send string via UART (for printf)
int puts(const char* str) {
  while(*str) DEBUG_write(*str++);
  return 1;
}

// Send string via UART
void DEBUG_print(const char* str) {
  while(*str) DEBUG_write(*str++);
}

// Send string via UART with newline
void DEBUG_println(const char* str) {
  DEBUG_print(str);
  DEBUG_write('\n');
}

// For BCD conversion
const uint32_t DIVIDER[] = {1, 10, 100, 1000, 10000, 100000, 1000000,
                            10000000, 100000000, 1000000000};

// Print decimal value (BCD conversion by substraction method)
void DEBUG_printD(uint32_t value) {
  uint8_t digits   = 10;                          // print 10 digits
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digits--) {                               // for all digits digits
    uint8_t digitval = 0;                         // start with digit value 0
    uint32_t divider = DIVIDER[digits];           // read current divider
    while(value >= divider) {                     // if current divider fits into the value
      leadflag = 1;                               // end of leading spaces
      digitval++;                                 // increase digit value
      value -= divider;                           // decrease value by divider
    }
    if(!digits)  leadflag++;                      // least digit has to be printed
    if(leadflag) DEBUG_write(digitval + '0');     // print the digit
  }
}

// Convert byte nibble into hex character and print it via UART
void DEBUG_printN(uint8_t nibble) {
  DEBUG_write((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert byte into hex characters and print it via UART
void DEBUG_printB(uint8_t value) {
  DEBUG_printN(value >> 4);
  DEBUG_printN(value & 0x0f);
}

// Convert word into hex characters and print it via UART
void DEBUG_printW(uint16_t value) {
  DEBUG_printB(value >> 8);
  DEBUG_printB(value);
}

// Convert long into hex characters and print it via UART
void DEBUG_printL(uint32_t value) {
  DEBUG_printW(value >> 16);
  DEBUG_printW(value);
}

#endif
