// ===================================================================================
// UART2 with DMA RX Buffer for CH32V203                                      * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "uart2_dma.h"

// Circular RX buffer
char    UART2_RX_buffer[UART2_RX_BUF_SIZE];
uint8_t UART2_RX_tptr = 0;
#define UART2_RX_hptr (UART2_RX_BUF_SIZE - DMA1_Channel6->CNTR)

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

// Send string via UART
void UART2_print(const char* str) {
  while(*str) UART2_write(*str++);
}

// Send string via UART with newline
void UART2_println(const char* str) {
  UART2_print(str);
  UART2_write('\n');
}

// Print decimal value
void DEBUG_printD(uint32_t value) {
  uint8_t digitval;                               // current digit value
  uint8_t leadflag = 0;                           // flag for leading spaces
  uint32_t divider = 1000000000;                  // current divider
  while(divider) {                                // for all digits
    digitval = value / divider;                   // calculate digit value
    value    = value % divider;                   // set value to division remainder
    divider /= 10;                                // calculate next divider
    if(digitval) leadflag++;                      // end of leading spaces
    if(!divider) leadflag++;                      // least digit has to be printed
    if(leadflag) UART2_write(digitval + '0');     // print the digit
  }
}

// Convert byte nibble into hex character and print it via UART
void UART2_printN(uint8_t nibble) {
  UART2_write((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert byte into hex characters and print it via UART
void UART2_printB(uint8_t value) {
  UART2_printN(value >> 4);
  UART2_printN(value & 0x0f);
}

// Convert word into hex characters and print it via UART
void UART2_printW(uint16_t value) {
  UART2_printB(value >> 8);
  UART2_printB(value);
}

// Convert long into hex characters and print it via UART
void UART2_printL(uint32_t value) {
  UART2_printW(value >> 16);
  UART2_printW(value);
}
