// ===================================================================================
// USART2 with DMA RX Buffer for STM32G0xx                                    * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart2_dma.h"

// Circular RX buffer
char UART2_RX_buffer[UART2_RX_BUF_SIZE];
uint8_t UART2_RX_tptr = 0;
#define UART2_RX_hptr (UART2_RX_BUF_SIZE - UART2_DMA_CHAN->CNDTR)

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
  USART2->BRR   = ((2 * F_CPU / UART2_BAUD) + 1) / 2;
  USART2->CR3  |= USART_CR3_DMAR;
  USART2->CR1   = USART_CR1_RE | USART_CR1_TE | USART_CR1_UE;

  // Setup DMA
  RCC->AHBENR |= RCC_AHBENR_DMA1EN;                     // enable DMA module clock
  UART2_DMA_MUX->CCR = 52;                              // set USART2 RX as trigger
  UART2_DMA_CHAN->CNDTR = (uint16_t)UART2_RX_BUF_SIZE;  // ring buffer size
  UART2_DMA_CHAN->CMAR  = (uint32_t)UART2_RX_buffer;    // memory address
  UART2_DMA_CHAN->CPAR  = (uint32_t)&USART2->RDR;       // peripheral address
  UART2_DMA_CHAN->CCR   = DMA_CCR_MINC                  // increment memory address
                        | DMA_CCR_CIRC                  // circular mode
                        | DMA_CCR_EN;                   // enable
}

// Check if something is in the RX buffer
uint8_t UART2_available(void) {
  return(UART2_RX_hptr != UART2_RX_tptr);
}

// Read byte via UART
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
  USART2->TDR = c;
}
