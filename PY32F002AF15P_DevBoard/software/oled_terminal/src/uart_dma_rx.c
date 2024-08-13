// ===================================================================================
// UART1 with DMA (RX only) for PY32F0xx                                      * v1.0 *
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
  #if UART_MAP == 0
    // Setup pin PA3 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(3<<1)) ))
                                     |  ( ((uint32_t)0b10<<(3<<1)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(3<<1)) ))
                                     |  ( ((uint32_t)0b01<<(3<<1)) );
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)15  <<(3<<2)) ))
                                     |  ( ((uint32_t)1   <<(3<<2)) );
  #elif UART_MAP == 1
    // Setup pin PA8 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(8<<1)) ))
                                     |  ( ((uint32_t)0b10<<(8<<1)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(8<<1)) ))
                                     |  ( ((uint32_t)0b01<<(8<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)15  <<(0<<2)) ))
                                     |  ( ((uint32_t)8   <<(0<<2)) );
  #elif UART_MAP == 2
    // Setup pin PA9 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(9<<1)) ))
                                     |  ( ((uint32_t)0b10<<(9<<1)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(9<<1)) ))
                                     |  ( ((uint32_t)0b01<<(9<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)15  <<(1<<2)) ))
                                     |  ( ((uint32_t)8   <<(1<<2)) );
  #elif UART_MAP == 3
    // Setup pin PA10 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b10<<(10<<1)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b01<<(10<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)15  <<(2<<2)) ))
                                     |  ( ((uint32_t)1   <<(2<<2)) );
  #elif UART_MAP == 4
    // Setup pin PA13 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(13<<1)) ))
                                     |  ( ((uint32_t)0b10<<(13<<1)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(13<<1)) ))
                                     |  ( ((uint32_t)0b01<<(13<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)15  <<(5<<2)) ))
                                     |  ( ((uint32_t)8   <<(5<<2)) );
  #elif UART_MAP == 5
    // Setup pin PA15 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(15<<1)) ))
                                     |  ( ((uint32_t)0b10<<(15<<1)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(15<<1)) ))
                                     |  ( ((uint32_t)0b01<<(15<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)15  <<(7<<2)) ))
                                     |  ( ((uint32_t)1   <<(7<<2)) );
  #elif UART_MAP == 6
    // Setup pin PB2 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(2<<1)) ))
                                     |  ( ((uint32_t)0b10<<(2<<1)) );
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~( ((uint32_t)0b11<<(2<<1)) ))
                                     |  ( ((uint32_t)0b01<<(2<<1)) );
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~( ((uint32_t)15  <<(2<<2)) ))
                                     |  ( ((uint32_t)0   <<(2<<2)) );
  #elif UART_MAP == 7
    // Setup pin PB7 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(7<<1)) ))
                                     |  ( ((uint32_t)0b10<<(7<<1)) );
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~( ((uint32_t)0b11<<(7<<1)) ))
                                     |  ( ((uint32_t)0b01<<(7<<1)) );
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~( ((uint32_t)15  <<(7<<2)) ))
                                     |  ( ((uint32_t)0   <<(7<<2)) );
  #elif UART_MAP == 8
    // Setup pin PF0 (RX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOFEN;
    GPIOF->MODER    = (GPIOF->MODER  & ~( ((uint32_t)0b11<<(0<<1)) ))
                                     |  ( ((uint32_t)0b10<<(0<<1)) );
    GPIOF->PUPDR    = (GPIOF->PUPDR  & ~( ((uint32_t)0b11<<(0<<1)) ))
                                     |  ( ((uint32_t)0b01<<(0<<1)) );
    GPIOF->AFR[0]   = (GPIOF->AFR[0] & ~( ((uint32_t)15  <<(0<<2)) ))
                                     |  ( ((uint32_t)8   <<(0<<2)) );
  #else
    #warning No automatic pin mapping for USART1
  #endif
	
  // Setup and start UART (8N1, RX, default BAUD rate)
  RCC->APBENR2 |= RCC_APBENR2_USART1EN;
  USART1->BRR   = ((2 * F_CPU / UART_BAUD) + 1) / 2;
  USART1->CR3  |= USART_CR3_DMAR;
  USART1->CR1   = USART_CR1_RE | USART_CR1_UE;

  // Setup DMA
  RCC->AHBENR   |= RCC_AHBENR_DMAEN;                  // enable DMA module clock
  RCC->APBENR2  |= RCC_APBENR2_SYSCFGEN;              // enable SYSCFG module clock
  SYSCFG->CFGR3 |= 0b00110 << UART_DMA_POS;           // set USART1 RX as trigger
  UART_DMA_CHAN->CNDTR = (uint16_t)UART_RX_BUF_SIZE;  // ring buffer size
  UART_DMA_CHAN->CMAR  = (uint32_t)UART_RX_buffer;    // memory address
  UART_DMA_CHAN->CPAR  = (uint32_t)&USART1->DR;       // peripheral address
  UART_DMA_CHAN->CCR   = DMA_CCR_MINC                 // increment memory address
                       | DMA_CCR_CIRC                 // circular mode
                       | DMA_CCR_EN;                  // enable
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
