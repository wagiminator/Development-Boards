// ===================================================================================
// USART2 with DMA RX Buffer for CH32X035/X034/X033                           * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart2_dma.h"

// Circular RX buffer
char UART2_RX_buffer[UART2_RX_BUF_SIZE];
uint8_t UART2_RX_tptr = 0;
#define UART2_RX_hptr (UART2_RX_BUF_SIZE - DMA1_Channel6->CNTR)

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
  USART2->CTLR3  |= USART_CTLR3_DMAR;
  USART2->CTLR1   = USART_CTLR1_RE | USART_CTLR1_TE | USART_CTLR1_UE;

  // Setup DMA Channel 6
  RCC->AHBPCENR |= RCC_DMA1EN;
  DMA1_Channel6->CNTR  = (uint16_t)UART2_RX_BUF_SIZE;
  DMA1_Channel6->MADDR = (uint32_t)UART2_RX_buffer;
  DMA1_Channel6->PADDR = (uint32_t)&USART2->DATAR;
  DMA1_Channel6->CFGR  = DMA_CFGR1_MINC       // increment memory address
                       | DMA_CFGR1_CIRC       // circular mode
                       | DMA_CFGR1_EN;        // enable
}

// Check if something is in the RX buffer
uint8_t UART2_available(void) {
  return(UART2_RX_hptr != UART2_RX_tptr);
}

// Read from UART buffer
char UART2_read(void) {
  char result;
  while(!UART2_available());
  result = UART2_RX_buffer[UART2_RX_tptr++];
  if(UART2_RX_tptr >= UART2_RX_BUF_SIZE) UART2_RX_tptr = 0;
  return result;
}

// Send byte via UART
void UART2_write(const char c) {
  while(!UART2_ready());
  USART2->DATAR = c;
}
