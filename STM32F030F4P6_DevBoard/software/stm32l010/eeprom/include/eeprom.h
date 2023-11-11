// ===================================================================================
// Basic EEPROM Functions for STM32L01x, STM32L021, STM32L031                 * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// EEPROM_read32(a)         Read 32-bit word (address must be 4-byte-aligned)
// EEPROM_read16(a)         Read 16-bit half-word (address must be 2-byte-aligned)
// EEPROM_read8(a)          Read 8-bit byte
//
// EEPROM_write32(a, w)     Write 32-bit word (address must be 4-byte-aligned)
// EEPROM_write16(a, h)     Write 16-bit half-word (address must be 2-byte-aligned)
// EEPROM_write8(a, b)      Write 8-bit byte
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// EEPROM functions and macros
#define EEPROM_read32(addr) (*(__IO uint32_t *)(DATA_EEPROM_BASE + addr))
#define EEPROM_read16(addr) (*(__IO uint16_t *)(DATA_EEPROM_BASE + addr))
#define EEPROM_read8(addr)  (*(__IO uint8_t *)(DATA_EEPROM_BASE + addr))

void EEPROM_write32(uint16_t addr, uint32_t value);
void EEPROM_write16(uint16_t addr, uint16_t value);
void EEPROM_write8(uint16_t addr, uint8_t value);

#ifdef __cplusplus
};
#endif
