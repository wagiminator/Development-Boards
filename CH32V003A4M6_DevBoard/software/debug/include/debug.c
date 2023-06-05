// ===================================================================================
// Basic Serial Debug Functions for CH32V003                                  * v1.0 *
// ===================================================================================

#include "debug.h"

// DEBUG calculate BAUD rate setting
#define DEBUG_BRR   (((2 * (F_CPU) / (DEBUG_BAUD)) + 1) / 2)

// Init debug interface (UART TX)
void DEBUG_init(void) {
  #if DEBUG_ENABLE == 1
  
  // Enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;

  // Set pin PD5 to output, push-pull, 10MHz, auxiliary
  GPIOD->CFGLR &= ~(0b1111<<(5<<2));
  GPIOD->CFGLR |=   0b1001<<(5<<2);
	
  // Setup and start UART (8N1, TX only)
  USART1->BRR   = DEBUG_BRR;
  USART1->CTLR1 = USART_CTLR1_TE | USART_CTLR1_UE;
  #endif
}

// Send byte via UART (for printf)
int putchar(int c) {
  #if DEBUG_ENABLE == 1
  while(!(USART1->STATR & USART_STATR_TC));
  USART1->DATAR = (const char)c;
  #endif
  return 1;
}

// Send byte via UART
void DEBUG_write(const char c) {
  #if DEBUG_ENABLE == 1
  while(!(USART1->STATR & USART_STATR_TC));
  USART1->DATAR = c;
  #endif
}

// Send string via UART
void DEBUG_print(const char* str) {
  #if DEBUG_ENABLE == 1
  while(*str) DEBUG_write(*str++);
  #endif
}

// Send string via UART with newline
void DEBUG_println(const char* str) {
  #if DEBUG_ENABLE == 1
  DEBUG_print(str);
  DEBUG_write('\n');
  #endif
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
void DEBUG_printH(uint32_t value) {
  DEBUG_printW(value >> 16);
  DEBUG_printW(value);
}
