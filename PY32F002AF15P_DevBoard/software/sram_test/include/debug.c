// ===================================================================================
// Basic Serial Debug Functions for PY32F0xx                                  * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "debug.h"

#if DEBUG_ENABLE > 0

// Init UART
void DEBUG_init(void) {
  // Setup GPIO pins
  #if DEBUG_TX == 0
    // Setup pin PA2 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(2<<1)) ))
                                     |  ( ((uint32_t)0b10<<(2<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(2<<0));
    GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(2<<1));
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(2<<2)) ))
                                     |  ( ((uint32_t)0x1 <<(2<<2)) );
  #elif DEBUG_TX == 1
    // Setup pin PA7 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(7<<1)) ))
                                     |  ( ((uint32_t)0b10<<(7<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(7<<0));
    GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(7<<1));
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(7<<2)) ))
                                     |  ( ((uint32_t)0x8 <<(7<<2)) );
  #elif DEBUG_TX == 2
    // Setup pin PA9 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(9<<1)) ))
                                     |  ( ((uint32_t)0b10<<(9<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(9<<0));
    GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(9<<1));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(1<<2)) ))
                                     |  ( ((uint32_t)0x1 <<(1<<2)) );
  #elif DEBUG_TX == 3
    // Setup pin PA10 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b10<<(10<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(10<<0));
    GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(10<<1));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(2<<2)) ))
                                     |  ( ((uint32_t)0x8 <<(2<<2)) );
  #elif DEBUG_TX == 4
    // Setup pin PA14 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(14<<1)) ))
                                     |  ( ((uint32_t)0b10<<(14<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(14<<0));
    GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(14<<1));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(6<<2)) ))
                                     |  ( ((uint32_t)0x1 <<(6<<2)) );
  #elif DEBUG_TX == 5
    // Setup pin PB6 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(6<<1)) ))
                                     |  ( ((uint32_t)0b10<<(6<<1)) );
    GPIOB->OTYPER  &=                  ~  ((uint32_t)0b1 <<(6<<0));
    GPIOB->OSPEEDR |=                     ((uint32_t)0b11<<(6<<1));
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~( ((uint32_t)0xf <<(6<<2)) ))
                                     |  ( ((uint32_t)0x0 <<(6<<2)) );
  #elif DEBUG_TX == 6
    // Setup pin PB8 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(8<<1)) ))
                                     |  ( ((uint32_t)0b10<<(8<<1)) );
    GPIOB->OTYPER  &=                  ~  ((uint32_t)0b1 <<(8<<0));
    GPIOB->OSPEEDR |=                     ((uint32_t)0b11<<(8<<1));
    GPIOB->AFR[1]   = (GPIOB->AFR[1] & ~( ((uint32_t)0xf <<(0<<2)) ))
                                     |  ( ((uint32_t)0x8 <<(0<<2)) );
  #elif DEBUG_TX == 7
    // Setup pin PF1 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOFEN;
    GPIOF->MODER    = (GPIOF->MODER  & ~( ((uint32_t)0b11<<(1<<1)) ))
                                     |  ( ((uint32_t)0b10<<(1<<1)) );
    GPIOF->OTYPER  &=                  ~  ((uint32_t)0b1 <<(1<<0));
    GPIOF->OSPEEDR |=                     ((uint32_t)0b11<<(1<<1));
    GPIOF->AFR[0]   = (GPIOF->AFR[0] & ~( ((uint32_t)0xf <<(1<<2)) ))
                                     |  ( ((uint32_t)0x8 <<(1<<2)) );
  #elif DEBUG_TX == 8
    // Setup pin PF3 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOFEN;
    GPIOF->MODER    = (GPIOF->MODER  & ~( ((uint32_t)0b11<<(3<<1)) ))
                                     |  ( ((uint32_t)0b10<<(3<<1)) );
    GPIOF->OTYPER  &=                  ~  ((uint32_t)0b1 <<(3<<0));
    GPIOF->OSPEEDR |=                     ((uint32_t)0b11<<(3<<1));
    GPIOF->AFR[0]   = (GPIOF->AFR[0] & ~( ((uint32_t)0xf <<(3<<2)) ))
                                     |  ( ((uint32_t)0x0 <<(3<<2)) );
  #else
    #warning No automatic pin mapping for USART1
  #endif
	
  // Setup and start UART (8N1, TX only, default BAUD rate)
  RCC->APBENR2 |= RCC_APBENR2_USART1EN;
  USART1->BRR = ((2 * F_CPU / DEBUG_BAUD) + 1) / 2;
  USART1->CR1 = USART_CR1_TE | USART_CR1_UE;
}

// Send byte via UART
void DEBUG_write(const char c) {
  while(!(USART1->SR & USART_SR_TXE));
  USART1->DR = c;
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

// Convert 4-bit byte nibble into hex character and print it via UART
void DEBUG_printN(uint8_t nibble) {
  DEBUG_write((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert 8-bit byte into hex characters and print it via UART
void DEBUG_printB(uint8_t value) {
  DEBUG_printN(value >> 4);
  DEBUG_printN(value & 0x0f);
}

// Convert 16-bit half-word into hex characters and print it via UART
void DEBUG_printH(uint16_t value) {
  DEBUG_printB(value >> 8);
  DEBUG_printB(value);
}

// Convert 32-bit word into hex characters and print it via UART
void DEBUG_printW(uint32_t value) {
  DEBUG_printH(value >> 16);
  DEBUG_printH(value);
}

#endif // DEBUG_ENABLE > 0
