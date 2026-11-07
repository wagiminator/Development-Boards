// ===================================================================================
// Basic System Functions for tinyAVR 0-Series and 1-Series                   * v1.0 *
// ===================================================================================
//
// System clock functions available:
// ---------------------------------
// CLK_init()               init main clock prescaler according to F_CPU
//
// MCO_enable()             enable system clock output on pin PB5 (20/24-pin devices only)
// MCO_disable()            disable system clock output
//
// HSI_ready()              check if high-speed internal oscillator OSC20M is stable
// HSI_STDBY_on()           enable OSC20M in STANDBY sleep mode
// HSI_STDBY_off()          disable OSC20M in STANDBY sleep mode
// HSI_CAL_read()           read OSC20M calibration value (OSC20MCALIBA)
// HSI_CAL_write(v)         write OSC20M calibration value (OSC20MCALIBA)
// HSI_ERR_read()           read factory oscillator frequency error at 3V in 1/1024
// HSI_ERR5V_read()         read factory oscillator frequency error at 5V in 1/1024
// LSI_ready()              check if low-speed internal oscillator OSCULP32K is stable
// LSI_STDBY_on()           enable OSCULP32K in STANDBY sleep mode
// LSI_STDBY_off()          disable OSCULP32K in STANDBY sleep mode
// HSE_ready()              check if high-speed external clock EXTCLK has started
// LSE_enable()             enable external 32.768kHz low-speed crystal XOSC32K (1-series)
// LSE_disable()            disable XOSC32K
// LSE_ready()              check if XOSC32K is stable
// LSE_STDBY_on()           enable XOSC32K in STANDBY sleep mode
// LSE_STDBY_off()          disable XOSC32K in STANDBY sleep mode
//
// Real-Time Counter (RTC) functions available:
// --------------------------------------------
// RTC_enable(p)            enable RTC with clock prescaler p
//                          (p = 1, 2, 4, 8, ..., 32768)
// RTC_disable()            disable RTC
// RTC_STDBY_on()           enable RTC in STANDBY sleep mode
// RTC_STDBY_off()          disable RTC in STANDBY sleep mode
// RTC_busy()               check if RTC is busy synchronizing
//
// RTC_setLSI()             set internal 32.768kHz (OSCULP32K) as RTC clock source
// RTC_setLSI_1K()          set internal  1.024kHz (OSCULP32K) as RTC clock source
// RTC_setLSE()             set external 32.768kHz (XOSC32K)   as RTC clock source
// RTC_setHSE()             set external clock signal (EXTCLK) as RTC clock source
//
// RTC_CNT_read()           read  RTC counter (16-bit)
// RTC_CNT_write(n)         write RTC counter (16-bit)
// RTC_PER_read()           read  RTC period (16-bit)
// RTC_PER_write(n)         write RTC period (16-bit)
// RTC_CMP_read()           read  RTC compare value (16-bit)
// RTC_CMP_write(n)         write RTC compare value (16-bit)
//
// RTC_CMP_INT_enable()     enable RTC compare match interrupt
// RTC_CMP_INT_disable()    disable RTC compare match interrupt
// RTC_CMP_FLAG_read()      read RTC compare match interrupt flag
// RTC_CMP_FLAG_clear()     clear RTC compare match interrupt flag
//
// RTC_OVF_INT_enable()     enable RTC overflow interrupt
// RTC_OVF_INT_disable()    disable RTC overflow interrupt
// RTC_OVF_FLAG_read()      read RTC overflow interrupt flag
// RTC_OVF_FLAG_clear()     clear RTC overflow interrupt flag
//
// Periodic Interrupt Timer (PIT) functions available:
// ---------------------------------------------------
// PIT_enable(p)            enable PIT with period p in RTC clock cycles
//                          (p = 4, 8, 16, ..., 32768)
// PIT_disable()            disable PIT
// PIT_INT_enable()         enable PIT interrupt
// PIT_INT_disable()        disable PIT interrupt
// PIT_FLAG_read()          read PIT interrupt flag
// PIT_FLAG_clear()         clear PIT interrupt flag
//
// Watchdog Timer (WDT) functions available:
// -----------------------------------------
// WDT_start(ms)            start WDT with period in ms (8, 16, 32, ..., 8192)
// WDT_stop()               stop watchdog timer
// WDT_reset()              reset watchdog timer
// WDT_feed()               reset watchdog timer (alias)
//
// Brown-out Detector (BOD) and Voltage Level Monitor (VLM) functions available:
// -----------------------------------------------------------------------------
// BOD_LVL_read()           read BOD level setting from fuse (0: 1.8V, 2: 2.6V, 7: 4.2V)
// VLM_set(n)               set VLM level (0: 5%, 1: 15%, 2: 25% above BOD level)
// VLM_isBelow()            check if supply voltage is below VLM level (BOD must be on)
// VLM_INT_below()          enable VLM interrupt when supply goes below VLM level
// VLM_INT_above()          enable VLM interrupt when supply goes above VLM level
// VLM_INT_cross()          enable VLM interrupt when supply crosses VLM level
// VLM_INT_disable()        disable VLM interrupt
// VLM_FLAG_read()          read VLM interrupt flag
// VLM_FLAG_clear()         clear VLM interrupt flag
//
// Delay (DLY) functions available:
// --------------------------------
// DLY_ticks(n)             delay n clock cycles
// DLY_cycles(n)            delay n clock cycles (alias)
// DLY_us(n)                delay n microseconds
// DLY_ms(n)                delay n milliseconds
//
// Sleep functions available:
// --------------------------
// SLEEP_set_IDLE()         set sleep mode to IDLE
// SLEEP_set_STDBY()        set sleep mode to STANDBY
// SLEEP_set_PDOWN()        set sleep mode to POWER-DOWN
// SLEEP_now()              start sleep mode
//
// Reset (RST) functions available:
// --------------------------------
// RST_now()                conduct software reset
// RST_clearFlags()         clear all reset flags
// RST_wasUPDI()            check if last reset was caused by UPDI
// RST_wasSoftware()        check if last reset was caused by software
// RST_wasWDT()             check if last reset was caused by watchdog
// RST_wasExternal()        check if last reset was caused by external
// RST_wasBrownout()        check if last reset was caused by brown-out
// RST_wasPower()           check if last reset was caused by power-on
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_SREG_read()          read status register (SREG)
// INT_SREG_write(v)        write status register (SREG)
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
//
// EEPROM functions available:
// ---------------------------
// EEPROM_read(a)             read  8-bit byte from EEPROM address a
// EEPROM_read_byte(a)        read  8-bit byte from EEPROM address a (alias)
// EEPROM_read_word(a)        read 16-bit word from EEPROM address a
// EEPROM_read_block(p,a,l)   read block of length l from EEPROM address a to pointer p
// EEPROM_write(a,b)          write (update)  8-bit byte b to EEPROM address a
// EEPROM_write_byte(a,b)     write (update)  8-bit byte b to EEPROM address a (alias)
// EEPROM_write_word(a,w)     write (update) 16-bit word w to EEPROM address a
// EEPROM_write_block(p,a,l)  write block of length l to EEPROM address a from pointer p
//
// Chip identification (ID) functions available:
// ---------------------------------------------
// ID_DEV_read()            read 24-bit device ID (signature, e.g. 0x1E9322 for ATtiny814)
// ID_SER_read(n)           read byte n (0..9) of the unique 80-bit serial number
// ID_REV_read()            read silicon revision (0: A, 1: B, ...)
//
// Notes:
// ------
// - The main clock is derived from OSC20M, which runs at 16MHz or 20MHz depending on
//   fuse 2 (OSCCFG). CLK_init() only sets the prescaler. The fuse must match F_CPU
//   (e.g. 20MHz for F_CPU = 20/10/5MHz, 16MHz for F_CPU = 16/8/4MHz). tinyupdi sets
//   the fuse automatically with the -t option.
// - F_CPU must be defined before system.h is included (e.g. in the makefile).
// - DLY_us() and DLY_ms() expect compile-time constants.
//
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

