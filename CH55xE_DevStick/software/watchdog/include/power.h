// ===================================================================================
// Power, Reset, Sleep and Watchdog Functions for CH551, CH552 and CH554
// ===================================================================================
//
// Functions available:
// --------------------
// WDT_start()              start watchdog timer with full period
// WDT_stop()               stop watchdog timer
// WDT_reset()              reload watchdog timer with full period
// WDT_set(time)            reload watchdog timer with specified time in ms
// WDT_feed(value)          reload watchdog timer with specified value
//
// RST_reset()              perform software reset
// RST_keep(value)          keep this value after RESET
// RST_getKeep()            read the keeped value
// RST_wasWDT()             check if last RESET was caused by watchdog timer
// RST_wasPIN()             check if last RESET was caused by RST PIN
// RST_wasPWR()             check if last RESET was caused by power-on
// RST_wasSOFT()            check if last RESET was caused by software
//
// SLEEP_now()              put device into sleep
// WAKE_enable(source)      enable wake-up from sleep source (see below)
// WAKE_disable(source)     disable wake-up from sleep source
// WAKE_PIN_enable(PIN)     enable wake-up by PIN low (P13, P14, P15 only)
// WAKE_PIN_disable(PIN)    disable wake-up by PIN low
// WAKE_all_disable()       disable all wake-up sources
//
// Wake-up from SLEEP sources:
// ---------------------------
// WAKE_USB                 enable wake-up by USB event
// WAKE_RXD0                enable wake-up by RXD0 low level
// WAKE_RXD1                enable wake-up by RXD1 low level
// WAKE_P13                 enable wake-up by pin P1.3 low level
// WAKE_P14                 enable wake-up by pin P1.4 low level
// WAKE_P15                 enable wake-up by pin P1.5 low level
// WAKE_RST                 enable wake-up by pin RST high level
// WAKE_INT                 enable wake-up by pin P3.2 edge or pin P3.3 low level
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

// ===================================================================================
// Watchdog Timer
// ===================================================================================
#define WDT_reset()       WDOG_COUNT = 0
#define WDT_feed(value)   WDOG_COUNT = value
#define WDT_set(time)     WDOG_COUNT = (uint8_t)(256 - ((FREQ_SYS / 1000) * time / 65536))

inline void WDT_start(void) {
  WDOG_COUNT  = 0;
  SAFE_MOD    = 0x55;
  SAFE_MOD    = 0xAA;
  GLOBAL_CFG |= bWDOG_EN;
  SAFE_MOD    = 0x00;
}

inline void WDT_stop(void) {
  SAFE_MOD    = 0x55;
  SAFE_MOD    = 0xAA; 
  GLOBAL_CFG &= ~bWDOG_EN;
  SAFE_MOD    = 0x00;
}

// ===================================================================================
// Reset
// ===================================================================================
#define RST_keep(value)   RESET_KEEP = value
#define RST_getKeep()     (RESET_KEEP)
#define RST_wasWDT()      ((PCON & MASK_RST_FLAG) == RST_FLAG_WDOG)
#define RST_wasPIN()      ((PCON & MASK_RST_FLAG) == RST_FLAG_PIN)
#define RST_wasPWR()      ((PCON & MASK_RST_FLAG) == RST_FLAG_POR)
#define RST_wasSOFT()     ((PCON & MASK_RST_FLAG) == RST_FLAG_SW)

inline void RST_reset(void) {
  SAFE_MOD    = 0x55;
  SAFE_MOD    = 0xAA;
  GLOBAL_CFG |= bSW_RESET;
}

// ===================================================================================
// Sleep
// ===================================================================================
#define SLEEP_now()       PCON |= PD

#define WAKE_USB          bWAK_BY_USB     // enable wake-up by USB event
#define WAKE_RXD0         bWAK_RXD0_LO    // enable wake-up by RXD0 low level
#define WAKE_RXD1         bWAK_RXD1_LO    // enable wake-up by RXD1 low level
#define WAKE_P13          bWAK_P1_3_LO    // enable wake-up by pin P1.3 low level
#define WAKE_P14          bWAK_P1_4_LO    // enable wake-up by pin P1.4 low level
#define WAKE_P15          bWAK_P1_5_LO    // enable wake-up by pin P1.5 low level
#define WAKE_RST          bWAK_RST_HI     // enable wake-up by pin RST high level
#define WAKE_INT          bWAK_P3_2E_3L   // enable wake-up by pin P3.2 (INT0) edge or pin P3.3 (INT1) low level

#define WAKE_enable(source)     WAKE_CTRL |=  source
#define WAKE_disable(source)    WAKE_CTRL &= ~source
#define WAKE_all_disable()      WAKE_CTRL  =  0

#define WAKE_PIN_enable(PIN) \
  ((PIN == P13) ? (WAKE_CTRL |= bWAK_P1_3_LO) : \
  ((PIN == P14) ? (WAKE_CTRL |= bWAK_P1_4_LO) : \
  ((PIN == P15) ? (WAKE_CTRL |= bWAK_P1_5_LO) : \
(0)))))

#define WAKE_PIN_disable(SOURCE) \
  ((PIN == P13) ? (WAKE_CTRL &= ~bWAK_P1_3_LO) : \
  ((PIN == P14) ? (WAKE_CTRL &= ~bWAK_P1_4_LO) : \
  ((PIN == P15) ? (WAKE_CTRL &= ~bWAK_P1_5_LO) : \
(0)))))
