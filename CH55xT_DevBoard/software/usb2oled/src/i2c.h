// ===================================================================================
// I2C Functions for CH551, CH552 and CH554                                   * v1.2 *
// ===================================================================================
//
// Simple I2C bitbanging for 400kHz slave devices. For system clock < 12MHz the 
// I2C clock frequency is slower. ACK bit of the slave is ignored. Clock stretching 
// by the slave is not allowed.
//
// PIN_SDA and PIN_SCL must be defined in config.h:
// PIN_SDA - pin connected to serial data of the I2C bus
// PIN_SCL - pin connected to serial clock of the I2C bus
// External pull-up resistors (4k7 - 10k) are mandatory!
//
// Further information:     https://github.com/wagiminator/ATtiny13-TinyOLEDdemo
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>

void I2C_init(void);            // I2C init function
void I2C_start(uint8_t addr);   // I2C start transmission
void I2C_restart(uint8_t addr); // I2C restart transmission
void I2C_stop(void);            // I2C stop transmission
void I2C_write(uint8_t data);   // I2C transmit one data byte to the slave
uint8_t I2C_read(uint8_t ack);  // I2C receive one data byte from the slave

// I2C buffer functions
#define I2C_writeBuffer(buf,len)  do {uint8_t _n = (len); uint8_t* _p = (buf); while(_n--) I2C_write(*_p++); I2C_stop();} while(0)