// ===================================================================================
// System Clock (CLK) Functions
// ===================================================================================
#ifndef F_CPU
  #define F_CPU               3333333   // default (20MHz / 6 after reset)
#endif

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/eeprom.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <util/atomic.h>

// Main clock prescaler (MCLKCTRLB = PDIV << 1 | PEN)
#if   F_CPU == 20000000 || F_CPU == 16000000
  #define CLK_PRESC_VAL       0x00      // no prescaler
#elif F_CPU == 16500000 || F_CPU == 12800000
  #define CLK_PRESC_VAL       0x00      // no prescaler (tuned 16MHz oscillator, V-USB)
#elif F_CPU == 10000000 || F_CPU ==  8000000
  #define CLK_PRESC_VAL       0x01      // divide by 2
#elif F_CPU ==  5000000 || F_CPU ==  4000000
  #define CLK_PRESC_VAL       0x03      // divide by 4
#elif F_CPU ==  3333333 || F_CPU ==  2666667
  #define CLK_PRESC_VAL       0x11      // divide by 6 (reset default)
#elif F_CPU ==  2500000 || F_CPU ==  2000000
  #define CLK_PRESC_VAL       0x05      // divide by 8
#elif F_CPU ==  1600000
  #define CLK_PRESC_VAL       0x13      // divide by 10
