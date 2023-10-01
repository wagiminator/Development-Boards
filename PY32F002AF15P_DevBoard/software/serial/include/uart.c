// ===================================================================================
// Basic USART1 Functions for PY32F0xx                                        * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"

// Init UART
void UART_init(void) {
// Enable USART1 module
RCC->APBENR2 |= RCC_APBENR2_USART1EN;

#if UART_MAP == 0
  // Setup pin PA2 (TX) and pin PA3 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(2<<1)) | ((uint32_t)0b11<<(3<<1)) ))
                                   |  ( ((uint32_t)0b10<<(2<<1)) | ((uint32_t)0b10<<(3<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(2<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(2<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(3<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(3<<1)) );
  GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(2<<2)) | ((uint32_t)0xf <<(3<<2)) ))
                                   |  ( ((uint32_t)0x1 <<(2<<2)) | ((uint32_t)0x1 <<(3<<2)) );
#elif UART_MAP == 1
  // Setup pin PA7 (TX) and pin PA8 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(7<<1)) | ((uint32_t)0b11<<(8<<1)) ))
                                   |  ( ((uint32_t)0b10<<(7<<1)) | ((uint32_t)0b10<<(8<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(7<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(7<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(8<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(8<<1)) );
  GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(7<<2))                            ))
                                   |  ( ((uint32_t)0x8 <<(7<<2))                            );
  GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~(                            ((uint32_t)0xf <<(1<<2)) ))
                                   |  (                            ((uint32_t)0x8 <<(1<<2)) );
#elif UART_MAP == 2
  // Setup pin PA9 (TX) and pin PA10 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                   |  ( ((uint32_t)0b10<<(9<<1)) | ((uint32_t)0b10<<(10<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(9<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(9<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(10<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(10<<1)) );
  GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(1<<2)) | ((uint32_t)0xf <<(2<<2)) ))
                                   |  ( ((uint32_t)0x1 <<(1<<2)) | ((uint32_t)0x1 <<(2<<2)) );
#elif UART_MAP == 3
  // Setup pin PA14 (TX) and pin PA13 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(14<<1)) | ((uint32_t)0b11<<(13<<1)) ))
                                   |  ( ((uint32_t)0b10<<(14<<1)) | ((uint32_t)0b10<<(13<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(14<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(14<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                             ((uint32_t)0b11<<(13<<1)) ))
                                   |  (                             ((uint32_t)0b01<<(13<<1)) );
  GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(6<<2))  | ((uint32_t)0xf <<(5<<2)) ))
                                   |  ( ((uint32_t)0x1 <<(6<<2))  | ((uint32_t)0x8 <<(5<<2)) );
#elif UART_MAP == 4
  // Setup pin PA14 (TX) and pin PA15 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
  GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(14<<1)) | ((uint32_t)0b11<<(15<<1)) ))
                                   |  ( ((uint32_t)0b10<<(14<<1)) | ((uint32_t)0b10<<(15<<1)) );
  GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(14<<0));
  GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(14<<1));
  GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                             ((uint32_t)0b11<<(15<<1)) ))
                                   |  (                             ((uint32_t)0b01<<(15<<1)) );
  GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(6<<2))  | ((uint32_t)0xf <<(7<<2)) ))
                                   |  ( ((uint32_t)0x1 <<(6<<2))  | ((uint32_t)0x1 <<(7<<2)) );
#elif UART_MAP == 5
  // Setup pin PB6 (TX) and pin PB7 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
  GPIOB->MODER    = (GPIOB->MODER & ~( ((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1)) ))
                                  |  ( ((uint32_t)0b10<<(6<<1)) | ((uint32_t)0b10<<(7<<1)) );
  GPIOB->OTYPER  &=                 ~  ((uint32_t)0b1 <<(6<<0));
  GPIOB->OSPEEDR |=                    ((uint32_t)0b11<<(6<<1));
  GPIOB->PUPDR    = (GPIOB->PUPDR & ~(                            ((uint32_t)0b11<<(7<<1)) ))
                                  |  (                            ((uint32_t)0b01<<(7<<1)) );
  GPIOB->AFR[0]  &=                 ~( ((uint32_t)0xf <<(6<<2)) | ((uint32_t)0xf <<(7<<2)) );
#elif UART_MAP == 6
  // Setup pin PF1 (TX) and pin PF0 (RX)
  RCC->IOPENR    |= RCC_IOPENR_GPIOFEN;
  GPIOF->MODER    = (GPIOF->MODER  & ~( ((uint32_t)0b11<<(1<<1)) | ((uint32_t)0b11<<(0<<1)) ))
                                   |  ( ((uint32_t)0b10<<(1<<1)) | ((uint32_t)0b10<<(0<<1)) );
  GPIOF->OTYPER  &=                  ~  ((uint32_t)0b1 <<(1<<0));
  GPIOF->OSPEEDR |=                     ((uint32_t)0b11<<(1<<1));
  GPIOF->PUPDR    = (GPIOF->PUPDR  & ~(                            ((uint32_t)0b11<<(0<<1)) ))
                                   |  (                            ((uint32_t)0b01<<(0<<1)) );
  GPIOF->AFR[0]   = (GPIOF->AFR[0] & ~( ((uint32_t)0xf <<(1<<2)) | ((uint32_t)0xf <<(0<<2)) ))
                                   |  ( ((uint32_t)0x8 <<(1<<2)) | ((uint32_t)0x8 <<(0<<2)) );
#else
  #warning No automatic pin mapping for USART1
#endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  USART1->BRR = ((2 * F_CPU / UART_BAUD) + 1) / 2;
  USART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());
  return USART1->DR;
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  USART1->DR = c;
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

// Convert 4-bit byte nibble into hex character and print it via UART
void UART_printN(uint8_t nibble) {
  UART_write((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert 8-bit byte into hex characters and print it via UART
void UART_printB(uint8_t value) {
  UART_printN(value >> 4);
  UART_printN(value & 0x0f);
}

// Convert 16-bit half-word into hex characters and print it via UART
void UART_printH(uint16_t value) {
  UART_printB(value >> 8);
  UART_printB(value);
}

// Convert 32-bit word into hex characters and print it via UART
void UART_printW(uint32_t value) {
  UART_printW(value >> 16);
  UART_printW(value);
}
