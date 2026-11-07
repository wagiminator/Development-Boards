// ===================================================================================
// 24Cxx I2C EEPROM Functions                                                 * v1.1 *
// ===================================================================================
//
// Simple 24Cxx I2C EEPROM (and FRAM) functions.
//
// Functions available:
// --------------------
// EE_init()              init I2C (if EEPROM_INIT_I2C is set)
// EE_read(a)             read single byte from EEPROM address (a)
// EE_write(a,v)          write single byte (v) to EEPROM address (a)
// EE_update(a,v)         write if changed (reduces write cycles)
// EE_readStream(a,p,l)   read (l) number of bytes starting at address (a) to pointer (p)
// EE_writeStream(a,p,l)  write (l) number of bytes from pointer (p) starting at
//                        address (a), page boundaries are handled automatically
//
// Supported devices (set parameters below accordingly):
// -----------------------------------------------------
// Device             EEPROM_ADDR16   EEPROM_PAGESIZE   EEPROM_WRITE_TIME
// 24C01, 24C02       0               8                 5
// 24C04 - 24C16      0               16                5
// 24C32, 24C64       1               32                5
// 24C128, 24C256     1               64                5
// 24C512             1               128               5
// FM24Cxx, MB85RCxx  same as EEPROMs of the same size  0
//
// Notes:
// ------
// - The prefix EE_ avoids conflicts with the internal EEPROM functions (EEPROM_xxx)
//   of some MCU templates.
// - For 24C04 to 24C16 the upper address bits are sent within the device address,
//   the address pins A0..A2 of these devices are not used.
//
// 2024 by Stefan Wagner:     https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                    // choose your I2C library
#include "system.h"
#ifdef __SDCC
#include "delay.h"
#endif

// EEPROM parameters
#define EEPROM_INIT_I2C   1         // 1: init I2C with EE_init()
#define EEPROM_ADDR       0x50      // EEPROM I2C device address (A0..A2 = 0)
#define EEPROM_ADDR16     0         // 0: 8-bit word address, 1: 16-bit word address
#define EEPROM_PAGESIZE   8         // page size of EEPROM (power of 2)
#define EEPROM_WRITE_TIME 5         // write cycle time in ms (0 for FRAM)

// EEPROM functions
void EE_init(void);                                 // init I2C
uint8_t EE_read(uint16_t addr);                     // read single byte from EEPROM
void EE_write(uint16_t addr, uint8_t value);        // write single byte to EEPROM
void EE_update(uint16_t addr, uint8_t value);       // write if changed
void EE_readStream(uint16_t addr, uint8_t* ptr, uint16_t len);
void EE_writeStream(uint16_t addr, const uint8_t* ptr, uint16_t len);

#ifdef __cplusplus
}
#endif