#elif F_CPU ==  1666667 || F_CPU ==  1333333
  #define CLK_PRESC_VAL       0x15      // divide by 12
#elif F_CPU ==  1250000 || F_CPU ==  1000000
  #define CLK_PRESC_VAL       0x07      // divide by 16
#elif F_CPU ==   833333 || F_CPU ==   666667
  #define CLK_PRESC_VAL       0x17      // divide by 24
#elif F_CPU ==   625000 || F_CPU ==   500000
  #define CLK_PRESC_VAL       0x09      // divide by 32
#elif F_CPU ==   416667 || F_CPU ==   333333
  #define CLK_PRESC_VAL       0x19      // divide by 48
#elif F_CPU ==   312500 || F_CPU ==   250000
  #define CLK_PRESC_VAL       0x0B      // divide by 64
#else
  #warning Unsupported system clock frequency, using default (3.33MHz).
#endif

#ifdef CLK_PRESC_VAL
  #define CLK_init()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLB, CLK_PRESC_VAL)
#else
  #define CLK_init()
#endif

#define MCO_enable()          _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL.MCLKCTRLA |  CLKCTRL_CLKOUT_bm)
#define MCO_disable()         _PROTECTED_WRITE(CLKCTRL.MCLKCTRLA, CLKCTRL.MCLKCTRLA & ~CLKCTRL_CLKOUT_bm)

#define HSI_ready()           (CLKCTRL.MCLKSTATUS & CLKCTRL_OSC20MS_bm)
#define HSI_STDBY_on()        _PROTECTED_WRITE(CLKCTRL.OSC20MCTRLA, CLKCTRL_RUNSTDBY_bm)
#define HSI_STDBY_off()       _PROTECTED_WRITE(CLKCTRL.OSC20MCTRLA, 0x00)
#define HSI_CAL_read()        (CLKCTRL.OSC20MCALIBA)
#define HSI_CAL_write(v)      _PROTECTED_WRITE(CLKCTRL.OSC20MCALIBA, (v))

// Factory measured oscillator error (signed, in 1/1024 of the frequency) at 3V or 5V
#define HSI_ERR_read()        ((int8_t)(((FUSE.OSCCFG & FUSE_FREQSEL_gm) == FREQSEL_20MHZ_gc) \
                              ? SIGROW.OSC20ERR3V : SIGROW.OSC16ERR3V))
