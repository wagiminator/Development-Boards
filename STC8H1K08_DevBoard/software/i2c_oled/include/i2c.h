// ===================================================================================
// Basic I2C Master Functions for STC8H Microcontrollers                      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               I2C init function
// I2C_start(addr)          I2C start transmission, address must contain R/W bit
// I2C_restart(addr)        I2C restart transmission, address must contain R/W bit
// I2C_stop()               I2C stop transmission
// I2C_write(data)          I2C transmit one data byte to the slave
// I2C_read(ack)            I2C receive one data byte (set ack=0 for last byte)
//
// I2C_enable()             enable I2C
// I2C_disable()            disable I2C
//
// I2C pin mapping (set below in I2C parameters):
// ----------------------------------------------
// I2C       MAP0   MAP1   MAP2   MAP3
// SDA-pin   P1.4   P2.4   P7.6   P3.3
// SCL-pin   P1.5   P2.5   P7.7   P3.2
//
// Notes:
// ------
// Access to extended registers must be granted (P_SW2 |= 0x80;).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

// ===================================================================================
// I2C Parameters
// ===================================================================================
#define I2C_RX_ENABLE     0             // 0: transmit only, 1: transmit and receive
#define I2C_MAP           3             // I2C port pin map (0 - 3)
#define I2C_CLKRATE       400000        // I2C bus speed in Hz

// ===================================================================================
// I2C States
// ===================================================================================
#define I2C_busy()        (I2CMSST & 0x80)
#define I2C_ready()       (I2CMSST & 0x40)
#define I2C_getACK()      (I2CMSST & 0x02)

// ===================================================================================
// I2C Commands
// ===================================================================================
#define I2C_CMD_START     0b0001        // send a START signal
#define I2C_CMD_STOP      0b0110        // send a STOP signal
#define I2C_CMD_WRITE     0b0010        // send data byte in I2CTXD register
#define I2C_CMD_READ      0b0100        // read data byte to I2CRXD register
#define I2C_CMD_ACK_READ  0b0011        // read ACK bit to MSACKI (I2CMSST.1)
#define I2C_CMD_ACK_WRITE 0b0101        // send ACK bit in MSACKO (I2CMSST.0)
#define I2C_SEQ_START     0b1001        // START + send data byte + read ACK
#define I2C_SEQ_WRITE     0b1010        // send data byte + read ACK
#define I2C_SEQ_READ      0b1100        // read data byte + send NACK (ACK=1)
#define I2C_SEQ_READLAST  0b1011        // read data byte + send ACK (ACK=0)

// ===================================================================================
// I2C Functions and Macros
// ===================================================================================
#define I2C_MSSPEED       ((F_CPU / I2C_CLKRATE / 2 - 4) / 2)
#define I2C_setSpeed(n)   I2CCFG = 0xc0 | ((F_CPU / (n) / 2 - 4) / 2)
#define I2C_restart(addr) {I2C_stop(); I2C_start(addr);}
#define I2C_enable()      I2CCFG |=  0x80
#define I2C_disable()     I2CCFG &= ~0x80

inline void I2C_init(void) {
  #if   I2C_MAP == 0
    P1M0  &= ~((1<<4) | (1<<5));
    P1M1  &= ~((1<<4) | (1<<5));
  #elif I2C_MAP == 1
    P2M0  &= ~((1<<4) | (1<<5));
    P2M1  &= ~((1<<4) | (1<<5));
    P_SW2 |=  1 << 4;
  #elif I2C_MAP == 2
    P7M0  &= ~((1<<6) | (1<<7));
    P7M1  &= ~((1<<6) | (1<<7));
    P_SW2 |=  2 << 4;
  #elif I2C_MAP == 3
    P3M0  &= ~((1<<2) | (1<<3));
    P3M1  &= ~((1<<2) | (1<<3));
    P_SW2 |=  3 << 4;
  #endif

  I2CCFG   = 0xc0 | I2C_MSSPEED;        // enable I2C with specified speed
  I2CMSAUX = 0x01;                      // enable automatic transmission
  I2CMSST  = 0x00;                      // clear flags
}

void I2C_start(uint8_t addr);
void I2C_stop(void);
void I2C_write(uint8_t data);

#if I2C_RX_ENABLE > 0
  uint8_t I2C_read(uint8_t ack);
#endif
