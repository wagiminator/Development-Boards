// ===================================================================================
// 24Cxx I2C EEPROM Functions                                                 * v1.1 *
// ===================================================================================
// 2024 by Stefan Wagner:     https://github.com/wagiminator

#include "eeprom_24c.h"

// Start transmission and send word address (internal)
static void EE_setAddr(uint16_t addr) {
  #if EEPROM_ADDR16 > 0
  I2C_start((uint8_t)(EEPROM_ADDR << 1));
  I2C_write(addr >> 8);
  #else
  I2C_start((EEPROM_ADDR << 1) | ((addr >> 7) & 0x0E));  // block bits for 24C04..24C16
  #endif
  I2C_write(addr);
}

// Start reading at address (internal)
static void EE_readStart(uint16_t addr) {
  EE_setAddr(addr);
  I2C_stop();
  #if EEPROM_ADDR16 > 0
  I2C_start((uint8_t)((EEPROM_ADDR << 1) | 1));
  #else
  I2C_start((EEPROM_ADDR << 1) | ((addr >> 7) & 0x0E) | 1);
  #endif
}

// Stop and wait for write cycle (internal)
static void EE_writeStop(void) {
  I2C_stop();
  #if EEPROM_WRITE_TIME > 0
  DLY_ms(EEPROM_WRITE_TIME);
  #endif
}

// Init I2C
void EE_init(void) {
  #if EEPROM_INIT_I2C > 0
  I2C_init();
  #endif
}

// Read single byte from EEPROM
uint8_t EE_read(uint16_t addr) {
  uint8_t result;
  EE_readStart(addr);
  result = I2C_read(0);
  I2C_stop();
  return result;
}

// Write single byte to EEPROM
void EE_write(uint16_t addr, uint8_t value) {
  EE_setAddr(addr);
  I2C_write(value);
  EE_writeStop();
}

// Write single byte to EEPROM if changed (this reduces write cycles)
void EE_update(uint16_t addr, uint8_t value) {
  if(EE_read(addr) != value) EE_write(addr, value);
}

// Read byte stream from EEPROM
void EE_readStream(uint16_t addr, uint8_t* ptr, uint16_t len) {
  if(!len) return;
  EE_readStart(addr);
  while(--len) *ptr++ = I2C_read(1);
  *ptr = I2C_read(0);                             // last byte with NACK
  I2C_stop();
}

// Write byte stream to EEPROM (handles page boundaries)
void EE_writeStream(uint16_t addr, const uint8_t* ptr, uint16_t len) {
  while(len) {
    EE_setAddr(addr);
    do {
      I2C_write(*ptr++);
      addr++; len--;
    } while(len && (addr & (EEPROM_PAGESIZE - 1)));  // until end of data or page
    EE_writeStop();
  }
}
