// ===================================================================================
// Basic USART1 Functions for STM32C0xx  (no buffer, no interrupt, no DMA)    * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"

// Init UART
void UART_init(void) {
  // Set GPIO pins
  #if UART_MAP < 2
    // Setup pin PA9 (TX) and pin PA10 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b10<<(9<<1)) | ((uint32_t)0b10<<(10<<1)) );
    GPIOA->OTYPER  &=                  ~  ((uint32_t)0b1 <<(9<<0));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(                            ((uint32_t)0b11<<(10<<1)) ))
                                     |  (                            ((uint32_t)0b01<<(10<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(1<<2)) | ((uint32_t)0xf <<( 2<<2)) ))
                                     |  ( ((uint32_t)0x1 <<(1<<2)) | ((uint32_t)0x1 <<( 2<<2)) );
    #if UART_MAP == 1
      // Remap pins PA11/PA12 to PA9/PA10
      RCC->APBENR2  |= RCC_APBENR2_SYSCFGEN;
      SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_RMP | SYSCFG_CFGR1_PA12_RMP;
    #endif
  #elif UART_MAP == 2
    // Setup pin PB6 (TX) and pin PB7 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER & ~( ((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1)) ))
                                    |  ( ((uint32_t)0b10<<(6<<1)) | ((uint32_t)0b10<<(7<<1)) );
    GPIOB->OTYPER  &=                 ~  ((uint32_t)0b1 <<(6<<0));
    GPIOB->PUPDR    = (GPIOB->PUPDR & ~(                            ((uint32_t)0b11<<(7<<1)) ))
                                    |  (                            ((uint32_t)0b01<<(7<<1)) );
    GPIOB->AFR[0]  &=                 ~( ((uint32_t)0xf <<(6<<2)) | ((uint32_t)0xf <<(7<<2)) );
  #else
    #warning No automatic pin mapping for USART1
  #endif
	
  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  RCC->APBENR2 |= RCC_APBENR2_USART1EN;
  USART1->BRR = ((2 * F_CPU / UART_BAUD) + 1) / 2;
  #if UART_FIFO > 0
  USART1->CR1 = USART_CR1_FIFOEN | USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
  #else
  USART1->CR1 = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;
  #endif
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());
  return USART1->RDR;
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  USART1->TDR = c;
}
