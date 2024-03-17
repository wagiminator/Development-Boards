// ===================================================================================
// USART1 with DMA RX Buffer (RX only) for STM32C0xx                          * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART_setBaud(n)          Set BAUD rate
// UART_setDataBits(n)      Set number of data bits (n = 7, 8, 9)
// UART_setStopBits(n)      Set number of stop bits (n = 1, 2)
// UART_setNoParity()       Set no parity bit
// UART_setOddParity()      Set parity bit, odd
// UART_setEvenParity()     Set parity bit, even
//
// UART_available()         Check if there is something to read
// UART_read()              Read character via UART
//
// UART_enable()            Enable USART
// UART_disable()           Disable USART
// UART_TX_enable()         Enable transmitter
// UART_TX_disable()        Disable transmitter
// UART_RX_enable()         Enable receiver
// UART_RX_disable()        Disable receiver
// UART_LP_enable()         Enable wake from low-power
// UART_LP_disable()        Disable wake from low-power
// UART_FIFO_enable()       Enable FIFO mode
// UART_FIFO_disable()      Disable FIFO mode
//
// USART1 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART_MAP   0     1     2     3
// RX-pin    PA10  PA12  PB7   No mapping
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// UART parameters
#define UART_BAUD             115200      // default UART baud rate
#define UART_RX_BUF_SIZE      64          // UART RX buffer size
#define UART_MAP              1           // UART pin mapping (see above)
#define UART_DMA_CHANNEL      1           // DMA channel (1 - 3)
#define UART_FIFO             1           // 1: enable 8-byte FIFO

// UART macros
#define UART_enable()         USART1->CR1 |= USART_CR1_UE           // enable USART
#define UART_disable()        USART1->CR1 &= ~USART_CR1_UE          // disable USART
#define UART_TX_enable()      USART1->CR1 |= USART_CR1_TE           // enable transmitter
#define UART_TX_disable()     USART1->CR1 &= ~USART_CR1_TE          // disable transmitter
#define UART_RX_enable()      USART1->CR1 |= USART_CR1_RE           // enable receiver
#define UART_RX_disable()     USART1->CR1 &= ~USART_CR1_RE          // disable receiver
#define UART_LP_enable()      USART1->CR1 |= USART_CR1_UESM         // enable wake from low-power
#define UART_LP_disable()     USART1->CR1 &= ~USART_CR1_UESM        // disable wake from low-power
#define UART_FIFO_enable()    USART1->CR1 |= USART_CR1_FIFOEN       // enable FIFO mode
#define UART_FIFO_disable()   USART1->CR1 |= ~USART_CR1_FIFOEN      // disable FIFO mode

#define UART_setBAUD(n)       USART1->BRR = ((2*F_CPU/(n))+1)/2     // set BAUD rate
#define UART_setDataBits(n)   {USART1->CR1 &= ~(USART_CR1_M1 | USART_CR1_M0); \
                              (n==9 ? (USART1->CR1 |= USART_CR1_M0) :         \
                              (n==7 ? (USART1->CR1 |= USART_CR1_M1) : (0)));  }
#define UART_setStopBits(n)   (n==2 ? (USART1->CR2 |= ((uint32_t)1<<13) : (USART1->CR2 &= ~((uint32_t)1<<13)))
#define UART_setEvenParity()  {USART1->CR1 |= USART_CR1_PCE; USART1->CR1 &= ~USART_CR1_PS;}
#define UART_setOddParity()   {USART1->CR1 |= USART_CR1_PCE; USART1->CR1 |=  USART_CR1_PS;}
#define UART_setNoParity()    USART1->CR1 &= ~USART_CR1_PCE

// UART functions
void UART_init(void);                     // init UART with default BAUD rate
char UART_read(void);                     // read character via UART
uint8_t UART_available(void);             // check if there is something to read

// DMA channel defines
#if   UART_DMA_CHANNEL == 1
  #define UART_DMA_CHAN   DMA1_Channel1
  #define UART_DMA_MUX    DMAMUX1_Channel0
#elif UART_DMA_CHANNEL == 2
  #define UART_DMA_CHAN   DMA1_Channel2
  #define UART_DMA_MUX    DMAMUX1_Channel1
#elif UART_DMA_CHANNEL == 3
  #define UART_DMA_CHAN   DMA1_Channel3
  #define UART_DMA_MUX    DMAMUX1_Channel2
#endif

#ifdef __cplusplus
};
#endif
