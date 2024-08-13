// ===================================================================================
// Basic I2C Master Functions with DMA for PY32F0xx                           * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               Init I2C with defined clock rate (see below)
// I2C_start(addr)          I2C start transmission, addr must contain R/W bit
// I2C_write(b)             I2C transmit one data byte via I2C
// I2C_read(ack)            I2C receive one data byte (set ack=0 for last byte)
// I2C_stop()               I2C stop transmission
//
// I2C_writeBuffer(buf,len) Send buffer (*buf) with length (len) via I2C/DMA and stop
// I2C_readBuffer(buf,len)  Read buffer (*buf) with length (len) via I2C and stop
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C_MAP   0     1     2     3     4     5     6     7
// SDA-pin  PA2   PA7   PA9   PA12  PB7   PB7   PF0   No mapping
// SCL-pin  PA3   PA8   PA10  PA11  PB6   PB8   PF1   No mapping
//
// External pull-up resistors (4k7 - 10k) are mandatory!
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// I2C parameters
#define I2C_CLKRATE       400000  // I2C bus clock rate (Hz)
#define I2C_MAP           6       // I2C pin mapping (see above)
#define I2C_DMA_CHANNEL   1       // DMA channel (1 - 3)

// Interrupt enable check
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

// I2C functions
void I2C_init(void);              // I2C init function
void I2C_start(uint8_t addr);     // I2C start transmission, addr must contain R/W bit
void I2C_stop(void);              // I2C stop transmission
void I2C_write(uint8_t data);     // I2C transmit one data byte via I2C
uint8_t I2C_read(uint8_t ack);    // I2C receive one data byte from the slave

void I2C_writeBuffer(uint8_t* buf, uint16_t len);
void I2C_readBuffer(uint8_t* buf, uint16_t len);

#define I2C_busy()                (I2C1->SR2 & I2C_SR2_BUSY)

// DMA channel defines
#if   I2C_DMA_CHANNEL == 1
  #define I2C_DMA_CHAN   DMA1_Channel1
  #define I2C_DMA_POS    SYSCFG_CFGR3_DMA1_MAP_Pos
  #define I2C_DMA_IRQn   DMA1_Channel1_IRQn
  #define I2C_DMA_ISR    DMA1_Channel1_IRQHandler
  #define I2C_DMA_SHIFT  0
#elif I2C_DMA_CHANNEL == 2
  #define I2C_DMA_CHAN   DMA1_Channel2
  #define I2C_DMA_POS    SYSCFG_CFGR3_DMA2_MAP_Pos
  #define I2C_DMA_IRQn   DMA1_Channel2_3_IRQn
  #define I2C_DMA_ISR    DMA1_Channel2_3_IRQHandler
  #define I2C_DMA_SHIFT  4
#elif I2C_DMA_CHANNEL == 3
  #define I2C_DMA_CHAN   DMA1_Channel3
  #define I2C_DMA_POS    SYSCFG_CFGR3_DMA3_MAP_Pos
  #define I2C_DMA_IRQn   DMA1_Channel2_3_IRQn
  #define I2C_DMA_ISR    DMA1_Channel2_3_IRQHandler
  #define I2C_DMA_SHIFT  8
#endif

#ifdef __cplusplus
};
#endif
