// ===================================================================================
// Basic Serial Debug Functions for CH32V203                                  * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "debug_serial.h"

#if DEBUG_ENABLE > 0

// Init debug interface (UART TX)
void DEBUG_init(void) {
  // Set pin PA2 to output, push-pull, 10MHz, auxiliary
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN;
  GPIOA->CFGLR    = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(2<<2)))
                                  |  ((uint32_t)0b1001<<(2<<2));
	
  // Setup and start UART (8N1, TX only)
  RCC->APB1PCENR |= RCC_USART2EN;
  USART2->BRR     = ((2 * F_CPU / DEBUG_BAUD) + 1) / 2;
  USART2->CTLR1   = USART_CTLR1_TE | USART_CTLR1_UE;
}

// Send byte via UART
void DEBUG_write(const char c) {
  while(!(USART2->STATR & USART_STATR_TXE));
  USART2->DATAR = c;
}

// Send string via UART
void DEBUG_print(const char* str) {
  while(*str) DEBUG_write(*str++);
}

// Send string via UART (for printf)
int puts(const char* str) {
  while(*str) DEBUG_write(*str++);
  return 1;
}

// Send string via UART with newline
void DEBUG_println(const char* str) {
  DEBUG_print(str);
  DEBUG_write('\n');
}

// Print decimal value
void DEBUG_printD(uint32_t value) {
  uint8_t digitval;                               // current digit value
  uint8_t leadflag = 0;                           // flag for leading spaces
  uint32_t divider = 1000000000;                  // current divider
  while(divider) {                                // for all digits
    digitval = value / divider;                   // calculate digit value
    value    = value % divider;                   // set value to division remainder
    divider /= 10;                                // calculate next divider
    if(digitval) leadflag++;                      // end of leading spaces
    if(!divider) leadflag++;                      // least digit has to be printed
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
