// ===================================================================================
// Basic UART Functions for CH32V203  (no buffer, no interrupt, no DMA)       * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"

// ===================================================================================
// UART1
// ===================================================================================

// Init UART
void UART1_init(void) {
#if UART1_REMAP == 0
  // Enable GPIO port A and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_USART1EN;

  // Set pin PA9  (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PA10 (RX) to input, pullup
  GPIOA->CFGHR  = (GPIOA->CFGHR & ~(((uint32_t)0b1111<<(1<<2)) | ((uint32_t)0b1111<<(2<<2))))
                                |  (((uint32_t)0b1001<<(1<<2)) | ((uint32_t)0b1000<<(2<<2)));
  GPIOA->OUTDR |= 1<<10;
#elif UART1_REMAP == 1
  // Remap UART pins, enable GPIO port B and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN | RCC_USART1EN;
  
  // Set pin PB6 (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PB7 (RX) to input, pullup
  GPIOB->CFGLR  = (GPIOB->CFGLR & ~(((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(7<<2))))
                                |  (((uint32_t)0b1001<<(6<<2)) | ((uint32_t)0b1000<<(7<<2)));
  GPIOB->OUTDR |= 1<<7;
  AFIO->PCFR1  |= 1<<2;
#elif UART1_REMAP == 2
  // Remap UART pins, enable GPIO port A/B and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_USART1EN;

  // Set pin PB15 (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PA8  (RX) to input, pullup
  GPIOB->CFGHR  = (GPIOB->CFGHR & ~((uint32_t)0b1111<<(7<<2))) | ((uint32_t)0b1001<<(7<<2));
  GPIOA->CFGHR  = (GPIOA->CFGHR & ~((uint32_t)0b1111<<(1<<2))) | ((uint32_t)0b1000<<(1<<2));
  GPIOA->OUTDR |= 1<<8;
  AFIO->PCFR2  |= 1<<26;
#elif UART1_REMAP == 3
  // Remap UART pins, enable GPIO port A and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_USART1EN;

  // Set pin PA6 (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PA7 (RX) to input, pullup
  GPIOA->CFGLR  = (GPIOA->CFGLR & ~(((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(7<<2))))
                                |  (((uint32_t)0b1001<<(6<<2)) | ((uint32_t)0b1000<<(7<<2)));
  GPIOA->OUTDR |= 1<<7;
  AFIO->PCFR1  |= 1<<2;
  AFIO->PCFR2  |= 1<<26;
#else
  #warning Wrong UART1 REMAP
#endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  USART1->BRR   = ((2 * F_CPU / UART1_BAUD) + 1) / 2;
  USART1->CTLR1 = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_UE;
}

// Read byte via UART
char UART1_read(void) {
  while(!UART1_available());
  return USART1->DATAR;
}

// Send byte via UART
void UART1_write(const char c) {
  while(!UART1_ready());
  USART1->DATAR = c;
}

// Send string via UART
void UART1_print(const char* str) {
  while(*str) UART1_write(*str++);
}

// Send string via UART with newline
void UART1_println(const char* str) {
  UART1_print(str);
  UART1_write('\n');
}

// ===================================================================================
// UART2
// ===================================================================================

// Init UART
void UART2_init(void) {
#if UART2_REMAP == 0
  // Set pin PA2 (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PA3 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN;
  GPIOA->CFGLR    = (GPIOA->CFGLR & ~(((uint32_t)0b1111<<(2<<2)) | ((uint32_t)0b1111<<(3<<2))))
                                  |  (((uint32_t)0b1001<<(2<<2)) | ((uint32_t)0b1000<<(3<<2)));
  GPIOA->OUTDR   |= 1<<3;
#elif UART2_REMAP == 1 
  // Set pin PD5 (TX) to output, push-pull, 10MHz, multiplex
  // Set pin PD6 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN;
  GPIOD->CFGLR    = (GPIOD->CFGLR & ~(((uint32_t)0b1111<<(5<<2)) | ((uint32_t)0b1111<<(6<<2))))
                                  |  (((uint32_t)0b1001<<(5<<2)) | ((uint32_t)0b1000<<(6<<2)));
  GPIOD->OUTDR   |= 1<<6;
  AFIO->PCFR1    |= 1<<3;
#else
  #warning Wrong UART2 REMAP
#endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  RCC->APB1PCENR |= RCC_USART2EN;
  USART2->BRR     = ((2 * F_CPU / UART2_BAUD) + 1) / 2;
  USART2->CTLR1   = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_UE;
}

// Read byte via UART
char UART2_read(void) {
  while(!UART2_available());
  return USART2->DATAR;
}

// Send byte via UART
void UART2_write(const char c) {
  while(!UART2_ready());
  USART2->DATAR = c;
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
