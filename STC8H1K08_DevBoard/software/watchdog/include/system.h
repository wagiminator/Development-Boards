// ===================================================================================
// Basic System Functions for STC8H Microcontrollers                          * v1.2 *
// ===================================================================================
//
// System clock functions available:
// ---------------------------------
// SYS_CLK_HSI()            set HSI as system clock source
// SYS_CLK_HSE()            set HSE as system clock source (must be enabled first)
// SYS_CLK_DIV(d)           set system clock divider
//
// SYS_MCO_DIV(d)           set sytem clock output (MCO) divider (0: MCO off)
// SYS_MCO_P54(p)           define P5.4 as MCO pin
// SYS_MCO_P16(p)           define P1.6 as MCO pin
//
// HSI_enable()             enable high speed internal oscillator (HSI)
// HSI_disable()            disable high speed internal oscillator (HSI)
// LSI_enable()             enable low speed internal oscillator (LSI)
// LSI_disable()            disable low speed internal oscillator (LSI)
// HSE_enable(t)            enable high speed external oscillator (HSE)
//                          (t=0: clock signal on P1.7, 1: oscillator on P1.6/P1.7)
// HSE_disable()            disable high speed external oscillator (HSE)
// LSE_enable(g)            enable low speed external oscillator (LSE)
//                          (g=0: low gain, g=1: high gain)
// LSE_disable()            disable low speed external oscillator (LSE)
// IRC48M_enable()          enable 48MHz internal oscillator (IRC48M)
// IRC48M_disable()         disable 48MHz internal oscillator (IRC48M)
//
// Watchdog Timer (WDT) functions available:
// -----------------------------------------
// WDT_start(ms)            start WDT with period in milliseconds, disabled in IDLE
// WDT_start_idle(ms)       start WDT with period in milliseconds, enabled in IDLE
// WDT_reset()              reset WDT
// WDT_feed()               reset WDT (alias)
//
// Reset (RST) functions available:
// --------------------------------
// RST_now()                perform software reset
// RST_pin_enable()         enable reset pin
// RST_pin_disable()        disable reset pin (make P5.4 a normal I/O pin)
// RST_LVD_enable()         enable low voltage detection (LVD) reset
// RST_LVD_disable()        disable low voltage detection (LVD) reset
// RST_LVD_set(n)           set LVD threshold voltage (0: 2.0V, 1: 2.4V, 2: 2.7V, 3: 3.0V)
// RST_LVD_flag()           read low voltage detection flag
// RST_POV_flag()           read power-on/off reset flag
//
// BOOT_now()               reset to bootloader
//
// Sleep functions available:
// --------------------------
// SLEEP_idle()             put device into IDLE mode
// SLEEP_stop()             put device into STOP mode
// SLEEP_ms(ms)             put device into STOP, wakeup after ms milliseconds (max 16384)
// SLEEP_timer_read()       get time in milliseconds the device was in sleep
// SLEEP_timer_set(ms)      set wakeup timer in milliseconds
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
//
// Notes:
// ------
// Access to extended registers must be granted (P_SW2 |= 0x80;).
// Max watchdog timer period = 12 * 32768 * 256 * 1000 / F_CPU [ms]
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

// ===================================================================================
// System Clock Functions
// ===================================================================================
#define HSI_enable()        {IRCCR = 0x80; while(!(IRCCR & 0x01));}
#define HSI_disable()       IRCCR = 0x00
#define LSI_enable()        {IRC32KCR = 0x80; while(!(IRC32KCR & 0x01));}
#define LSI_disable()       IRC32KCR = 0x00
#define HSE_enable(t)       {(t) ? (XOSCCR |= (0x01 << 7) | (0x01 << 6)) : (XOSCCR |= (0x01 << 7)); \
                            while(!(XOSCCR & 0x01));}
#define HSE_disable()       XOSCCR &= ~((0x01 << 7) | (0x01 << 6))
#define LSE_enable(g)       {(g) ? (X32KCR  = (0x01 << 7) | (0x01 << 6)) : (X32KCR  = (0x01 << 7)); \
                            while(!(X32KCR & 0x01));}
