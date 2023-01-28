// ===================================================================================
// Data Flash Functions for CH551, CH552 and CH554
// ===================================================================================

#include "flash.h"

// Write single byte to data flash
void FLASH_write(uint8_t addr, uint8_t value) {
  if(addr < 128) {                      // max addr
    SAFE_MOD    = 0x55;
    SAFE_MOD    = 0xAA;                 // enter safe mode
    GLOBAL_CFG |= bDATA_WE;             // enable data flash write
    SAFE_MOD    = 0;                    // exit safe mode
    ROM_ADDR_H  = DATA_FLASH_ADDR >> 8; // set address high byte
    ROM_ADDR_L  = addr << 1;            // set address low byte (must be even)
    ROM_DATA_L  = value;                // set value
    if(ROM_STATUS & bROM_ADDR_OK)       // valid access address?
      ROM_CTRL  = ROM_CMD_WRITE;        // write value to data flash
    SAFE_MOD    = 0x55;
    SAFE_MOD    = 0xAA;                 // enter safe mode
    GLOBAL_CFG &= ~bDATA_WE;            // disable data flash write
    SAFE_MOD    = 0;                    // exit safe mode
  }
}

// Read single byte from data flash
uint8_t FLASH_read(uint8_t addr) {
  ROM_ADDR_H = DATA_FLASH_ADDR >> 8;    // set address high byte
  ROM_ADDR_L = addr << 1;               // set address low byte (must be even)
  ROM_CTRL   = ROM_CMD_READ;            // read value from data flash
  return ROM_DATA_L;                    // return value
}

// Write single byte to data flash if changed (this reduces write cycles)
void FLASH_update(uint8_t addr, uint8_t value) {
  if(FLASH_read(addr) != value) FLASH_write(addr, value);
}
