// ===================================================================================
// Basic EEPROM Functions for STM32L01x, STM32L021, STM32L031                 * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "eeprom.h"

// Flash keys
#define FLASH_PEKEY1      ((uint32_t)0x89ABCDEF)
#define FLASH_PEKEY2      ((uint32_t)0x02030405)

// Unlock data EEPROM and FLASH_PECR register
void EEPROM_unlock(void) {
  while(FLASH->SR & FLASH_SR_BSY);        // wait till no operation is on going
  if(FLASH->PECR & FLASH_PECR_PELOCK) {   // check if the PELOCK is unlocked
    FLASH->PEKEYR = FLASH_PEKEY1;         // perform unlock sequence
    FLASH->PEKEYR = FLASH_PEKEY2;
  }
}

// Lock data EEPROM and FLASH_PECR
void EEPROM_lock(void) {
  while(FLASH->SR & FLASH_SR_BSY);        // wait till no operation is on going
  FLASH->PECR |= FLASH_PECR_PELOCK;       // lock the NVM by setting PELOCK
}

// Erase data EEPROM
void EEPROM_erase(uint16_t addr) {
  addr &= 0xfffc;                                               // addr must be 4-byte-aligned
  if(addr <= (DATA_EEPROM_END - DATA_EEPROM_BASE)) {            // valid address?
    EEPROM_unlock();                                            // unlock EEPROM
    FLASH->PECR |= FLASH_PECR_ERASE | FLASH_PECR_DATA;          // enable page erase
    FLASH->SR   |= FLASH_SR_EOP;                                // clear EOP flag
    *(__IO uint32_t *)(DATA_EEPROM_BASE + addr) = (uint32_t)0;  // start erase
    while(!(FLASH->SR & FLASH_SR_EOP));                         // wait till completed
    FLASH->PECR &= ~(FLASH_PECR_ERASE | FLASH_PECR_DATA);       // disable page erase
    EEPROM_lock();                                              // lock EEPROM
  }
}

// Write 32-bit word to EEPROM
void EEPROM_write32(uint16_t addr, uint32_t value) {
  addr &= 0xfffc;                                               // addr must be 4-byte-aligned
  if(addr <= (DATA_EEPROM_END - DATA_EEPROM_BASE)) {            // valid address?
    if(value != *(__IO uint32_t *)(DATA_EEPROM_BASE + addr)) {  // value update needed?
      EEPROM_unlock();                                          // unlock EEPROM
      FLASH->SR |= FLASH_SR_EOP;                                // clear EOP flag
      *(__IO uint32_t *)(DATA_EEPROM_BASE + addr) = value;      // write new value
      while(!(FLASH->SR & FLASH_SR_EOP));                       // wait till completed
      EEPROM_lock();                                            // lock EEPROM
    }
  }
}

// Write 16-bit half-word to EEPROM
void EEPROM_write16(uint16_t addr, uint16_t value) {
  uint32_t page = *(__IO uint32_t *)(DATA_EEPROM_BASE + (addr & 0xfffc)); // read page
  page = (page & ~((uint32_t)0xffff<<((addr&2)<<3)))            // clear hword within page
               |  ((uint32_t)value <<((addr&2)<<3));            // set new value
  EEPROM_write32(addr, page);        
}

// Write 8-bit byte to EEPROM
void EEPROM_write8(uint16_t addr, uint8_t value) {
  uint32_t page = *(__IO uint32_t *)(DATA_EEPROM_BASE + (addr & 0xfffc)); // read page
  page = (page & ~((uint32_t)0xff <<((addr&3)<<3)))             // clear byte within page
               |  ((uint32_t)value<<((addr&3)<<3));             // set new value
  EEPROM_write32(addr, page);                                   // write modified page
}
