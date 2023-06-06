// ===================================================================================
// Basic FLASH Functions for CH32V003                                         * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// FLASH_lock()             Lock FLASH (set write protection)
// FLASH_unlock()           Unlock FLASH (remove write protection)
//
// FLASH_OB_lock()          Lock option bytes (set write protection)
// FLASH_OB_unlock()        Unlock option bytes (remove write protection)
//
// FLASH_OB_erase()         Erase option bytes and remove read protection
// FLASH_OB_write(a, b)     Write 8-bit option byte (b) to address (a)
// FLASH_OB_DATA_read()     Read option bytes user data (16-bit)
// FLASH_OB_DATA_write(d)   Write option bytes user data (16-bit)
// FLASH_OB_USER_read()     Read option bytes user flags (see below)
// FLASH_OB_USER_write(f)   Write option bytes user flags (see below)
//
// FLASH_OB_RESET2GPIO()    Make the RESET pin a normal GPIO pin (PD7)
// FLASH_OB_DEFAULT()       Set option bytes user flags to default
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
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v003.h"

void FLASH_write(uint32_t addr, uint16_t data);

void FLASH_OB_erase(void);
void FLASH_OB_write(uint32_t addr, uint8_t data);
void FLASH_OB_USER_write(uint8_t flags);
void FLASH_OB_DATA_write(uint16_t data);
void FLASH_OB_read_enable(void);

#define FLASH_OB_lock()       FLASH->CTLR &= ~FLASH_CTLR_OPTWRE
#define FLASH_lock()          FLASH->CTLR |=  FLASH_CTLR_LOCK
#define FLASH_unlock()        {FLASH->KEYR = FLASH_KEY1; FLASH->KEYR = FLASH_KEY2;}

#define FLASH_OB_DATA_read()  (uint16_t)((OB->Data0 & 0x00FF) | (OB->Data1 << 8))
#define FLASH_OB_USER_read()  (uint8_t)(OB->USER & 0x1F)

#define FLASH_OB_RESET2GPIO() FLASH_OB_USER_write(FLASH_OB_IWDGSW | FLASH_OB_STBYRST | FLASH_OB_RST_1MS)
#define FLASH_OB_DEFAULT()    FLASH_OB_USER_write(0x1F)

#define FLASH_OB_IWDGSW       ((uint8_t)0b00000001)
#define FLASH_OB_STOPRST      ((uint8_t)0b00000010)
#define FLASH_OB_STBYRST      ((uint8_t)0b00000100)
#define FLASH_OB_RST_128US    ((uint8_t)0b00000000)
#define FLASH_OB_RST_1MS      ((uint8_t)0b00001000)
#define FLASH_OB_RST_12MS     ((uint8_t)0b00010000)
#define FLASH_OB_RST_OFF      ((uint8_t)0b00011000)

#ifdef __cplusplus
};
#endif
