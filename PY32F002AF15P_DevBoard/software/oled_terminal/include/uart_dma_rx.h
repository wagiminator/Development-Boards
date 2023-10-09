// ===================================================================================
// UART1 with DMA for PY32F0xx (RX only)                                      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// UART_init()              Init UART with 8N1 and default BAUD rate (115200)
// UART_setBaud(n)          Set BAUD rate
// UART_setDataBits(n)      Set number of data bits (n = 8, 9)
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
//
// USART1 pin mapping (set below in UART parameters):
// --------------------------------------------------
// UART_MAP   0    1    2    3     4     5     6    7    8
// RX-pin    PA3  PA8  PA9  PA10  PA13  PA15  PB2  PB7  PF0  
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "py32f0xx.h"

// UART parameters
#define UART_BAUD             115200    // default UART baud rate
#define UART_RX_BUF_SIZE      64        // UART RX buffer size
#define UART_MAP              0         // UART pin mapping (see above)
#define UART_DMA_CHANNEL      1         // DMA channel (1 - 3)

// UART macros
#define UART_enable()         USART1->CR1 |= USART_CR1_UE         // enable USART
#define UART_disable()        USART1->CR1 &= ~USART_CR1_UE        // disable USART

#define UART_setBAUD(n)       USART1->BRR = ((2*F_CPU/(n))+1)/2   // set BAUD rate
#define UART_setDataBits(n)   (n==9 ? (USART1->CR1 |= USART_CR1_M) : (USART1->CR1 &= ~USART_CR1_M))
#define UART_setStopBits(n)   (n==2 ? (USART1->CR2 |= USART_CR2_STOP) : (USART1->CR2 &= ~USART_CR2_STOP))
#define UART_setEvenParity()  {USART1->CR1 |= USART_CR1_PCE; USART1->CR1 &= ~USART_CR1_PS;}
#define UART_setOddParity()   {USART1->CR1 |= USART_CR1_PCE; USART1->CR1 |=  USART_CR1_PS;}
#define UART_setNoParity()    USART1->CR1 &= ~USART_CR1_PCE

// UART functions
void UART_init(void);         // init UART with default BAUD rate
char UART_read(void);         // read character via UART
uint8_t UART_available(void); // check if there is something to read

// DMA channel defines
#if   UART_DMA_CHANNEL == 1
  #define UART_DMA_CHAN       DMA1_Channel1
  #define UART_DMA_POS        SYSCFG_CFGR3_DMA1_MAP_Pos
#elif UART_DMA_CHANNEL == 2
  #define UART_DMA_CHAN       DMA1_Channel2
  #define UART_DMA_POS        SYSCFG_CFGR3_DMA2_MAP_Pos
#elif UART_DMA_CHANNEL == 3
  #define UART_DMA_CHAN       DMA1_Channel3
  #define UART_DMA_POS        SYSCFG_CFGR3_DMA3_MAP_Pos
#endif

#ifdef __cplusplus
};
#endif
