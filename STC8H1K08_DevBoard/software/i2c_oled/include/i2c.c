// ===================================================================================
// Basic I2C Master Functions for STC8H Microcontrollers                      * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c.h"

// Wait for last command execution to be completed
void I2C_wait(void) {
  while(!(I2CMSST & 0x40));
  I2CMSST &= ~0x40;
}

// Start transmission, address must contain R/W bit
void I2C_start(uint8_t addr) {
  I2CMSCR = I2C_CMD_START;
  I2C_write(addr);
  #if I2C_RX_ENABLE > 0
    while(!(I2CMSST & 0x40));
  #endif
}

// Stop transmission
void I2C_stop(void) {
  I2C_wait();
  I2CMSCR = I2C_CMD_STOP;
  I2C_wait();
}

// Transmit one data byte to the slave
void I2C_write(uint8_t data) {
  I2C_wait();
  I2CTXD = data;
}

#if I2C_RX_ENABLE > 0
// Receive one data byte (set ack=0 for last byte)
uint8_t I2C_read(uint8_t ack) {
  I2CMSST &= ~0x40;
  I2CMSCR = (ack ? I2C_SEQ_READ : I2C_SEQ_READLAST);
  while(!(I2CMSST & 0x40));
  return I2CRXD;
}
#endif
