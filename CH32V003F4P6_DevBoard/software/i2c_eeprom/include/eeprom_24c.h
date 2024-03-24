// ===================================================================================
// 24Cxx I2C EEPROM Functions                                                 * v1.0 *
// ===================================================================================
//
// Simple 24Cxx I2C EEPROM functions.
//
// Functions available:
// --------------------
// EEPROM_read(a)             read single byte from EEPROM address (a)
// EEPROM_write(a,v)          write single byte (v) to EEPROM address (a)
// EEPROM_update(a,v)         write if changed (reduces write cycles)
// EEPROM_writeStream(a,p,l)  write (l) number of bytes starting at address (a) from pointer (p)
// EEPROM_readStream(a,p,l)   read (l) number of bytes starting at address (a) to pointer (p)
//
// 2024 by Stefan Wagner:     https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"
#include "system.h"

// EEPROM definitions
#define EEPROM_ADDR       0xa0      // EEPROM I2C device address including write bit
#define EEPROM_PAGESIZE   8         // page size of EEPROM

// EEPROM functions
uint8_t EEPROM_read(uint16_t addr);                 // read single byte from EEPROM
void EEPROM_write(uint16_t addr, uint8_t value);    // write single byte to EEPROM
void EEPROM_update(uint16_t addr, uint8_t value);   // write if changed (reduces write cycles)
void EEPROM_writeStream(uint16_t addr, uint8_t* ptr, uint8_t len);
void EEPROM_readStream(uint16_t addr, uint8_t* ptr, uint8_t len);

#ifdef __cplusplus
};
#endif
