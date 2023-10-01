// ===================================================================================
// Basic USART2 Functions for PY32F0xx                                        * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart2.h"

// Init UART
void UART2_init(void) {
// Enable USART1 module
RCC->APBENR1 |= RCC_APBENR1_USART2EN;

#if UART2_MAP == 0
  // Setup pin PA0 (TX) and pin PA1 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(0<<1)) | ((uint32_t)0b11<<(1<<1)) ))
                                   |  ( ((uint32_t)0b10<<(0<<1)) | ((uint32_t)0b10<<(1<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(0<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(0<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(1<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(1<<1)) );
  GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(0<<2)) | ((uint32_t)0xf <<(1<<2)) ))
                                   |  ( ((uint32_t)0x9 <<(0<<2)) | ((uint32_t)0x9 <<(1<<2)) );
#elif UART2_MAP == 1
  // Setup pin PA2 (TX) and pin PA3 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(2<<1)) | ((uint32_t)0b11<<(3<<1)) ))
                                   |  ( ((uint32_t)0b10<<(2<<1)) | ((uint32_t)0b10<<(3<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(2<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(2<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(3<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(3<<1)) );
  GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(2<<2)) | ((uint32_t)0xf <<(3<<2)) ))
                                   |  ( ((uint32_t)0x4 <<(2<<2)) | ((uint32_t)0x4 <<(3<<2)) );
#elif UART2_MAP == 2
  // Setup pin PA4 (TX) and pin PA5 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(4<<1)) | ((uint32_t)0b11<<(5<<1)) ))
                                   |  ( ((uint32_t)0b10<<(4<<1)) | ((uint32_t)0b10<<(5<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(4<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(4<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(5<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(5<<1)) );
  GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(4<<2)) | ((uint32_t)0xf <<(5<<2)) ))
                                   |  ( ((uint32_t)0x9 <<(4<<2)) | ((uint32_t)0x9 <<(5<<2)) );
#elif UART2_MAP == 3
  // Setup pin PA7 (TX) and pin PA8 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(7<<1)) | ((uint32_t)0b11<<(8<<1)) ))
                                   |  ( ((uint32_t)0b10<<(7<<1)) | ((uint32_t)0b10<<(8<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(7<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(7<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(8<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(8<<1)) );
  GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(7<<2))                            ))
                                   |  ( ((uint32_t)0x9 <<(7<<2))                            );
  GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~(                            ((uint32_t)0xf <<(1<<2)) ))
                                   |  (                            ((uint32_t)0x9 <<(1<<2)) );
#elif UART2_MAP == 4
  // Setup pin PA9 (TX) and pin PA10 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                   |  ( ((uint32_t)0b10<<(9<<1)) | ((uint32_t)0b10<<(10<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(9<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(9<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(10<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(10<<1)) );
  GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(1<<2)) | ((uint32_t)0xf <<(2<<2)) ))
                                   |  ( ((uint32_t)0x4 <<(1<<2)) | ((uint32_t)0x4 <<(2<<2)) );
#elif UART2_MAP == 5
  // Setup pin PA14 (TX) and pin PA15 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(14<<1)) | ((uint32_t)0b11<<(15<<1)) ))
                                   |  ( ((uint32_t)0b10<<(14<<1)) | ((uint32_t)0b10<<(15<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(14<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(14<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                             ((uint32_t)0b11<<(15<<1)) ))
                                   |  (                             ((uint32_t)0b01<<(15<<1)) );
  GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(6<<2))  | ((uint32_t)0xf <<(7<<2)) ))
                                   |  ( ((uint32_t)0x4 <<(6<<2))  | ((uint32_t)0x4 <<(7<<2)) );
#elif UART2_MAP == 6
  // Setup pin PB6 (TX) and pin PB7 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
  GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1)) ))
                                   |  ( ((uint32_t)0b10<<(6<<1)) | ((uint32_t)0b10<<(7<<1)) );
  GPIOB->OTYPER  &=                  ~  ((uint32_t)0b1 <<(6<<0));
  GPIOB->OSPEEDR |=                     ((uint32_t)0b11<<(6<<1));
  GPIOB->PUPDR    = (GPIOB->PUPDR  & ~(                            ((uint32_t)0b11<<(7<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(7<<1)) );
  GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~( ((uint32_t)0xf <<(6<<2)) | ((uint32_t)0xf <<(7<<2)) ))
                                   |  ( ((uint32_t)0x4 <<(6<<2)) | ((uint32_t)0x4 <<(7<<2)) );
#elif UART2_MAP == 7
  // Setup pin PF1 (TX) and pin PF0 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOFEN;
  GPIOF->MODER    = (GPIOF->MODER  & ~( ((uint32_t)0b11<<(1<<1)) | ((uint32_t)0b11<<(0<<1)) ))
                                   |  ( ((uint32_t)0b10<<(1<<1)) | ((uint32_t)0b10<<(0<<1)) );
  GPIOF->OTYPER  &=                  ~  ((uint32_t)0b1 <<(1<<0));
  GPIOF->OSPEEDR |=                     ((uint32_t)0b11<<(1<<1));
  GPIOF->PUPDR    = (GPIOF->PUPDR  & ~(                            ((uint32_t)0b11<<(0<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(0<<1)) );
  GPIOF->AFR[0]   = (GPIOF->AFR[0] & ~( ((uint32_t)0xf <<(1<<2)) | ((uint32_t)0xf <<(0<<2)) ))
                                   |  ( ((uint32_t)0x4 <<(1<<2)) | ((uint32_t)0x4 <<(0<<2)) );
#elif UART2_MAP == 7
  // Setup pin PF3 (TX) and pin PF2 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOFEN;
  GPIOF->MODER    = (GPIOF->MODER  & ~( ((uint32_t)0b11<<(3<<1)) | ((uint32_t)0b11<<(2<<1)) ))
                                   |  ( ((uint32_t)0b10<<(3<<1)) | ((uint32_t)0b10<<(2<<1)) );
  GPIOF->OTYPER  &=                  ~  ((uint32_t)0b1 <<(3<<0));
  GPIOF->OSPEEDR |=                     ((uint32_t)0b11<<(3<<1));
  GPIOF->PUPDR    = (GPIOF->PUPDR  & ~(                            ((uint32_t)0b11<<(2<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(2<<1)) );
  GPIOF->AFR[0]   = (GPIOF->AFR[0] & ~( ((uint32_t)0xf <<(3<<2)) | ((uint32_t)0xf <<(2<<2)) ))
                                   |  ( ((uint32_t)0x4 <<(3<<2)) | ((uint32_t)0x4 <<(2<<2)) );
#else
  #warning No automatic pin mapping for USART1
#endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  USART2->BRR = ((2 * F_CPU / UART2_BAUD) + 1) / 2;
  USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

// Read byte via UART
char UART2_read(void) {
  while(!UART2_available());
  return USART2->DR;
}

// Send byte via UART
void UART2_write(const char c) {
  while(!UART2_ready());
  USART2->DR = c;
}

// Send string via UART
void UART2_print(const char* str) {
  while(*str) UART2_write(*str++);
}

// Send string via UART with newline
void UART2_println(const char* str) {
  UART2_print(str);
  UART2_write('\n');
}

// For BCD conversion
const uint32_t DIVIDER[] = {1, 10, 100, 1000, 10000, 100000, 1000000,
                            10000000, 100000000, 1000000000};

// Print decimal value (BCD conversion by substraction method)
void UART2_printD(uint32_t value) {
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
    if(leadflag) UART2_write(digitval + '0');     // print the digit
  }
}

// Convert 4-bit byte nibble into hex character and print it via UART
void UART2_printN(uint8_t nibble) {
  UART2_write((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert 8-bit byte into hex characters and print it via UART
void UART2_printB(uint8_t value) {
  UART2_printN(value >> 4);
  UART2_printN(value & 0x0f);
}

// Convert 16-bit half-word into hex characters and print it via UART
void UART2_printH(uint16_t value) {
  UART2_printB(value >> 8);
  UART2_printB(value);
}

// Convert 32-bit word into hex characters and print it via UART
void UART2_printW(uint32_t value) {
  UART2_printW(value >> 16);
  UART2_printW(value);
}
