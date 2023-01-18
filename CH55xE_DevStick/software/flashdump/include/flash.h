// ===================================================================================
// Data Flash Functions for CH551, CH552 and CH554
// ===================================================================================

#pragma once
#include <stdint.h>
#include "ch554.h"

uint8_t FLASH_read(uint8_t addr);                   // read single byte from data flash
void FLASH_write(uint8_t addr, uint8_t value);      // write single byte to data flash
void FLASH_update(uint8_t addr, uint8_t value);     // write if changed (reduces write cycles)