#define LSE_disable()       X32KCR = 0x00
#define IRC48M_enable()     {IRC48MCR = 0x80; while(!(IRC48MCR & 0x01));}
#define IRC48M_disable()    IRC48MCR = 0x00

#define SYS_CLK_HSI()       CKSEL = 0x00
#define SYS_CLK_HSE()       CKSEL = 0x01
#define SYS_CLK_DIV(d)      CLKDIV = (d)

#define SYS_MCO_DIV(d)      MCLKOCR = (MCLKOCR & 0x80) | (d)
#define SYS_MCO_P54()       MCLKOCR &= 0x7f
#define SYS_MCO_P16()       MCLKOCR |= 0x80

// ===================================================================================
// Watchdog Timer (WDT) Functions
// ===================================================================================
#define WDT_PERIOD(d)       (12UL * 32768UL * (2UL << (d)) / (F_CPU / 1000))

#define WDT_start(ms)                              \
  (ms <= WDT_PERIOD(0) ? (WDT_CONTR  = 0x20 | 0) : \
  (ms <= WDT_PERIOD(1) ? (WDT_CONTR  = 0x20 | 1) : \
  (ms <= WDT_PERIOD(2) ? (WDT_CONTR  = 0x20 | 2) : \
  (ms <= WDT_PERIOD(3) ? (WDT_CONTR  = 0x20 | 3) : \
  (ms <= WDT_PERIOD(4) ? (WDT_CONTR  = 0x20 | 4) : \
  (ms <= WDT_PERIOD(5) ? (WDT_CONTR  = 0x20 | 5) : \
  (ms <= WDT_PERIOD(6) ? (WDT_CONTR  = 0x20 | 6) : \
                         (WDT_CONTR  = 0x20 | 7))))))))

#define WDT_start_idle(ms)                         \
  (ms <= WDT_PERIOD(0) ? (WDT_CONTR  = 0x28 | 0) : \
  (ms <= WDT_PERIOD(1) ? (WDT_CONTR  = 0x28 | 1) : \
  (ms <= WDT_PERIOD(2) ? (WDT_CONTR  = 0x28 | 2) : \
  (ms <= WDT_PERIOD(3) ? (WDT_CONTR  = 0x28 | 3) : \
  (ms <= WDT_PERIOD(4) ? (WDT_CONTR  = 0x28 | 4) : \
  (ms <= WDT_PERIOD(5) ? (WDT_CONTR  = 0x28 | 5) : \
  (ms <= WDT_PERIOD(6) ? (WDT_CONTR  = 0x28 | 6) : \
                         (WDT_CONTR  = 0x28 | 7))))))))

#define WDT_reset()         WDT_CONTR |= 0x10
#define WDT_feed()          WDT_CONTR |= 0x10

// ===================================================================================
// Reset (RST) and Bootloader (BOOT) Functions
// ===================================================================================
#define RST_now()           IAP_CONTR |= 0x20
#define RST_pin_enable()    RSTCFG |= 0x10
#define RST_pin_disable()   RSTCFG &= 0xef
#define RST_LVD_enable()    RSTCFG |= 0x40
#define RST_LVD_disable()   RSTCFG &= 0xbf
#define RST_LVD_set(n)      RSTCFG = (RSTCFG & 0xfc) | (n)
#define RST_LVD_flag()      (PCON & 0x20)
#define RST_POV_flag()      (PCON & 0x10)

#define BOOT_now()          IAP_CONTR |= 0x60

// ===================================================================================
// Sleep Functions
// ===================================================================================
#define SLEEP_idle()        PCON |= 0x01
#define SLEEP_stop()        PCON |= 0x02
#define SLEEP_ms(ms)        {SLEEP_timer_set(ms); SLEEP_stop();}
#define SLEEP_timer_read()  ( ((((uint16_t)(WKTCH & 0x7f) << 8) | WKTCL) + 1) >> 1 )
#define SLEEP_timer_set(ms) {WKTCL = ((ms) << 1) - 1; WKTCH = 0x80 | ((((ms) << 1) - 1) >> 8);}

// ===================================================================================
// Interrupt (INT) Functions
// ===================================================================================
#define INT_enable()        EA = 1
#define INT_disable()       EA = 0
#define INT_ATOMIC_BLOCK    for(EA=0;!EA;EA=1)