#define HSI_ERR5V_read()      ((int8_t)(((FUSE.OSCCFG & FUSE_FREQSEL_gm) == FREQSEL_20MHZ_gc) \
                              ? SIGROW.OSC20ERR5V : SIGROW.OSC16ERR5V))

#define HSE_ready()           (CLKCTRL.MCLKSTATUS & CLKCTRL_EXTS_bm)

#define LSI_ready()           (CLKCTRL.MCLKSTATUS & CLKCTRL_OSC32KS_bm)
#define LSI_STDBY_on()        _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA, CLKCTRL_RUNSTDBY_bm)
#define LSI_STDBY_off()       _PROTECTED_WRITE(CLKCTRL.OSC32KCTRLA, 0x00)

#define LSE_enable()          _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL.XOSC32KCTRLA |  CLKCTRL_ENABLE_bm)
#define LSE_disable()         _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL.XOSC32KCTRLA & ~CLKCTRL_ENABLE_bm)
#define LSE_ready()           (CLKCTRL.MCLKSTATUS & CLKCTRL_XOSC32KS_bm)
#define LSE_STDBY_on()        _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL.XOSC32KCTRLA |  CLKCTRL_RUNSTDBY_bm)
#define LSE_STDBY_off()       _PROTECTED_WRITE(CLKCTRL.XOSC32KCTRLA, CLKCTRL.XOSC32KCTRLA & ~CLKCTRL_RUNSTDBY_bm)

// ===================================================================================
// Real-Time Counter (RTC) Functions
// ===================================================================================

// Helper: log2 of a power of two (rounded up), evaluated at compile time
#define _SYS_LOG2(p) ((p)<=1?0:(p)<=2?1:(p)<=4?2:(p)<=8?3:(p)<=16?4:(p)<=32?5:   \
                      (p)<=64?6:(p)<=128?7:(p)<=256?8:(p)<=512?9:(p)<=1024?10:   \
                      (p)<=2048?11:(p)<=4096?12:(p)<=8192?13:(p)<=16384?14:15)

#define RTC_enable(p)         do { while(RTC.STATUS & RTC_CTRLABUSY_bm) {}                \
                                   RTC.CTRLA = (RTC.CTRLA & RTC_RUNSTDBY_bm)             \
                                             | (_SYS_LOG2(p) << RTC_PRESCALER_gp)        \
                                             | RTC_RTCEN_bm; } while(0)
#define RTC_disable()         do { while(RTC.STATUS & RTC_CTRLABUSY_bm) {} RTC.CTRLA &= ~RTC_RTCEN_bm;    } while(0)
#define RTC_STDBY_on()        do { while(RTC.STATUS & RTC_CTRLABUSY_bm) {} RTC.CTRLA |=  RTC_RUNSTDBY_bm; } while(0)
#define RTC_STDBY_off()       do { while(RTC.STATUS & RTC_CTRLABUSY_bm) {} RTC.CTRLA &= ~RTC_RUNSTDBY_bm; } while(0)
#define RTC_busy()            (RTC.STATUS & 0x0f)
#define RTC_setLSI()          RTC.CLKSEL = RTC_CLKSEL_INT32K_gc
#define RTC_setLSI_1K()       RTC.CLKSEL = RTC_CLKSEL_INT1K_gc
#define RTC_setLSE()          RTC.CLKSEL = RTC_CLKSEL_TOSC32K_gc
#define RTC_setHSE()          RTC.CLKSEL = RTC_CLKSEL_EXTCLK_gc
#define RTC_CNT_read()        (RTC.CNT)
#define RTC_CNT_write(n)      do { while(RTC.STATUS & RTC_CNTBUSY_bm) {} RTC.CNT = (n); } while(0)
#define RTC_PER_read()        (RTC.PER)
#define RTC_PER_write(n)      do { while(RTC.STATUS & RTC_PERBUSY_bm) {} RTC.PER = (n); } while(0)
#define RTC_CMP_read()        (RTC.CMP)
#define RTC_CMP_write(n)      do { while(RTC.STATUS & RTC_CMPBUSY_bm) {} RTC.CMP = (n); } while(0)

