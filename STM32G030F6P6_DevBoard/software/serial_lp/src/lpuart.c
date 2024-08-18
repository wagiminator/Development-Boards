// ===================================================================================
// Basic LPUART1 Functions for STM32G0xx  (no buffer, no interrupt, no DMA)   * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "lpuart.h"

// Init UART
void LPUART_init(void) {
  // Set GPIO pins
  #if LPUART_MAP == 0
    // Setup pin PA2 (TX) and pin PA3 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(2<<1)) | ((uint32_t)0b11<<(3<<1)) ))
                                     |  ( ((uint32_t)0b10<<(2<<1)) | ((uint32_t)0b10<<(3<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(2<<0));
    GPIOA->OSPEEDR |=                     ((uint32_t)0b11<<(2<<1));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(3<<1)) ))
                                     |  (                            ((uint32_t)0b01<<(3<<1)) );
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)0xf <<(2<<2)) | ((uint32_t)0xf <<(3<<2)) ))
                                     |  ( ((uint32_t)0x6 <<(2<<2)) | ((uint32_t)0x6 <<(3<<2)) );
  #elif LPUART_MAP == 1
    // Setup pin PB11 (TX) and pin PB10 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(11<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b10<<(11<<1)) | ((uint32_t)0b10<<(10<<1)) );
    GPIOB->OTYPER  &=                  ~  ((uint32_t)0b1 <<(11<<0));
    GPIOB->OSPEEDR |=                     ((uint32_t)0b11<<(11<<1));
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~(                             ((uint32_t)0b11<<(10<<1)) ))
                                     |  (                             ((uint32_t)0b01<<(10<<1)) );
    GPIOB->AFR[1]   = (GPIOB->AFR[1] & ~( ((uint32_t)0xf <<( 3<<2)) | ((uint32_t)0xf <<( 2<<2)) ))
                                     |  ( ((uint32_t)0x1 <<( 3<<2)) | ((uint32_t)0x1 <<( 2<<2)) );
  #else
    #warning No automatic pin mapping for LPUART1
  #endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  RCC->APBENR1 |= RCC_APBENR1_LPUART1EN;

  #if LPUART_LSE > 0
    RCC->BDCR |= RCC_BDCR_LSEON;
    while(!(RCC->BDCR & RCC_BDCR_LSERDY));
    RCC->CCIPR |= RCC_CCIPR_LPUART1SEL;
    LPUART1->BRR = (((uint64_t)512 * LSE_VALUE / LPUART_BAUD) + 1) / 2;
  #else
    LPUART1->BRR = (((uint64_t)512 * F_CPU / LPUART_BAUD) + 1) / 2;
  #endif

  #if LPUART_FIFO > 0
    LPUART1->CR1 = USART_CR1_FIFOEN | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
  #else
    LPUART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
  #endif
}

// Read byte via UART
char LPUART_read(void) {
  while(!LPUART_available());
  return LPUART1->RDR;
}

// Send byte via UART
void LPUART_write(const char c) {
  while(!LPUART_ready());
  LPUART1->TDR = c;
}
