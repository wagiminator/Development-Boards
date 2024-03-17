// ===================================================================================
// USART1 with DMA RX Buffer (RX only) for STM32C0xx                          * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart_dma_rx.h"

// Circular RX buffer
char UART_RX_buffer[UART_RX_BUF_SIZE];
uint8_t UART_RX_tptr = 0;
#define UART_RX_hptr (UART_RX_BUF_SIZE - UART_DMA_CHAN->CNDTR)

// Init UART
void UART_init(void) {
  // Set GPIO pins
  #if UART_MAP < 2
    // Setup pin PA10 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(10<<1))) | ((uint32_t)0b10<<(10<<1));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(10<<1))) | ((uint32_t)0b01<<(10<<1));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~((uint32_t)0xf <<( 2<<2))) | ((uint32_t)0x1 <<( 2<<2));
    #if UART_MAP == 1
      // Remap pin PA12 to PA10
      RCC->APBENR2  |= RCC_APBENR2_SYSCFGEN;
      SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA12_RMP;
    #endif
  #elif UART_MAP == 2
    // Setup pin PB7 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER & ~((uint32_t)0b11<<(7<<1))) | ((uint32_t)0b10<<(7<<1));
    GPIOB->PUPDR    = (GPIOB->PUPDR & ~((uint32_t)0b11<<(7<<1))) | ((uint32_t)0b01<<(7<<1));
    GPIOB->AFR[0]  &= ((uint32_t)0xf <<(7<<2));
  #else
    #warning No automatic pin mapping for USART1
  #endif
	
  // Setup and start UART (8N1, RX, default BAUD rate)
  RCC->APBENR2 |= RCC_APBENR2_USART1EN;
  USART1->BRR   = ((2 * F_CPU / UART_BAUD) + 1) / 2;
  USART1->CR3  |= USART_CR3_DMAR;
  #if UART_FIFO > 0
  USART1->CR1 = USART_CR1_FIFOEN | USART_CR1_RE | USART_CR1_UE;
  #else
  USART1->CR1 = USART_CR1_RE | USART_CR1_UE;
  #endif

  // Setup DMA
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;                     // enable DMA module clock
  UART_DMA_MUX->CCR = 50;                               // set USART1 RX as trigger
  UART_DMA_CHAN->CNDTR = (uint16_t)UART_RX_BUF_SIZE;    // ring buffer size
  UART_DMA_CHAN->CMAR  = (uint32_t)UART_RX_buffer;      // memory address
  UART_DMA_CHAN->CPAR  = (uint32_t)&USART1->RDR;        // peripheral address
  UART_DMA_CHAN->CCR   = DMA_CCR_MINC                   // increment memory address
                       | DMA_CCR_CIRC                   // circular mode
                       | DMA_CCR_EN;                    // enable
}

// Check if something is in the RX buffer
uint8_t UART_available(void) {
  return(UART_RX_hptr != UART_RX_tptr);
}

// Read byte via UART
char UART_read(void) {
  char result;
  while(!UART_available());
  result = UART_RX_buffer[UART_RX_tptr++];
  if(UART_RX_tptr >= UART_RX_BUF_SIZE) UART_RX_tptr = 0;
  return result;
}
