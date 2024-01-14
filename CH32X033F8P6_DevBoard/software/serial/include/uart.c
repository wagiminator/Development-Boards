// ===================================================================================
// Basic UART Functions for CH32X035/X034/X033  (no buffer/interrupt/DMA)     * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"

// ===================================================================================
// UART1
// ===================================================================================

// Init UART
void UART1_init(void) {
#if UART1_REMAP == 0
  // Enable GPIO port B and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN | RCC_USART1EN;

  // Set pin PB10 (TX) to output, push-pull, alternate
  // Set pin PB11 (RX) to input, pullup
  GPIOB->CFGHR = (GPIOB->CFGHR & ~(((uint32_t)0b1111<<(2<<2)) | ((uint32_t)0b1111<<(3<<2))))
                               |  (((uint32_t)0b1011<<(2<<2)) | ((uint32_t)0b1000<<(3<<2)));
  GPIOB->BSHR  = (uint32_t)1<<11;
#elif UART1_REMAP == 1
  // Enable GPIO port A and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_USART1EN;

  // Set pin PA10 (TX) to output, push-pull, alternate
  // Set pin PA11 (RX) to input, pullup
  GPIOA->CFGHR = (GPIOA->CFGHR & ~(((uint32_t)0b1111<<(2<<2)) | ((uint32_t)0b1111<<(3<<2))))
                               |  (((uint32_t)0b1011<<(2<<2)) | ((uint32_t)0b1000<<(3<<2)));
  GPIOA->BSHR  = (uint32_t)1<<11;
  AFIO->PCFR1 |= (uint32_t)0b01<<5;
#elif UART1_REMAP == 2
  // Enable GPIO port B and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN | RCC_USART1EN;

  // Set pin PB10 (TX) to output, push-pull, alternate
  // Set pin PB11 (RX) to input, pullup
  GPIOB->CFGHR = (GPIOB->CFGHR & ~(((uint32_t)0b1111<<(2<<2)) | ((uint32_t)0b1111<<(3<<2))))
                               |  (((uint32_t)0b1011<<(2<<2)) | ((uint32_t)0b1000<<(3<<2)));
  GPIOB->BSHR  = (uint32_t)1<<11;
  AFIO->PCFR1 |= (uint32_t)0b10<<5;
#elif UART1_REMAP == 3
  // Enable GPIO port A/B and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN | RCC_USART1EN;

  // Set pin PA7 (TX) to output, push-pull, alternate
  // Set pin PB2 (RX) to input, pullup
  GPIOA->CFGLR = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(7<<2))) | ((uint32_t)0b1011<<(7<<2));
  GPIOB->CFGLR = (GPIOB->CFGLR & ~((uint32_t)0b1111<<(2<<2))) | ((uint32_t)0b1000<<(2<<2));
  GPIOB->BSHR  = (uint32_t)1<<2;
  AFIO->PCFR1 |= (uint32_t)0b11<<5;
#else
  #warning No USART1 REMAP
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

// ===================================================================================
// UART2
// ===================================================================================

// Init UART
void UART2_init(void) {
#if UART2_REMAP == 0
  // Set pin PA2 (TX) to output, push-pull, alternate
  // Set pin PA3 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN;
  GPIOA->CFGLR    = (GPIOA->CFGLR & ~(((uint32_t)0b1111<<(2<<2)) | ((uint32_t)0b1111<<(3<<2))))
                                  |  (((uint32_t)0b1011<<(2<<2)) | ((uint32_t)0b1000<<(3<<2)));
  GPIOA->BSHR     = (uint32_t)1<<3;
#elif UART2_REMAP == 1 
  // Set pin PA20 (TX) to output, push-pull, alternate
  // Set pin PA19 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN;
  GPIOA->CFGXR    = (GPIOA->CFGXR & ~(((uint32_t)0b1111<<(4<<2)) | ((uint32_t)0b1111<<(3<<2))))
                                  |  (((uint32_t)0b1011<<(4<<2)) | ((uint32_t)0b1000<<(3<<2)));
  GPIOA->BSXR     = (uint32_t)1<<3;
  AFIO->PCFR1    |= (uint32_t)0b001<<7;
#elif UART2_REMAP == 2 
  // Set pin PA15 (TX) to output, push-pull, alternate
  // Set pin PA16 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN;
  GPIOA->CFGHR    = (GPIOA->CFGHR & ~((uint32_t)0b1111<<(7<<2))) | ((uint32_t)0b1011<<(7<<2)));
  GPIOA->CFGXR    = (GPIOA->CFGXR & ~((uint32_t)0b1111<<(0<<2))) | ((uint32_t)0b1000<<(0<<2)));
  GPIOA->BSXR     = (uint32_t)1<<0;
  AFIO->PCFR1    |= (uint32_t)0b010<<7;
