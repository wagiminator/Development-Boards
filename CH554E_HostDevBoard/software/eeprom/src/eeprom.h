// ===================================================================================
// Data Flash ("EEPROM") Functions for CH551, CH552 and CH554                 * v1.0 *
// ===================================================================================
//
// 2022 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

uint8_t EEPROM_read(uint8_t addr);                // read single byte from data flash
void EEPROM_write(uint8_t addr, uint8_t value);   // write single byte to data flash
void EEPROM_update(uint8_t addr, uint8_t value);  // write if changed (reduces write cycles)