#define RTC_CMP_INT_enable()  RTC.INTCTRL |=  RTC_CMP_bm
#define RTC_CMP_INT_disable() RTC.INTCTRL &= ~RTC_CMP_bm
#define RTC_CMP_FLAG_read()   (RTC.INTFLAGS & RTC_CMP_bm)
#define RTC_CMP_FLAG_clear()  RTC.INTFLAGS =  RTC_CMP_bm

#define RTC_OVF_INT_enable()  RTC.INTCTRL |=  RTC_OVF_bm
#define RTC_OVF_INT_disable() RTC.INTCTRL &= ~RTC_OVF_bm
#define RTC_OVF_FLAG_read()   (RTC.INTFLAGS & RTC_OVF_bm)
#define RTC_OVF_FLAG_clear()  RTC.INTFLAGS =  RTC_OVF_bm

// PIT period: CYC4 = 1, CYC8 = 2, ..., CYC32768 = 14
#define PIT_enable(p)         do { while(RTC.PITSTATUS & RTC_CTRLBUSY_bm) {}                \
                                   RTC.PITCTRLA = ((_SYS_LOG2((p) < 4 ? 4 : (p)) - 1)     \
                                                << RTC_PERIOD_gp) | RTC_PITEN_bm; } while(0)
#define PIT_disable()         do { while(RTC.PITSTATUS & RTC_CTRLBUSY_bm) {} RTC.PITCTRLA = 0; } while(0)

#define PIT_INT_enable()      RTC.PITINTCTRL = RTC_PI_bm
#define PIT_INT_disable()     RTC.PITINTCTRL = 0
#define PIT_FLAG_read()       (RTC.PITINTFLAGS & RTC_PI_bm)
#define PIT_FLAG_clear()      RTC.PITINTFLAGS = RTC_PI_bm

// ===================================================================================
// Watchdog Timer (WDT) Functions
// ===================================================================================
// WDT period: 8CLK (8ms) = 1, 16CLK = 2, ..., 8KCLK (8.2s) = 11
#define WDT_busy()            (WDT.STATUS & WDT_SYNCBUSY_bm)
#define WDT_start(ms)         do { while(WDT_busy()) {}                                     \
                                   _PROTECTED_WRITE(WDT.CTRLA,                            \
                                   _SYS_LOG2((ms) < 8 ? 8 : ((ms) > 8192 ? 8192 : (ms))) - 2); \
                              } while(0)
#define WDT_stop()            do { while(WDT_busy()) {} _PROTECTED_WRITE(WDT.CTRLA, 0); } while(0)
#define WDT_reset()           __builtin_avr_wdr()
#define WDT_feed()            __builtin_avr_wdr()

// ===================================================================================
// Brown-out Detector (BOD) and Voltage Level Monitor (VLM) Functions
// ===================================================================================
#define BOD_LVL_read()        (FUSE.BODCFG >> 5)
#define VLM_set(n)            BOD.VLMCTRLA = (n) & 0x03
#define VLM_isBelow()         (BOD.STATUS & BOD_VLMS_bm)
#define VLM_INT_below()       BOD.INTCTRL  = BOD_VLMCFG_BELOW_gc | BOD_VLMIE_bm
#define VLM_INT_above()       BOD.INTCTRL  = BOD_VLMCFG_ABOVE_gc | BOD_VLMIE_bm
#define VLM_INT_cross()       BOD.INTCTRL  = BOD_VLMCFG_CROSS_gc | BOD_VLMIE_bm
#define VLM_INT_disable()     BOD.INTCTRL  = 0
#define VLM_FLAG_read()       (BOD.INTFLAGS & BOD_VLMIF_bm)
#define VLM_FLAG_clear()      BOD.INTFLAGS = BOD_VLMIF_bm

