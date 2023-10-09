// ===================================================================================
// Basic I2C Master Functions for PY32F0xx (write only)                       * v0.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// I2C_init()               Init I2C with defined clock rate (see below)
// I2C_start(addr)          I2C start transmission, addr must contain R/W bit
// I2C_write(b)             I2C transmit one data byte via I2C
// I2C_stop()               I2C stop transmission
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

#include "py32f0xx.h"

// I2C Parameters
#define I2C_CLKRATE   400000    // I2C bus clock rate (Hz)
#define I2C_MAP       6         // I2C pin mapping (see above)

// I2C Functions
void I2C_init(void);            // I2C init function
void I2C_start(uint8_t addr);   // I2C start transmission, addr must contain R/W bit
void I2C_write(uint8_t data);   // I2C transmit one data byte via I2C
void I2C_stop(void);            // I2C stop transmission

#ifdef __cplusplus
};
#endif
