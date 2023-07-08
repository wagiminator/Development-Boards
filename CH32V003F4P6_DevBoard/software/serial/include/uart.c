// ===================================================================================
// Basic UART Functions for CH32V003                                          * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"

// Init UART
void UART_init(void) {
#if UART_REMAP == 0
  // Enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;

  // Set pin PD5 (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PD6 (RX) to input, pullup
  GPIOD->CFGLR  = (GPIOD->CFGLR & ~(((uint32_t)0b1111<<(5<<2)) | ((uint32_t)0b1111<<(6<<2))))
                                |  (((uint32_t)0b1001<<(5<<2)) | ((uint32_t)0b1000<<(6<<2)));
  GPIOD->OUTDR |= 1<<6;
#elif UART_REMAP == 1
  // Remap UART pins, enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;
  AFIO->PCFR1    |= 1<<2;
  
  // Set pin PD0 (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PD1 (RX) to input, pullup
  GPIOD->CFGLR  = (GPIOD->CFGLR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
                                |  (((uint32_t)0b1001<<(0<<2)) | ((uint32_t)0b1000<<(1<<2)));
  GPIOD->OUTDR |= 1<<1;
#elif UART_REMAP == 2
  // Remap UART pins, enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;
  AFIO->PCFR1    |= 1<<21;

  // Set pin PD6 (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PD5 (RX) to input, pullup
  GPIOD->CFGLR  = (GPIOD->CFGLR & ~(((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(5<<2))))
                                |  (((uint32_t)0b1001<<(6<<2)) | ((uint32_t)0b1000<<(5<<2)));
  GPIOD->OUTDR |= 1<<5;
#elif UART_REMAP == 3
  // Remap UART pins, enable GPIO port C and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN | RCC_USART1EN;
  AFIO->PCFR1    |= (1<<21) | (1<<2);

  // Set pin PC0 (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PC1 (RX) to input, pullup
  GPIOC->CFGLR  = (GPIOC->CFGLR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
                                |  (((uint32_t)0b1001<<(0<<2)) | ((uint32_t)0b1000<<(1<<2)));
  GPIOC->OUTDR |=  1<<1;
#else
  #warning Wrong UART REMAP
#endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  USART1->BRR   = ((2 * F_CPU / UART_BAUD) + 1) / 2;
  USART1->CTLR1 = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_UE;
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());
  return USART1->DATAR;
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  USART1->DATAR = c;
}

// Send string via UART
void UART_print(const char* str) {
  while(*str) UART_write(*str++);
}

// Send string via UART with newline
void UART_println(const char* str) {
  UART_print(str);
  UART_write('\n');
}

// For BCD conversion
const uint32_t DIVIDER[] = {1, 10, 100, 1000, 10000, 100000, 1000000,
                            10000000, 100000000, 1000000000};

// Print decimal value (BCD conversion by substraction method)
void UART_printD(uint32_t value) {
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
    if(leadflag) UART_write(digitval + '0');      // print the digit
  }
}

// Convert byte nibble into hex character and print it via UART
void UART_printN(uint8_t nibble) {
  UART_write((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert byte into hex characters and print it via UART
void UART_printB(uint8_t value) {
  UART_printN(value >> 4);
  UART_printN(value & 0x0f);
}

// Convert word into hex characters and print it via UART
void UART_printW(uint16_t value) {
  UART_printB(value >> 8);
  UART_printB(value);
}

// Convert long into hex characters and print it via UART
void UART_printL(uint32_t value) {
  UART_printW(value >> 16);
  UART_printW(value);
}
