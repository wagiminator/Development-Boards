// ===================================================================================
// Basic SPI Master Functions (TX only) for CH32X033/034/035                  * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SPI_init()               Init SPI with defined clock rate (see below)
// SPI_write(d)             Transmit one data byte
//
// SPI_busy()               Check if SPI bus is busy
// SPI_ready()              Check if SPI is ready to write
// SPI_enable()             Enable SPI module
// SPI_disable()            Disable SPI module
// SPI_setBAUD(n)           Set BAUD rate (see below)
// SPI_setCPOL(n)           0: SCK low in idle, 1: SCK high in idle
// SPI_setCPHA(n)           Start sampling from 0: first clock edge, 1: second clock edge

// SPI pin mapping:
// ----------------
// SCK-pin   PA5
// MOSI-pin  PA7
//
// Slave select pins (NSS) must be defined and controlled by the application.
// SPI clock rate must be defined below.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// SPI Parameters
#define SPI_PRESC           0     // SPI_CLKRATE = F_CPU / (2 << SPI_PRESC)

// I2C Functions and Macros
#define SPI_busy()          (SPI1->STATR & SPI_STATR_BSY)
#define SPI_ready()         (SPI1->STATR & SPI_STATR_TXE)

#define SPI_enable()        SPI1->CTLR1 |=  SPI_CTLR1_SPE
#define SPI_disable()       SPI1->CTLR1 &= ~SPI_CTLR1_SPE
#define SPI_setCPOL(n)      (n)?(SPI1->CTLR1|=SPI_CTLR1_CPOL):(SPI1->CTLR1&=~SPI_CTLR1_CPOL)
#define SPI_setCPHA(n)      (n)?(SPI1->CTLR1|=SPI_CTLR1_CPHA):(SPI1->CTLR1&=~SPI_CTLR1_CPHA)
#define SPI_setBAUD(n)      SPI1->CTLR1 = (SPI1->CTLR1&~SPI_CTLR1_BR) | (((n)&7)<<3)

void SPI_init(void);
void SPI_write(uint8_t data);

#ifdef __cplusplus
};
#endif
