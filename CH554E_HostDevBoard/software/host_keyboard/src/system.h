// ===================================================================================
// Basic System Functions for CH551, CH552 and CH554                          * v1.7 *
// ===================================================================================
//
// System clock (CLK) functions available:
// ---------------------------------------
// CLK_config()             set system clock frequency according to F_CPU
// CLK_external()           set external crystal as clock source (waits ~10ms for start-up)
// CLK_internal()           set internal oscillator as clock source
//
// Watchdog Timer (WDT) functions available:
// -----------------------------------------
// WDT_start()              start watchdog timer with full period
// WDT_stop()               stop watchdog timer
// WDT_reset()              reload watchdog timer with full period
// WDT_set(time)            reload watchdog timer with specified time in ms
// WDT_feed(value)          reload watchdog timer with specified value
//
// Reset (RST) and bootloader functions available:
// -----------------------------------------------
// RST_now()                perform software reset
// RST_keep(value)          keep this value after RESET
// RST_getKeep()            read the keeped value
// RST_wasWDT()             check if last RESET was caused by watchdog timer
// RST_wasPIN()             check if last RESET was caused by RST PIN
// RST_wasPWR()             check if last RESET was caused by power-on
// RST_wasSOFT()            check if last RESET was caused by software
//
// BOOT_prepare()           prepare peripherals (USB, UART, ...) before jumping to bootloader
// BOOT_now()               enter bootloader
//
// Sleep functions available:
// --------------------------
// SLEEP_now()              put device into sleep
//
// WAKE_enable(source)      enable wake-up from sleep source (sources see below)
// WAKE_disable(source)     disable wake-up from sleep source
// WAKE_all_disable()       disable all wake-up sources
//
// WAKE_USB_enable()        enable wake-up by USB event
// WAKE_RXD0_enable()       enable wake-up by RXD0 low level
// WAKE_RXD1_enable()       enable wake-up by RXD1 low level
// WAKE_P13_enable()        enable wake-up by pin P1.3 low level
// WAKE_P14_enable()        enable wake-up by pin P1.4 low level
// WAKE_P15_enable()        enable wake-up by pin P1.5 low level
// WAKE_RST_enable()        enable wake-up by pin RST high level
// WAKE_INT_enable()        enable wake-up by pin P3.2 edge or pin P3.3 low level
//
// WAKE_USB_disable()       disable wake-up by USB event
// WAKE_RXD0_disable()      disable wake-up by RXD0 low level
// WAKE_RXD1_disable()      disable wake-up by RXD1 low level
// WAKE_P13_disable()       disable wake-up by pin P1.3 low level
// WAKE_P14_disable()       disable wake-up by pin P1.4 low level
// WAKE_P15_disable()       disable wake-up by pin P1.5 low level
// WAKE_RST_disable()       disable wake-up by pin RST high level
// WAKE_INT_disable()       disable wake-up by pin P3.2 edge or pin P3.3 low level
//
// Wake-up from SLEEP sources:
// ---------------------------
// WAKE_USB                 wake-up by USB event
// WAKE_RXD0                wake-up by RXD0 low level
// WAKE_RXD1                wake-up by RXD1 low level
// WAKE_P13                 wake-up by pin P1.3 low level
// WAKE_P14                 wake-up by pin P1.4 low level
// WAKE_P15                 wake-up by pin P1.5 low level
// WAKE_RST                 wake-up by pin RST high level
// WAKE_INT                 wake-up by pin P3.2 edge or pin P3.3 low level
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted (restores EA state;
//                          do not leave the block with break, return or goto)
//
// Chip identification (ID) functions available:
// ---------------------------------------------
// ID_CHIP()                read chip ID (0x51: CH551, 0x52: CH552, 0x54: CH554)
// ID_read()                read 32-bit unique chip ID
//
// Notes:
// ------
// - F_CPU must be 24MHz (VCC >= 4.5V), 16MHz (VCC >= 3.8V), 12MHz, 6MHz, 3MHz, 750kHz
//   or 187.5kHz. USB needs at least 6MHz.
// - Registers CLOCK_CFG, GLOBAL_CFG and WAKE_CTRL can only be written in safe mode.
//   The safe mode ends after 13 to 23 clock cycles, so only short single writes are
//   done within it.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "ch554.h"

// ===================================================================================
// Safe Mode (internal)
// ===================================================================================
#define SAFE_MOD_enter()  do {SAFE_MOD = 0x55; SAFE_MOD = 0xAA;} while(0)
#define SAFE_MOD_exit()   SAFE_MOD = 0x00

// ===================================================================================
// System Clock (CLK) Functions
// ===================================================================================
#if   F_CPU == 24000000
  #define CLK_SEL_VAL     0b110
#elif F_CPU == 16000000
  #define CLK_SEL_VAL     0b101
#elif F_CPU == 12000000
  #define CLK_SEL_VAL     0b100
#elif F_CPU == 6000000
  #define CLK_SEL_VAL     0b011
#elif F_CPU == 3000000
  #define CLK_SEL_VAL     0b010
#elif F_CPU == 750000
  #define CLK_SEL_VAL     0b001
#elif F_CPU == 187500
  #define CLK_SEL_VAL     0b000
#else
  #error F_CPU invalid or not set (24/16/12/6/3/0.75/0.1875MHz, 32MHz is reserved)
#endif

inline void CLK_config(void) {
  uint8_t cfg = (CLOCK_CFG & ~MASK_SYS_CK_SEL) | CLK_SEL_VAL;
  SAFE_MOD_enter();
  CLOCK_CFG = cfg;                              // single write within safe mode
  SAFE_MOD_exit();
}

inline void CLK_external(void) {
  uint16_t i;
  SAFE_MOD_enter();
  CLOCK_CFG |=  bOSC_EN_XT;                     // enable external crystal
  SAFE_MOD_exit();
  for(i = F_CPU / 1000; i; i--) __asm__("nop"); // wait > 5ms for crystal start-up
  SAFE_MOD_enter();
  CLOCK_CFG &= ~bOSC_EN_INT;                    // switch to external crystal
  SAFE_MOD_exit();
}

inline void CLK_internal(void) {
  SAFE_MOD_enter();
  CLOCK_CFG |=  bOSC_EN_INT;                    // turn on and select internal oscillator
  SAFE_MOD_exit();
  SAFE_MOD_enter();
  CLOCK_CFG &= ~bOSC_EN_XT;                     // disable external crystal
  SAFE_MOD_exit();
}

#define CLK_inernal       CLK_internal          // old (misspelled) name

// ===================================================================================
// Watchdog Timer (WDT) Functions
// ===================================================================================
// Watchdog counter increments with Fsys/65536, timeout = (256 - WDOG_COUNT) * 65536 / Fsys
#define WDT_reset()       WDOG_COUNT = 0
#define WDT_feed(value)   WDOG_COUNT = (value)
#define WDT_set(time)     WDOG_COUNT = (uint8_t)(256 - (((F_CPU / 1000) * (uint32_t)(time)) >> 16))

inline void WDT_start(void) {
  WDOG_COUNT  = 0;
  SAFE_MOD_enter();
  GLOBAL_CFG |= bWDOG_EN;
  SAFE_MOD_exit();
}

inline void WDT_stop(void) {
  SAFE_MOD_enter();
  GLOBAL_CFG &= ~bWDOG_EN;
  SAFE_MOD_exit();
}

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
#define RST_keep(value)   RESET_KEEP = (value)
#define RST_getKeep()     (RESET_KEEP)
#define RST_wasWDT()      ((PCON & MASK_RST_FLAG) == RST_FLAG_WDOG)
#define RST_wasPIN()      ((PCON & MASK_RST_FLAG) == RST_FLAG_PIN)
#define RST_wasPWR()      ((PCON & MASK_RST_FLAG) == RST_FLAG_POR)
#define RST_wasSOFT()     ((PCON & MASK_RST_FLAG) == RST_FLAG_SW)

inline void RST_now(void) {
  SAFE_MOD_enter();
  GLOBAL_CFG |= bSW_RESET;
}

// ===================================================================================
// Bootloader (BOOT) Functions
// ===================================================================================
inline void BOOT_prepare(void) {
  EA = 0;                                       // disable interrupts
  ES = 0;
  PS = 0;
  TMOD       = 0;
  P1_DIR_PU  = 0;
  P1_MOD_OC  = 0;
  P1         = 0xFF;
  USB_INT_EN = 0;
  USB_CTRL   = 0x06;                            // reset USB module
}

inline void BOOT_now(void) {
  __asm
    ljmp #BOOT_LOAD_ADDR
  __endasm;
}

// ===================================================================================
// Sleep Functions
// ===================================================================================
#define SLEEP_now()   PCON |= PD

#define WAKE_USB      bWAK_BY_USB     // wake-up by USB event
#define WAKE_RXD0     bWAK_RXD0_LO    // wake-up by RXD0 low level
#define WAKE_RXD1     bWAK_RXD1_LO    // wake-up by RXD1 low level
#define WAKE_P13      bWAK_P1_3_LO    // wake-up by pin P1.3 low level
#define WAKE_P14      bWAK_P1_4_LO    // wake-up by pin P1.4 low level
#define WAKE_P15      bWAK_P1_5_LO    // wake-up by pin P1.5 low level
#define WAKE_RST      bWAK_RST_HI     // wake-up by pin RST high level
#define WAKE_INT      bWAK_P3_2E_3L   // wake-up by pin P3.2 (INT0) edge or pin P3.3 (INT1) low level

// WAKE_CTRL can only be written in safe mode
#define WAKE_enable(source)     do {SAFE_MOD_enter(); WAKE_CTRL |=  (source); SAFE_MOD_exit();} while(0)
#define WAKE_disable(source)    do {SAFE_MOD_enter(); WAKE_CTRL &= ~(source); SAFE_MOD_exit();} while(0)
#define WAKE_all_disable()      do {SAFE_MOD_enter(); WAKE_CTRL  =  0;        SAFE_MOD_exit();} while(0)

#define WAKE_USB_enable()       WAKE_enable(bWAK_BY_USB)
#define WAKE_RXD0_enable()      WAKE_enable(bWAK_RXD0_LO)
#define WAKE_RXD1_enable()      WAKE_enable(bWAK_RXD1_LO)
#define WAKE_P13_enable()       WAKE_enable(bWAK_P1_3_LO)
#define WAKE_P14_enable()       WAKE_enable(bWAK_P1_4_LO)
#define WAKE_P15_enable()       WAKE_enable(bWAK_P1_5_LO)
#define WAKE_RST_enable()       WAKE_enable(bWAK_RST_HI)
#define WAKE_INT_enable()       WAKE_enable(bWAK_P3_2E_3L)

#define WAKE_USB_disable()      WAKE_disable(bWAK_BY_USB)
#define WAKE_RXD0_disable()     WAKE_disable(bWAK_RXD0_LO)
#define WAKE_RXD1_disable()     WAKE_disable(bWAK_RXD1_LO)
#define WAKE_P13_disable()      WAKE_disable(bWAK_P1_3_LO)
#define WAKE_P14_disable()      WAKE_disable(bWAK_P1_4_LO)
#define WAKE_P15_disable()      WAKE_disable(bWAK_P1_5_LO)
#define WAKE_RST_disable()      WAKE_disable(bWAK_RST_HI)
#define WAKE_INT_disable()      WAKE_disable(bWAK_P3_2E_3L)

// ===================================================================================
// Interrupt (INT) Functions
// ===================================================================================
#define INT_enable()        EA = 1
#define INT_disable()       EA = 0
#define INT_ATOMIC_BLOCK    for(uint8_t _ea_save = EA, _ea_once = (EA = 0, 1); \
                                _ea_once; EA = _ea_save, _ea_once = 0)

// ===================================================================================
// Chip Identification (ID) Functions
// ===================================================================================
#define ID_CHIP()           (CHIP_ID)
#define ID_read()           (*(__code uint32_t*)ROM_CHIP_ID_LO)