#elif UART2_REMAP == 3
  // Set pin PC0 (TX) to output, push-pull, alternate
  // Set pin PC1 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  GPIOC->CFGLR    = (GPIOC->CFGLR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
                                  |  (((uint32_t)0b1011<<(0<<2)) | ((uint32_t)0b1000<<(1<<2)));
  GPIOC->BSHR     = (uint32_t)1<<1;
  AFIO->PCFR1    |= (uint32_t)0b011<<7;
#elif UART2_REMAP == 4 
  // Set pin PA15 (TX) to output, push-pull, alternate
  // Set pin PA16 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN;
  GPIOA->CFGHR    = (GPIOA->CFGHR & ~((uint32_t)0b1111<<(7<<2))) | ((uint32_t)0b1011<<(7<<2)));
  GPIOA->CFGXR    = (GPIOA->CFGXR & ~((uint32_t)0b1111<<(0<<2))) | ((uint32_t)0b1000<<(0<<2)));
  GPIOA->BSXR     = (uint32_t)1<<0;
  AFIO->PCFR1    |= (uint32_t)0b100<<7;
#else
  #warning No USART2 REMAP
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

// ===================================================================================
// UART3
// ===================================================================================

// Init UART
void UART3_init(void) {
#if UART3_REMAP == 0
  // Set pin PB3 (TX) to output, push-pull, alternate
  // Set pin PB4 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN;
  GPIOB->CFGLR    = (GPIOB->CFGLR & ~(((uint32_t)0b1111<<(3<<2)) | ((uint32_t)0b1111<<(4<<2))))
                                  |  (((uint32_t)0b1011<<(3<<2)) | ((uint32_t)0b1000<<(4<<2)));
  GPIOB->BSHR     = (uint32_t)1<<4;
#elif UART3_REMAP == 1 
  // Set pin PC18 (TX) to output, push-pull, alternate
  // Set pin PC19 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  GPIOC->CFGXR    = (GPIOC->CFGXR & ~(((uint32_t)0b1111<<(2<<2)) | ((uint32_t)0b1111<<(3<<2))))
                                  |  (((uint32_t)0b1011<<(2<<2)) | ((uint32_t)0b1000<<(3<<2)));
  GPIOC->BSXR     = (uint32_t)1<<3;
  AFIO->PCFR1    |= (uint32_t)0b01<<10;
#elif UART3_REMAP == 2 
  // Set pin PA18 (TX) to output, push-pull, alternate
  // Set pin PB14 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN;
  GPIOA->CFGXR    = (GPIOA->CFGXR & ~((uint32_t)0b1111<<(2<<2))) | ((uint32_t)0b1011<<(2<<2)));
  GPIOB->CFGHR    = (GPIOB->CFGHR & ~((uint32_t)0b1111<<(6<<2))) | ((uint32_t)0b1000<<(6<<2)));
  GPIOB->BSHR     = (uint32_t)1<<14;
  AFIO->PCFR1    |= (uint32_t)0b10<<10;
#else
  #warning No USART3 REMAP
#endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  RCC->APB1PCENR |= RCC_USART3EN;
  USART3->BRR     = ((2 * F_CPU / UART3_BAUD) + 1) / 2;
  USART3->CTLR1   = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_UE;
}

// Read byte via UART
char UART3_read(void) {
  while(!UART3_available());
  return USART3->DATAR;
}

