// ===================================================================================
// TPI Functions for CH32V003                                                 * v1.0 *
// ===================================================================================
//
// Tiny Programming Interface (TPI) for ATtiny4/5/9/10/20/40/102/104.
// TPI DATA is connected to MOSI, TPI CLK to SCK and RESET to the ISP RESET pin.
// Based on the implementation by Slawomir Fras for the USBasp.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "system.h"
#include "gpio.h"
#include "config.h"

// TPI instructions
#define TPI_OP_SLD              0x20
#define TPI_OP_SLD_INC          0x24
#define TPI_OP_SST              0x60
#define TPI_OP_SST_INC          0x64
#define TPI_OP_SSTPR(a)         (0x68 | (a))
#define TPI_OP_SIN(a)           (0x10 | (((a)<<1)&0x60) | ((a)&0x0F) )
#define TPI_OP_SOUT(a)          (0x90 | (((a)<<1)&0x60) | ((a)&0x0F) )
#define TPI_OP_SLDCS(a)         (0x80 | ((a)&0x0F) )
#define TPI_OP_SSTCS(a)         (0xC0 | ((a)&0x0F) )
#define TPI_OP_SKEY             0xE0

// TPI control/status registers
#define TPIIR                   0xF
#define TPIPCR                  0x2
#define TPISR                   0x0

// TPISR bits
#define TPISR_NVMEN             0x02

// NVM registers
#define NVMCSR                  0x32
#define NVMCMD                  0x33

// NVMCSR bits
#define NVMCSR_BSY              0x80

// NVMCMD values
#define NVMCMD_NOP              0x00
#define NVMCMD_CHIP_ERASE       0x10
#define NVMCMD_SECTION_ERASE    0x14
#define NVMCMD_WORD_WRITE       0x1D

// Globals
extern uint16_t TPI_dly_cnt;            // TPI clock delay (set by host, 1 count = 1/3us)

// Functions
void TPI_connect(void);                 // connect TPI bus
void TPI_disconnect(void);              // disconnect TPI bus
void TPI_writeByte(uint8_t b);          // send raw byte via TPI
uint8_t TPI_readByte(void);             // receive raw byte via TPI
void TPI_readBlock(uint16_t addr, uint8_t* buf, uint8_t len);   // read block
void TPI_writeBlock(uint16_t addr, uint8_t* buf, uint8_t len);  // write block
