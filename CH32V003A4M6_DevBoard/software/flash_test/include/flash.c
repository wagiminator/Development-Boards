// ===================================================================================
// Basic FLASH Functions for CH32V003                                         * v1.1 *
// ===================================================================================

#include "flash.h"

#define FLASH_OB_MASK       0b11100010
#define FLASH_busy()        (FLASH->STATR & FLASH_STATR_BSY)
#define FLASH_error()       (FLASH->STATR & FLASH_STATR_WRPRTERR)
#define FLASH_eop()         (FLASH->STATR & FLASH_STATR_EOP)

// Erase FLASH page (64 bytes)
void FLASH_PAGE_erase(uint8_t page) {
  uint32_t addr = FLASH_PAGE_BASE(page);
  FLASH_FAST_unlock();
  FLASH->CTLR |=  FLASH_CTLR_PAGE_ER;
  FLASH->ADDR  =  addr;
  FLASH->CTLR |=  FLASH_CTLR_STRT;
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_PAGE_ER;
  FLASH_FAST_lock();
}

// Write 16-bit data to FLASH addr
void FLASH_write(uint32_t addr, uint16_t data) {
  FLASH->CTLR |= FLASH_CTLR_PG;
  *(__IO uint16_t *)addr = data;
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_PG;
}

// Write option byte to FLASH addr
void FLASH_OB_write(uint32_t addr, uint8_t data) {
  FLASH->CTLR |= FLASH_CTLR_OPTPG;
  *(__IO uint16_t *)addr = data;
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTPG;
}

// Unlock FLASH and remove option bytes write protection
void FLASH_OB_unlock_full(void) {
  FLASH_unlock();
  FLASH_OB_unlock();
}

// Set option bytes read protection and CODE FLASH write protection
void FLASH_OB_protect(void) {
  FLASH_OB_unlock_full();
  FLASH->CTLR |=  FLASH_CTLR_OPTPG;
  OB->RDPR = 0x0001;
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTPG;
  FLASH_lock();
}

// Remove option bytes read protection and CODE FLASH write protection
void FLASH_OB_unprotect(void) {
  FLASH_OB_unlock_full();
  FLASH->CTLR |=  FLASH_CTLR_OPTPG;
  OB->RDPR = FLASH_RDPRT;
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTPG;
  FLASH_lock();
}

// Erase option bytes area and remove protection
void FLASH_OB_erase(void) {
  FLASH_OB_unlock_full();
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

// Write option bytes user flags
void FLASH_OB_USER_write(uint8_t flags) {
  FLASH_OB_unlock_full();
  FLASH->CTLR |= FLASH_CTLR_OPTPG;
  OB->USER = (uint16_t)(FLASH_OB_MASK | flags);
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTPG;
  FLASH_lock();
}

// Write option bytes user data
void FLASH_OB_DATA_write(uint16_t data) {
  FLASH_OB_unlock_full();
  FLASH->CTLR |= FLASH_CTLR_OPTPG;
  OB->Data0 = (uint16_t)(data & 0x00FF);
  while(FLASH_busy());
  OB->Data1 = (uint16_t)(data >> 8);
  while(FLASH_busy());
  FLASH->CTLR &= ~FLASH_CTLR_OPTPG;
  FLASH_lock();
}
