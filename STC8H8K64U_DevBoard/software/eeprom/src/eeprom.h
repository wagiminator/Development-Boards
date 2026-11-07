// ===================================================================================
// Basic EEPROM/IAP Functions for STC8H Microcontrollers                      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// EEPROM_read(addr)        Read byte from EEPROM address
// EEPROM_erase(addr)       Erase whole EEPROM sector of address (512 bytes)
// EEPROM_write(addr, val)  Write byte to EEPROM address, must be erased first
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

// TPS wait value according to system frequency
#define IAP_TPS_VAL       ((F_CPU + 500000) / 1000000)

// EEPROM commands
#define IAP_CMD_NOP       0x00
#define IAP_CMD_READ      0x01
#define IAP_CMD_WRITE     0x02
#define IAP_CMD_ERASE     0x03

// EEPROM functions
uint8_t EEPROM_read(uint16_t addr);
void EEPROM_erase(uint16_t addr);
void EEPROM_write(uint16_t addr, uint8_t data);
