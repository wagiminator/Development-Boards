// ===================================================================================
// Basic SPI Master Functions for STC8H Microcontrollers                      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SPI_init()               Init SPI with defined clock rate (see below)
// SPI_transfer(d)          Transmit and receive one data byte
//
// SPI_ready()              Check if SPI is ready for transfer
// SPI_enable()             Enable SPI module
// SPI_disable()            Disable SPI module
// SPI_MSB_first()          The MSB of the data byte is transmitted firstly
// SPI_LSB_first()          The MSB of the data byte is transmitted firstly
// SPI_setPRESC(n)          Set SPI clock prescaler (see below)
// SPI_setCPOL(n)           0: SCK low in idle, 1: SCK high in idle
// SPI_setCPHA(n)           Start sampling from 0: first clock edge, 1: second clock edge
//
// SPI pin mapping (set below in SPI parameters):
// ----------------------------------------------
// SPI        MAP0   MAP1   MAP2   MAP3
// MOSI-pin   P1.3   P2.3   P4.0   P3.4
// MISO-pin   P1.4   P2.4   P4.1   P3.3
// SCK-pin    P1.5   P2.5   P4.3   P3.2
//
// Notes:
// ------
// Slave select pins (NSS) must be defined and controlled by the application.
// SPI clock rate must be defined below.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

// ===================================================================================
// SPI Parameters
// ===================================================================================
#define SPI_MAP           0             // SPI port pin map (0 - 3)
#define SPI_PRESC         0             // SPI clock divider (0:4, 1:8, 2:16, 3:32/2)

// ===================================================================================
// SPI Functions and Macros
// ===================================================================================
#define SPI_ready()       (SPSTAT & 0x80)

#define SPI_enable()      SPCTL |=  0x40
#define SPI_disable()     SPCTL &= ~0x40
#define SPI_LSB_first()   SPCTL |=  0x20
#define SPI_MSB_first()   SPCTL &= ~0x20
#define SPI_setCPOL(n)    (n)?(SPCTL |= 0x08):(SPCTL &= ~0x08)
#define SPI_setCPHA(n)    (n)?(SPCTL |= 0x04):(SPCTL &= ~0x04)
#define SPI_setPRESC(n)   SPCTL = (SPCTL & 0xfc) | ((n) & 3)

// SPI init
inline void SPI_init(void) {
  #if SPI_MAP > 0
    P_SW1 |= SPI_MAP << 2;              // set SPI pin mapping
  #endif
  #if SPI_MAP == 0
    P1M0  &= ~((1<<3) | (1<<4) | (1<<5));
    P1M1  &= ~((1<<3) | (1<<4) | (1<<5));
  #elif SPI_MAP == 1
    P2M0  &= ~((1<<3) | (1<<4) | (1<<5));
    P2M1  &= ~((1<<3) | (1<<4) | (1<<5));
  #elif SPI_MAP == 2
    P4M0  &= ~((1<<0) | (1<<1) | (1<<3));
    P4M1  &= ~((1<<0) | (1<<1) | (1<<3));
  #elif SPI_MAP == 3
    P3M0  &= ~((1<<2) | (1<<3) | (1<<4));
    P3M1  &= ~((1<<2) | (1<<3) | (1<<4));
  #endif

  SPCTL  = 0xd0 | SPI_PRESC;            // enable SPI master mode
  SPSTAT = 0xc0;                        // clear interrupt flag
}

// SPI transmit and receive a byte
inline uint8_t SPI_transfer(uint8_t data) {
  SPSTAT = 0xc0;                        // clear interrupt flag
  SPDAT  = data;                        // start exchanging data byte
  while(!(SPSTAT & 0x80));              // wait for transfer to complete
  return SPDAT;                         // return received byte
}
