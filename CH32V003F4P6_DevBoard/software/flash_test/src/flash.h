// ===================================================================================
// Basic FLASH Functions for CH32V003                                         * v1.2 *
// ===================================================================================
//
// Functions available:
// --------------------
// FLASH_lock()             Lock FLASH (set write protection)
// FLASH_unlock()           Unlock FLASH (remove write protection)
// FLASH_locked()           Check if FLASH is locked (write protected)
//
// FLASH_read(a)            Read 16-bit data from FLASH address (a)
// FLASH_write(a, d)        Write 16-bit data (d) to FLASH address (a)
// FLASH_PAGE_erase(p)      Erase CODE FLASH page (0..255, 64 bytes each)
//
// FLASH_END_erase()        Erase last page of CODE FLASH (64 bytes)
// FLASH_END_read(a)        Read 16-bit data from CODE FLASH END address (a)
// FLASH_END_write(a, d)    Write 16-bit data (d) to CODE FLASH END address (a)
//
// FLASH_OB_lock()          Lock OPTION BYTES (set write protection)
// FLASH_OB_unlock()        Unlock OPTION BYTES (remove write protection)
// FLASH_OB_locked()        Check if OPTION BYTES are locked (write protected)
// FLASH_OB_protect()       Set FLASH read/write protection (be careful!!!)
// FLASH_OB_unprotect()     Remove FLASH read/write protection
// FLASH_OB_protected()     Check if FLASH is read/write protected
//
// FLASH_OB_erase()         Erase OPTION BYTES and remove read/write protection
// FLASH_OB_write(a, b)     Write 8-bit OPTION BYTE (b) to address (a)
// FLASH_OB_DATA_read()     Read OPTION BYTES user data (16-bit)
// FLASH_OB_DATA_write(d)   Write OPTION BYTES user data (16-bit)
// FLASH_OB_USER_read()     Read OPTION BYTES user flags (see below)
// FLASH_OB_USER_write(f)   Write OPTION BYTES user flags (see below)
//
// FLASH_OB_RESET2GPIO()    Make the RESET pin a normal GPIO pin (PD7)
// FLASH_OB_DEFAULT()       Set OPTION BYTES user flags to default
//
// Option bytes user flags (combine by OR):
// ----------------------------------------
// FLASH_OB_IWDGSW          IWDG is enabled by software (default)
// FLASH_OB_STBYRST         Enable low-power mngmt rst for standby (default)
// FLASH_OB_RST_128US       Reset pin I/O enable  (PD7), ignore delay time 128us
// FLASH_OB_RST_1MS         Reset pin I/O enable  (PD7), ignore delay time   1ms
// FLASH_OB_RST_12MS        Reset pin I/O enable  (PD7), ignore delay time  12ms
// FLASH_OB_RST_OFF         Reset pin I/O disable (PD7), (default)
//
// Notes:
// ------
// - The FLASH must be unlocked prior to writing. This is not necessary for the
//   functions FLASH_OB_erase(), FLASH_OB_DATA_write(d), FLASH_OB_USER_write(f),
//   FLASH_OB_protect(), FLASH_OB_unprotect(), FLASH_OB_RESET2GPIO() and 
//   FLASH_OB_DEFAULT().
// - FLASH areas must be erased before being overwritten.
// - The addresses (a) in FLASH_END_read(a) and FLASH_END_write(a, d) are counted
//   from the end of the CODE FLASH area meaning these functions can be used to store
//   user data without affecting the firmware code (if there's enough space left).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v003.h"

void FLASH_write(uint32_t addr, uint16_t data);
void FLASH_PAGE_erase(uint8_t page);
void FLASH_OB_write(uint32_t addr, uint8_t data);
void FLASH_OB_protect(void);
void FLASH_OB_unprotect(void);
void FLASH_OB_erase(void);
void FLASH_OB_USER_write(uint8_t flags);
void FLASH_OB_DATA_write(uint16_t data);

#define FLASH_RDPRT             0x00A5
#define FLASH_BOOT_BASE         0x1FFFF000
#define FLASH_CODE_BASE         FLASH_BASE
#define FLASH_PAGE_BASE(p)      (FLASH_BASE + ((uint16_t)(p) << 6))

#define FLASH_read(a)           (*(__IO uint16_t *)(a))
#define FLASH_END_erase()       FLASH_PAGE_erase(255)
#define FLASH_END_read(a)       (*(__IO uint16_t *)(FLASH_BASE + 0x3FFE - (a)))
#define FLASH_END_write(a,d)    FLASH_write(FLASH_BASE + 0x3FFE - (a), d)

#define FLASH_lock()            FLASH->CTLR |= FLASH_CTLR_LOCK
#define FLASH_unlock()          {FLASH->KEYR = FLASH_KEY1; FLASH->KEYR = FLASH_KEY2;}
#define FLASH_locked()          (FLASH->CTLR & FLASH_CTLR_LOCK)
#define FLASH_OB_lock()         FLASH->CTLR &= ~FLASH_CTLR_OPTWRE
#define FLASH_OB_unlock()       {FLASH->OBKEYR = FLASH_KEY1; FLASH->OBKEYR = FLASH_KEY2;}
#define FLASH_OB_locked()       (!(FLASH->CTLR & FLASH_CTLR_OPTWRE))
#define FLASH_OB_protected()    (FLASH->OBR & FLASH_OBR_RDPRT)
#define FLASH_FAST_lock()       FLASH->CTLR |= FLASH_CTLR_FLOCK
#define FLASH_FAST_unlock()     {FLASH->MODEKEYR = FLASH_KEY1; FLASH->MODEKEYR = FLASH_KEY2;}
#define FLASH_FAST_locked()     (FLASH->CTLR & FLASH_CTLR_FLOCK)

#define FLASH_OB_DATA_read()    (uint16_t)((OB->Data0 & 0x00FF) | (OB->Data1 << 8))
#define FLASH_OB_USER_read()    (uint8_t)(OB->USER & 0x1F)
#define FLASH_OB_DATA_get()     ((uint16_t)((uint32_t)FLASH->OBR >> 10))
#define FLASH_OB_USER_get()     ((uint8_t)(FLASH->OBR >> 2) & 0x1F)

#define FLASH_OB_RESET2GPIO()   FLASH_OB_USER_write(FLASH_OB_IWDGSW | FLASH_OB_STBYRST | FLASH_OB_RST_1MS)
#define FLASH_OB_DEFAULT()      FLASH_OB_USER_write(0x1F)

#define FLASH_OB_IWDGSW         ((uint8_t)0b00000001)
#define FLASH_OB_STOPRST        ((uint8_t)0b00000010)
#define FLASH_OB_STBYRST        ((uint8_t)0b00000100)
#define FLASH_OB_RST_128US      ((uint8_t)0b00000000)
#define FLASH_OB_RST_1MS        ((uint8_t)0b00001000)
#define FLASH_OB_RST_12MS       ((uint8_t)0b00010000)
#define FLASH_OB_RST_OFF        ((uint8_t)0b00011000)

#ifdef __cplusplus
};
#endif