// ===================================================================================
// Delay (DLY) Functions
// ===================================================================================
#define DLY_ticks(n)          __builtin_avr_delay_cycles(n)
#define DLY_cycles(n)         __builtin_avr_delay_cycles(n)
#define DLY_us(n)             _delay_us(n)
#define DLY_ms(n)             _delay_ms(n)

// ===================================================================================
// Sleep Functions
// ===================================================================================
#define SLEEP_set_IDLE()      SLPCTRL.CTRLA = SLPCTRL_SMODE_IDLE_gc  | SLPCTRL_SEN_bm
#define SLEEP_set_STDBY()     SLPCTRL.CTRLA = SLPCTRL_SMODE_STDBY_gc | SLPCTRL_SEN_bm
#define SLEEP_set_PDOWN()     SLPCTRL.CTRLA = SLPCTRL_SMODE_PDOWN_gc | SLPCTRL_SEN_bm
#define SLEEP_now()           sleep_cpu()

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
#define RST_now()             _PROTECTED_WRITE(RSTCTRL.SWRR, RSTCTRL_SWRE_bm)
#define RST_clearFlags()      RSTCTRL.RSTFR = 0x3f
#define RST_wasUPDI()         (RSTCTRL.RSTFR & RSTCTRL_UPDIRF_bm)
#define RST_wasSoftware()     (RSTCTRL.RSTFR & RSTCTRL_SWRF_bm)
#define RST_wasWDT()          (RSTCTRL.RSTFR & RSTCTRL_WDRF_bm)
#define RST_wasExternal()     (RSTCTRL.RSTFR & RSTCTRL_EXTRF_bm)
#define RST_wasBrownout()     (RSTCTRL.RSTFR & RSTCTRL_BORF_bm)
#define RST_wasPower()        (RSTCTRL.RSTFR & RSTCTRL_PORF_bm)

// ===================================================================================
// Interrupt (INT) Functions
// ===================================================================================
#define INT_enable()          sei()
#define INT_disable()         cli()
#define INT_SREG_read()       (SREG)
#define INT_SREG_write(v)     SREG = (v)
#define INT_ATOMIC_BLOCK      ATOMIC_BLOCK(ATOMIC_RESTORESTATE)

// ===================================================================================
// EEPROM Functions
// ===================================================================================
#define EEPROM_read(a)            eeprom_read_byte((const uint8_t*)(a))
#define EEPROM_read_byte(a)       eeprom_read_byte((const uint8_t*)(a))
#define EEPROM_read_word(a)       eeprom_read_word((const uint16_t*)(a))
#define EEPROM_read_block(p,a,l)  eeprom_read_block((void*)(p), (const void*)(a), (l))
#define EEPROM_write(a,b)         eeprom_update_byte((uint8_t*)(a), (b))
#define EEPROM_write_byte(a,b)    eeprom_update_byte((uint8_t*)(a), (b))
#define EEPROM_write_word(a,w)    eeprom_update_word((uint16_t*)(a), (w))
#define EEPROM_write_block(p,a,l) eeprom_update_block((const void*)(p), (void*)(a), (l))

// ===================================================================================
// Chip Identification (ID) Functions
// ===================================================================================
#define ID_DEV_read()         (((uint32_t)SIGROW.DEVICEID0 << 16) | ((uint16_t)SIGROW.DEVICEID1 << 8) | SIGROW.DEVICEID2)
#define ID_SER_read(n)        ((&SIGROW.SERNUM0)[(n)])
#define ID_REV_read()         (SYSCFG.REVID)

#ifdef __cplusplus
};
#endif