// Send byte via UART
void UART3_write(const char c) {
  while(!UART3_ready());
  USART3->DATAR = c;
}

// ===================================================================================
// UART4
// ===================================================================================

// Init UART
void UART4_init(void) {
#if UART4_REMAP == 0
  // Set pin PB0 (TX) to output, push-pull, alternate
  // Set pin PB1 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN;
  GPIOB->CFGLR    = (GPIOB->CFGLR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
                                  |  (((uint32_t)0b1011<<(0<<2)) | ((uint32_t)0b1000<<(1<<2)));
  GPIOB->BSHR     = (uint32_t)1<<1;
#elif UART4_REMAP == 1 
  // Set pin PA5 (TX) to output, push-pull, alternate
  // Set pin PA9 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN;
  GPIOA->CFGLR    = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(5<<2))) | ((uint32_t)0b1011<<(5<<2)));
  GPIOA->CFGHR    = (GPIOA->CFGHR & ~((uint32_t)0b1111<<(1<<2))) | ((uint32_t)0b1000<<(1<<2)));
  GPIOA->BSHR     = (uint32_t)1<<9;
  AFIO->PCFR1    |= (uint32_t)0b001<<12;
#elif UART4_REMAP == 2 
  // Set pin PC16 (TX) to output, push-pull, alternate
  // Set pin PC17 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  GPIOC->CFGXR    = (GPIOC->CFGXR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
                                  |  (((uint32_t)0b1011<<(0<<2)) | ((uint32_t)0b1000<<(1<<2)));
  GPIOC->BSXR     = (uint32_t)1<<1;
  AFIO->PCFR1    |= (uint32_t)0b010<<12;
#elif UART4_REMAP == 3
  // Set pin PB9  (TX) to output, push-pull, alternate
  // Set pin PA10 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_IOPBEN;
  GPIOB->CFGHR    = (GPIOB->CFGHR & ~((uint32_t)0b1111<<(1<<2))) | ((uint32_t)0b1011<<(1<<2)));
  GPIOA->CFGHR    = (GPIOA->CFGHR & ~((uint32_t)0b1111<<(2<<2))) | ((uint32_t)0b1000<<(2<<2)));
  GPIOA->BSHR     = (uint32_t)1<<10;
  AFIO->PCFR1    |= (uint32_t)0b011<<12;
#elif UART4_REMAP == 4 
  // Set pin PB13 (TX) to output, push-pull, alternate
  // Set pin PC19 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN | RCC_IOPCEN;
  GPIOB->CFGHR    = (GPIOB->CFGHR & ~((uint32_t)0b1111<<(5<<2))) | ((uint32_t)0b1011<<(5<<2)));
  GPIOC->CFGXR    = (GPIOC->CFGXR & ~((uint32_t)0b1111<<(3<<2))) | ((uint32_t)0b1000<<(3<<2)));
  GPIOC->BSXR     = (uint32_t)1<<3;
  AFIO->PCFR1    |= (uint32_t)0b100<<12;
#elif UART4_REMAP == 5 
  // Set pin PC17 (TX) to output, push-pull, alternate
  // Set pin PC16 (RX) to input, pullup
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  GPIOC->CFGXR    = (GPIOC->CFGXR & ~(((uint32_t)0b1111<<(1<<2)) | ((uint32_t)0b1111<<(0<<2))))
                                  |  (((uint32_t)0b1011<<(1<<2)) | ((uint32_t)0b1000<<(0<<2)));
  GPIOC->BSXR     = (uint32_t)1<<0;
  AFIO->PCFR1    |= (uint32_t)0b101<<12;
#else
  #warning No USART4 REMAP
#endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  RCC->APB1PCENR |= RCC_USART4EN;
  USART4->BRR     = ((2 * F_CPU / UART4_BAUD) + 1) / 2;
  USART4->CTLR1   = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_UE;
}

// Read byte via UART
char UART4_read(void) {
  while(!UART4_available());
  return USART4->DATAR;
}

// Send byte via UART
void UART4_write(const char c) {
  while(!UART4_ready());
  USART4->DATAR = c;
}
