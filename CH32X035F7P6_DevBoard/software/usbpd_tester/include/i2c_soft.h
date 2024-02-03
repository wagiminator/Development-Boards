// ===================================================================================
// Software I2C Master Functions for CH32X035/X034/X033                       * v1.0 *
// ===================================================================================
//
// Simple I2C bitbanging. ACK bit of the slave is ignored. Clock stretching by the 
// slave is not allowed. External pull-up resistors (4k7 - 10k) are mandatory!
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
// Define SDA/SCL pin and clock rate below!
//
// Further information:     https://github.com/wagiminator/ATtiny13-TinyOLEDdemo
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "system.h"
#include "gpio.h"

// I2C parameters
#ifndef PIN_SDA
#define PIN_SDA       PA11        // pin connected to serial data of the I2C bus
#define PIN_SCL       PA10        // pin connected to serial clock of the I2C bus
#endif
#define I2C_CLKRATE   600000      // I2C bus clock rate in Hz

// I2C Functions
void I2C_init(void);              // I2C init function
void I2C_start(uint8_t addr);     // I2C start transmission
void I2C_restart(uint8_t addr);   // I2C restart transmission
void I2C_stop(void);              // I2C stop transmission
void I2C_write(uint8_t data);     // I2C transmit one data byte to the slave
uint8_t I2C_read(uint8_t ack);    // I2C receive one data byte from the slave

#ifdef __cplusplus
};
#endif
