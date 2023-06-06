// ===================================================================================
// Basic FLASH Functions for CH32V003                                         * v1.0 *
// ===================================================================================

#include "flash.h"

#define FLASH_KEY1          0x45670123
#define FLASH_KEY2          0xCDEF89AB
#define FLASH_RDPRT         0x00A5

#define FLASH_OB_BASE       0b11100010

#define FLASH_busy()        (FLASH->STATR & FLASH_STATR_BSY)
#define FLASH_error()       (FLASH->STATR & FLASH_STATR_WRPRTERR)
#define FLASH_eop()         (FLASH->STATR & FLASH_STATR_EOP)
#define FLASH_locked()      (FLASH->CTLR  & FLASH_CTLR_LOCK)
#define FLASH_OB_locked()   (!(FLASH->CTLR & FLASH_CTLR_OPTWRE))

#define FLASH_WRE_unlock() {  \
  FLASH->OBKEYR = FLASH_KEY1; \
  FLASH->OBKEYR = FLASH_KEY2; \
}

// Write 16-bit data to FLASH addr
void FLASH_write(uint32_t addr, uint16_t data) {
  FLASH->CTLR |= FLASH_CTLR_PG;
  *(__IO uint16_t *)addr = data;
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_PG;
}

// Remove option bytes write protection
void FLASH_OB_unlock(void) {
  FLASH_unlock();
  FLASH_WRE_unlock();
}

// Erase option bytes and remove read protection
void FLASH_OB_erase(void) {
  FLASH_OB_unlock();
  FLASH->CTLR |=  FLASH_CTLR_OPTER;
  FLASH->CTLR |=  FLASH_CTLR_STRT;
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTER;
  FLASH->CTLR |=  FLASH_CTLR_OPTPG;
  OB->RDPR = FLASH_RDPRT;
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTPG;
  FLASH_lock();
}

// Write option byte to addr
void FLASH_OB_write(uint32_t addr, uint8_t data) {
  FLASH->CTLR |= FLASH_CTLR_OPTPG;
  *(__IO uint16_t *)addr = data;
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTPG;
}

// Write option bytes user flags
void FLASH_OB_USER_write(uint8_t flags) {
  FLASH_OB_unlock();
  FLASH->CTLR |= FLASH_CTLR_OPTPG;
  OB->USER = (uint16_t)(FLASH_OB_BASE | flags);
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTPG;
  FLASH_lock();
}

// Write option bytes user data
void FLASH_OB_DATA_write(uint16_t data) {
  FLASH_OB_unlock();
  FLASH->CTLR |= FLASH_CTLR_OPTPG;
  OB->Data0 = (uint16_t)(data & 0x00FF);
  while(FLASH_busy());
  OB->Data1 = (uint16_t)(data >> 8);
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTPG;
  FLASH_lock();
}
