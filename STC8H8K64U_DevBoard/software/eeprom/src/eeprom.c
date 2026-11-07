// ===================================================================================
// Basic EEPROM/IAP Functions for STC8H Microcontrollers                      * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "eeprom.h"

// Execute IAP command
void IAP_execute(uint16_t addr) {
  IAP_CONTR = 0x80;               // enable IAP
  IAP_TPS   = IAP_TPS_VAL;        // set the erasing wait parameter
  IAP_ADDRL = addr & 0xff;        // set IAP low address
  IAP_ADDRH = addr >> 8;          // set IAP high address
  IAP_TRIG  = 0x5a;               // write trigger command (0x5a)
  IAP_TRIG  = 0xa5;               // write trigger command (0xa5)
  NOP();
  IAP_CONTR = 0;                  // disable IAP function
  IAP_CMD   = 0;                  // clear command register
  IAP_TRIG  = 0;                  // clear trigger register
}

// Read byte from EEPROM
uint8_t EEPROM_read(uint16_t addr) {
  IAP_CMD = IAP_CMD_READ;         // set IAP read command
  IAP_execute(addr);              // execute read command
  return IAP_DATA;                // read and return data byte
}

// Write byte to EEPROM
void EEPROM_write(uint16_t addr, uint8_t data) {
  IAP_CMD  = IAP_CMD_WRITE;       // set IAP write command
  IAP_DATA = data;                // set data byte to write
  IAP_execute(addr);              // execute write command
}

// Erase EEPROM sector (512 bytes)
void EEPROM_erase(uint16_t addr) {
  IAP_CMD = IAP_CMD_ERASE;        // set IAP erase command
  IAP_execute(addr);              // execute erase command
}
