// ===================================================================================
// Basic SPI Master Functions for STM32C0xx                                   * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// SPI_init()               Init SPI with defined clock rate (see below)
// SPI_transfer(d)          Transmit and receive one data byte
//
// SPI_busy()               Check if SPI bus is busy
// SPI_ready()              Check if SPI is ready to write
// SPI_available()          Check if something was received
// SPI_enable()             Enable SPI module
// SPI_disable()            Disable SPI module
// SPI_setBAUD(n)           Set BAUD rate (see below)
// SPI_setCPOL(n)           0: SCK low in idle, 1: SCK high in idle
// SPI_setCPHA(n)           Start sampling from 0: first clock edge, 1: second clock edge
//
// SPI pin mapping (set below in SPI parameters):
// ----------------------------------------------
// SPI_MAP    0     1     2     3     4     5
// SCK-pin   PA5   PA1   PA5   PB3   PA5   no mapping
// MISO-pin  PA6   PA6   PA11  PB4   PA6   no mapping
// MOSI-pin  PA7   PA2   PA12  PB5   PB6   no mapping
// (PB3/PB4/PB5 are only available on STM32C031)
//
// Slave select pins (NSS) must be defined and controlled by the application.
// SPI clock rate must be defined below (max. 24MHz in master mode).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// SPI Parameters
#define SPI_PRESC           5     // SPI_CLKRATE = F_CPU / (2 << SPI_PRESC)
#define SPI_MAP             0     // SPI pin mapping (see above)

// SPI Functions and Macros
#define SPI_busy()          (SPI1->SR & SPI_SR_BSY)
#define SPI_ready()         (SPI1->SR & SPI_SR_TXE)
#define SPI_available()     (SPI1->SR & SPI_SR_RXNE)

#define SPI_enable()        (SPI1->CR1 |=  SPI_CR1_SPE)
#define SPI_disable()       (SPI1->CR1 &= ~SPI_CR1_SPE)

// BR, CPOL and CPHA should only be changed while no communication is ongoing
#define SPI_setCPOL(n)      ((n)?(SPI1->CR1|=SPI_CR1_CPOL):(SPI1->CR1&=~SPI_CR1_CPOL))
#define SPI_setCPHA(n)      ((n)?(SPI1->CR1|=SPI_CR1_CPHA):(SPI1->CR1&=~SPI_CR1_CPHA))
#define SPI_setBAUD(n)      (SPI1->CR1 = (SPI1->CR1&~SPI_CR1_BR) | (((n)&7)<<3))

void SPI_init(void);
uint8_t SPI_transfer(uint8_t data);

#ifdef __cplusplus
};
#endif
