// ===================================================================================
// 24Cxx I2C EEPROM Functions                                                 * v1.0 *
// ===================================================================================
//
// Simple 24Cxx I2C EEPROM functions.
//
// 2024 by Stefan Wagner: https://github.com/wagiminator

#include "eeprom_24c.h"

// Write single byte to EEPROM
void EEPROM_write(uint16_t addr, uint8_t value) {
  I2C_start(EEPROM_ADDR | ((addr >> 7) & 0x0e));
  I2C_write(addr);
  I2C_write(value);
  I2C_stop();
  DLY_ms(5);
}

// Read single byte from EEPROM
uint8_t EEPROM_read(uint16_t addr) {
  uint8_t result;
  I2C_start(EEPROM_ADDR | ((addr >> 7) & 0x0e));
  I2C_write(addr);
  I2C_stop();
  I2C_start(EEPROM_ADDR | ((addr >> 7) & 0x0e) | 1);
  result = I2C_read(0);
  I2C_stop();
  return result;
}

// Write single byte to data flash if changed (this reduces write cycles)
void EEPROM_update(uint16_t addr, uint8_t value) {
  if(EEPROM_read(addr) != value) EEPROM_write(addr, value);
}

// Write byte stream to page
void EEPROM_writeStream(uint16_t addr, uint8_t* ptr, uint8_t len) {
  if(len > EEPROM_PAGESIZE) len = EEPROM_PAGESIZE;
  I2C_start(EEPROM_ADDR | ((addr >> 7) & 0x0e));
  I2C_write(addr);
  while(len--) I2C_write(*ptr++);
  I2C_stop();
  DLY_ms(5);
}

// Read byte stream from EEPROM
void EEPROM_readStream(uint16_t addr, uint8_t* ptr, uint8_t len) {
  I2C_start(EEPROM_ADDR | ((addr >> 7) & 0x0e));
  I2C_write(addr);
  I2C_stop();
  I2C_start(EEPROM_ADDR | ((addr >> 7) & 0x0e) | 1);
  while(len--) *ptr++ = I2C_read(len);
  I2C_stop();
}
