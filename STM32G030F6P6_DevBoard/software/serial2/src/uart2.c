// ===================================================================================
// Basic USART2 Functions for STM32G0xx  (no buffer, no interrupt, no DMA)    * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart2.h"

// Init UART
void UART2_init(void) {
  // Set GPIO pins
  #if UART2_MAP == 0
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
  #elif UART2_MAP == 1
    // Setup pin PA14 (TX) and pin PA15 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(14<<1)) | ((uint32_t)0b11<<(15<<1)) ))
                                     |  ( ((uint32_t)0b10<<(14<<1)) | ((uint32_t)0b10<<(15<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(14<<0));
    GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(14<<1));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                             ((uint32_t)0b11<<(15<<1)) ))
                                     |  (                             ((uint32_t)0b01<<(15<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<( 6<<2)) | ((uint32_t)0xf <<( 7<<2)) ))
                                     |  ( ((uint32_t)0x1 <<( 6<<2)) | ((uint32_t)0x1 <<( 7<<2)) );
  #else
    #warning No automatic pin mapping for USART2
  #endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  RCC->APBENR1 |= RCC_APBENR1_USART2EN;
  USART2->BRR = ((2 * F_CPU / UART2_BAUD) + 1) / 2;
  USART2->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
}

// Read byte via UART
char UART2_read(void) {
  while(!UART2_available());
  return USART2->RDR;
}

// Send byte via UART
void UART2_write(const char c) {
  while(!UART2_ready());
  USART2->TDR = c;
}
