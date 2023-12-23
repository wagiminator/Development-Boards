// ===================================================================================
// Basic System Functions for STC8H Microcontrollers                          * v1.1 *
// ===================================================================================
//
// Functions available:
// --------------------
// SYS_CLK_HSI()            Set HSI as system clock source
// SYS_CLK_HSE()            Set HSE as system clock source
// SYS_CLK_DIV(d)           Set system clock divider
//
// HSI_enable()             Enable high speed internal oscillator (HSI)
// HSI_disable()            Disable high speed internal oscillator (HSI)
// LSI_enable()             Enable low speed internal oscillator (LSI)
// LSI_disable()            Disable low speed internal oscillator (LSI)
// HSE_enable(t)            Enable high speed external oscillator (HSE)
//                          (t=0: clock signal on P1.7, 1: oscillator on P1.6/P1.7)
// HSE_disable()            Disable high speed external oscillator (HSE)
// LSE_enable(g)            Enable low speed external oscillator (LSE)
//                          (g=0: low gain, g=1: high gain)
// LSE_disable()            Disable low speed external oscillator (LSE)
// IRC48M_enable()          Enable 48MHz internal oscillator (IRC48M)
// IRC48M_disable()         Disable 48MHz internal oscillator (IRC48M)
//
// SYS_MCO_DIV(d)           Set sytem clock output (MCO) divider (0: MCO off)
// SYS_MCO_PIN(p)           Define MCO pin (0: P5.4, 1: P1.6)
//
// WDT_start(d)             Start watchdog timer with clock divider d (0-7), disabled in IDLE
// WDT_start_idle(d)        Start watchdog timer with clock divider d (0-7), enabled in IDLE
// WDT_reset()              Reset watchdog timer
// WDT_feed()               Reset watchdog timer (alias)
//
// RST_now()                Perform software reset
// RST_pin_enable()         Enable reset pin
// RST_pin_disable()        Disable reset pin (make P5.4 a normal I/O pin)
// RST_LVD_enable()         Enable low voltage detection (LVD) reset
// RST_LVD_disable()        Disable low voltage detection (LVD) reset
// RST_LVD_set(n)           Set LVD threshold voltage (0: 2.0V, 1: 2.4V, 2: 2.7V, 3: 3.0V)
// RST_LVD_flag()           Read low voltage detection flag
// RST_POV_flag()           Read power-on/off reset flag
//
// BOOT_now()               Reset to bootloader
//
// SLEEP_idle()             Put device into IDLE mode
// SLEEP_stop()             Put device into STOP mode
// SLEEP_ms(ms)             Put device into STOP, wakeup after ms milliseconds (max 16384)
// SLEEP_timer_read()       Get time in milliseconds the device was in sleep
// SLEEP_timer_set(ms)      Set wakeup timer in milliseconds
//
// Notes:
// ------
// Access to extended registers must be granted (P_SW2 |= 0x80;).
// Watchdog timer period = 12 * 32768 * (2 << divider) / F_CPU [s]
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "stc8h.h"

// ===================================================================================
// System Clock
// ===================================================================================

// Enable high speed internal oscillator (HSI)
inline void HSI_enable(void) {
  IRCCR  =  0x80;
  while(!(IRCCR & 0x01));
}

// Disable high speed internal oscillator (HSI)
inline void HSI_disable(void) {
  IRCCR  =  0x00;
}

// Enable low speed internal oscillator (LSI)
inline void LSI_enable(void) {
  IRC32KCR =  0x80;
  while(!(IRC32KCR & 0x01));
}

// Disable low speed internal oscillator (LSI)
inline void LSI_disable(void) {
  IRC32KCR =  0x00;
}

// Enable high speed external oscillator (HSE)
inline void HSE_enable(__bit xitype) {
  (xitype) ? (XOSCCR |= (0x01 << 7) | (0x01 << 6)) : (XOSCCR |= (0x01 << 7));
  while(!(XOSCCR & 0x01));
}

// Disable high speed external oscillator (HSE)
inline void HSE_disable(void) {
  XOSCCR &= ~((0x01 << 7) | (0x01 << 6));
}

// Enable low speed external oscillator (LSE)
inline void LSE_enable(__bit gain) {
  (gain) ? (X32KCR = (0x01 << 7) | (0x01 << 6)) : (X32KCR = (0x01 << 7));
  while(!(X32KCR & 0x01));
}

// Disable low speed external oscillator (LSE)
inline void LSE_disable(void) {
  X32KCR =  0x00;
}

// Enable 48MHz internal oscillator (IRC48M)
inline void IRC48M_enable(void) {
  IRC48MCR = 0x80;
  while(!(IRC48MCR & 0x01));
}

// Disable 48MHz internal oscillator (IRC48M)
inline void IRC48M_disable(void) {
  IRC48MCR = 0x00;
}

// Set HSI as system clock source
inline void SYS_CLK_HSI(void) {
  IRCCR  =  0x80;
  while(!(IRCCR & 0x01));
  CKSEL  =  0x00;
}

// Set HSE as system clock source
inline void SYS_CLK_HSE(__bit xitype) {
  (xitype) ? (XOSCCR |= (0x01 << 7) | (0x01 << 6)) : (XOSCCR |= (0x01 << 7));
  while(!(XOSCCR & 0x01));
  CKSEL = 0x01;
}

// Set system clock divider
#define SYS_CLK_DIV(div)    CLKDIV = (div)

// Set sytem clock output (MCO) divider (0: MCO off)
#define SYS_MCO_DIV(div)    MCLKOCR = (MCLKOCR & 0x80) | (div)

// Define MCO pin (0: P5.4, 1: P1.6)
#define SYS_MCO_PIN(pin)    ((pin) ? MCLKOCR |= 0x80 : MCLKOCR &= 0x7f)

// ===================================================================================
// Watchdog Timer
// ===================================================================================
#define WDT_start(d)        WDT_CONTR  = 0x20 | ((d) & 0x07)
#define WDT_start_idle(d)   WDT_CONTR  = 0x28 | ((d) & 0x07)
#define WDT_reset()         WDT_CONTR |= 0x10
#define WDT_feed()          WDT_CONTR |= 0x10

// ===================================================================================
// Reset and Bootloader
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
// Sleep
// ===================================================================================
#define SLEEP_idle()        PCON |= 0x01
#define SLEEP_stop()        PCON |= 0x02
#define SLEEP_ms(ms)        {SLEEP_timer_set(ms); SLEEP_stop();}
#define SLEEP_timer_read()  ( ((((uint16_t)(WKTCH & 0x7f) << 8) | WKTCL) + 1) >> 1 )
#define SLEEP_timer_set(ms) {WKTCL = ((ms) << 1) - 1; WKTCH = 0x80 | ((((ms) << 1) - 1) >> 8);}
