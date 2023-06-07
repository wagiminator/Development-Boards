// ===================================================================================
// UART with DMA (RX only) for CH32V003                                       * v1.0 *
// ===================================================================================

#include "uart_dma_rx.h"

// Circular RX buffer
char UART_RX_buffer[UART_RX_BUF_SIZE];
uint8_t UART_RX_tptr = 0;
#define UART_RX_hptr (UART_RX_BUF_SIZE - DMA1_Channel5->CNTR)

// Init UART
void UART_init(void) {
  #if UART_REMAP == 0
  // Enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;

  // Set pin PD6 (RX) to input, pullup
  GPIOD->CFGLR &= ~(0b1111<<(6<<2));
  GPIOD->CFGLR |=  (0b1000<<(6<<2));
  GPIOD->OUTDR |=  1<<6;
  
  #elif UART_REMAP == 1
  // Remap UART pins, enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;
  AFIO->PCFR1    |= 1<<2;
  
  // Set pin PD1 (RX) to input, pullup
  GPIOD->CFGLR &= ~(0b1111<<(1<<2));
  GPIOD->CFGLR |=  (0b1000<<(1<<2));
  GPIOD->OUTDR |=  1<<1;
  
  #elif UART_REMAP == 2
  // Remap UART pins, enable GPIO port D and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN | RCC_USART1EN;
  AFIO->PCFR1    |= 1<<21;

  // Set pin PD5 (RX) to input, pullup
  GPIOD->CFGLR &= ~(0b1111<<(5<<2));
  GPIOD->CFGLR |=  (0b1000<<(5<<2));
  GPIOD->OUTDR |=  1<<5;
  
  #elif UART_REMAP == 3
  // Remap UART pins, enable GPIO port C and UART
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN | RCC_USART1EN;
  AFIO->PCFR1    |= (1<<21) | (1<<2);

  // Set pin PC1 (RX) to input, pullup
  GPIOC->CFGLR &= ~(0b1111<<(1<<2));
  GPIOC->CFGLR |=  (0b1000<<(1<<2));
  GPIOC->OUTDR |=  1<<1;

  #else
    #warning Wrong UART REMAP
  #endif
	
  // Setup and start UART (8N1, RX, default BAUD rate)
  USART1->BRR    = ((2 * (F_CPU) / (UART_BAUD)) + 1) / 2;
  USART1->CTLR3 |= USART_CTLR3_DMAR;
  USART1->CTLR1  = USART_CTLR1_RE | USART_CTLR1_UE;

  // Setup DMA Channel 5
  RCC->AHBPCENR |= RCC_DMA1EN;
  DMA1_Channel5->CNTR  = (uint16_t)UART_RX_BUF_SIZE;
  DMA1_Channel5->MADDR = (uint32_t)UART_RX_buffer;
  DMA1_Channel5->PADDR = (uint32_t)&USART1->DATAR;
  DMA1_Channel5->CFGR  = DMA_CFGR1_MINC       // increment memory address
                       | DMA_CFGR1_CIRC       // circular mode
                       | DMA_CFGR1_EN;        // enable
}

// Check if something is in the RX buffer
uint8_t UART_available(void) {
  return(UART_RX_hptr != UART_RX_tptr);
}

// Read from UART buffer
char UART_read(void) {
  char result;
  while(!UART_available());
  result = UART_RX_buffer[UART_RX_tptr++];
  if(UART_RX_tptr >= UART_RX_BUF_SIZE) UART_RX_tptr = 0;
  return result;
}
