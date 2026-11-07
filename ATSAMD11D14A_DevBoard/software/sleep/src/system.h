// ===================================================================================
// Basic System Functions for ATSAMD11C14A, ATSAMD11D14AM/AS/AU               * v1.0 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are
// set up automatically on system start.
//
// System clock functions available:
// ---------------------------------
// CLK_init_DFLL()          init DFLL48M (48MHz, divided) as system clock source
// CLK_init_OSC8M()         init internal 8MHz oscillator (divided) as system clock
// CLK_reset()              reset system clock to default state (OSC8M / 8 = 1MHz)
//
// OSC8M_enable()           enable internal 8MHz RC oscillator (OSC8M)
// OSC8M_disable()          disable OSC8M
// OSC8M_ready()            check if OSC8M is stable
// OSC8M_divider(n)         set OSC8M prescaler to 2^n (n = 0..3, OSC8M = 8MHz / 2^n)
//
// DFLL_ready()             check if DFLL48M is ready
// DFLL_locked()            check if DFLL48M is locked (closed loop mode)
// DFLL_USB_CRM_on()        enable USB clock recovery mode (crystalless USB)
// DFLL_USB_CRM_off()       disable USB clock recovery mode
//
// OSC32K_enable()          enable internal 32.768kHz RC oscillator (OSC32K)
// OSC32K_disable()         disable OSC32K
// OSC32K_ready()           check if OSC32K is stable
//
// XOSC32K_enable()         enable external 32.768kHz crystal oscillator (XOSC32K)
// XOSC32K_disable()        disable XOSC32K
// XOSC32K_ready()          check if XOSC32K is stable
//
// GCLK_setGEN(g,s,d)       set generic clock generator g (0..5) to source s with divider d
// GCLK_setCH(c,g)          connect generic clock channel (peripheral) c to generator g
// GCLK_stopCH(c)           disconnect generic clock channel c
//
// MCO_setSYS()             enable output system clock (GCLK0) on MCO pin (see below)
// MCO_stop()               disable clock output (MCO)
//
// Real-Time Clock (RTC) functions available (calendar mode):
// ----------------------------------------------------------
// RTC_init()               init RTC with OSC32K as clock source and 1s clock period
// RTC_init_XOSC32K()       init RTC with XOSC32K as clock source and 1s clock period
// RTC_setTime(h,m,s)       set time (hours, minutes, seconds), decimal values
// RTC_setDate(y,m,d)       set date (year 0..63 since RTC_YEAR_REF, month, day), decimal
// RTC_read()               read clock register (must be post-processed, see below)
// RTC_getSeconds(t)        get seconds of clock register value t (0..59)
// RTC_getMinutes(t)        get minutes of clock register value t (0..59)
// RTC_getHours(t)          get hours   of clock register value t (0..23)
// RTC_getDay(t)            get day     of clock register value t (1..31)
// RTC_getMonth(t)          get month   of clock register value t (1..12)
// RTC_getYear(t)           get year    of clock register value t (e.g. 2026)
// RTC_isInit()             check if RTC is running
//
// RTC_setAlarm(h,m,s)      set daily alarm (hours, minutes, seconds)
// RTC_stopAlarm()          stop alarm
// RTC_readAlarmFlag()      check alarm flag
// RTC_clearAlarmFlag()     clear alarm flag
// RTC_enableAlarmInt()     enable alarm interrupt (RTC_IRQHandler)
// RTC_disableAlarmInt()    disable alarm interrupt
//
// Low-Power Timer (LPT) functions available (uses RTC in counter mode):
// ---------------------------------------------------------------------
// LPT_init()               init and enable low-power timer (LPT, 1024Hz from OSCULP32K)
// LPT_shot(n)              restart LPT with period of n milliseconds (single shot)
// LPT_start(n)             start LPT in continuous mode with period of n milliseconds
// LPT_enable()             enable LPT
// LPT_disable()            disable LPT
// LPT_readReloadFlag()     read compare match flag (set by hardware at end of period)
// LPT_clearReloadFlag()    clear compare match flag
// LPT_enableReloadInt()    enable compare match interrupt (RTC_IRQHandler)
// LPT_disableReloadInt()   disable compare match interrupt
// LPT_sleep(n)             put device into STANDBY, wake-up after n milliseconds
// SLEEP_ms(n)              init LPT and put device into STANDBY for n milliseconds
// (LPT and RTC calendar functions cannot be used at the same time)
//
// Delay (DLY) and SysTick (STK) functions available:
// --------------------------------------------------
// DLY_ticks(n)             delay n+1 clock cycles (n = 0: no delay)
// DLY_us(n)                delay n microseconds
// DLY_ms(n)                delay n milliseconds
//
// STK_enable()             enable SYSTICK at system frequency
// STK_disable()            disable SYSTICK
//
// Watchdog Timer (IWDG/WDT) functions available:
// ----------------------------------------------
// IWDG_start(n)            start watchdog timer, n milliseconds, n<=16000 (see below)
// IWDG_reload(n)           reload watchdog counter with n milliseconds, n<=16000
// IWDG_feed()              feed the dog (reload last time)
// IWDG_stop()              stop watchdog timer
//
// Sleep functions available:
// --------------------------
// SLEEP_WFI_now()          put device into IDLE 0, wake up by interrupt (CPU stopped)
// SLEEP_WFE_now()          put device into IDLE 0, wake up by event
// IDLE2_WFI_now()          put device into IDLE 2, wake up by interrupt (CPU, AHB, APB)
// IDLE2_WFE_now()          put device into IDLE 2, wake up by event
// STDBY_WFI_now()          put device into STANDBY, wake up by interrupt
// STDBY_WFE_now()          put device into STANDBY, wake up by event
// (the clock configuration is retained, the DFLL48M restarts after wake-up)
//
// Reset (RST) and Bootloader (BOOT) functions available:
// ------------------------------------------------------
// BOOT_now()               jump to (SAM-BA/UF2) bootloader with double-tap support
// RST_now()                conduct software reset
// RST_clearFlags()         clear all reset flags (not necessary on SAMD)
// RST_wasIWDG()            check if last reset was caused by watchdog
// RST_wasSoftware()        check if last reset was caused by software
// RST_wasPower()           check if last reset was caused by POR/BOD
// RST_wasPin()             check if last reset was caused by RST pin low
//
// Cyclic Redundancy Check (CRC) functions available (DSU, IEEE 802.3 CRC-32):
// --------------------------------------------------------------------------
// CRC_calc(a,n,i)          calculate CRC-32 of n bytes (word aligned) at address a,
//                          start value i (use 0xffffffff), returns raw result
// CRC_calc32(a,n)          calculate standard CRC-32 (as zlib, PKZIP, Ethernet)
//
// Event System (EVSYS) functions available:
// -----------------------------------------
// EVSYS_init()             enable event system module
// EVSYS_connect(c,g,u)     connect event generator g with user u via channel c (0..5)
//                          (asynchronous path, e.g. EVSYS_ID_GEN_EIC_EXTINT_2,
//                          EVSYS_ID_USER_TC1_EVU, see samd11.h)
// EVSYS_disconnect(c,u)    disconnect channel c and user u
// EVSYS_trigger(c)         trigger software event on channel c
//
// Direct Memory Access Controller (DMA) functions available:
// ----------------------------------------------------------
// DMA_init()               init DMAC with descriptor memory (used by the libraries)
// DMA_DESC(c)              descriptor of DMA channel c (0..5)
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
//
// Notes:
// ------
// - Generic clock generators used by this template and the libraries:
//   GCLK0: main clock (F_CPU), GCLK1: 32.768kHz reference for the DFLL48M (only with
//   SYS_USE_XOSC32K), GCLK2: 1024Hz from OSCULP32K for WDT, GCLK3: 1024Hz for RTC/LPT,
//   GCLK4: 48MHz for USB (only if F_CPU < 48MHz), GCLK5: free for the user.
// - The watchdog timer runs at 1024Hz, the period is rounded up to the next valid
//   value (8, 16, 32, ... 16384 cycles, i.e. 8ms ... 16s).
// - MCO pin (set below): PA08 (default), PA24, PA25, PA27, PA30 or PA31 (GCLK_IO0).
// - The flash wait states are set for VDD >= 2.7V (1 wait state above 24MHz).
// - DFLL48M modes: closed loop with 32.768kHz crystal (SYS_USE_XOSC32K = 1), USB clock
//   recovery mode (SYS_USB_CRM = 1, recommended for crystalless USB, the DFLL is
//   adjusted to the USB start-of-frame packets) or open loop (factory calibration).
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "samd11.h"

// ===================================================================================
// System Options (set "1" to activate)
// ===================================================================================
#define SYS_CLK_INIT      1         // 1: init system clock on startup
#define SYS_TICK_INIT     1         // 1: init and start SYSTICK on startup
#define SYS_GPIO_EN       1         // 1: enable GPIO port on startup
#define SYS_CLEAR_BSS     1         // 1: clear uninitialized variables
#define SYS_USE_VECTORS   1         // 1: create interrupt vector table
#define SYS_USE_XOSC32K   0         // 1: use 32.768kHz crystal (PA08/PA09) for DFLL48M
#define SYS_USB_CRM       1         // 1: use USB clock recovery mode (if no XOSC32K)
#define MCO_PIN           8         // clock output pin number (8 = PA08, 24, 25, 27, 30, 31)

// ===================================================================================
// Sytem Clock Defines
// ===================================================================================

// Set system clock frequency
#ifndef F_CPU
  #define F_CPU           48000000  // 48Mhz if not otherwise defined
#endif

// Calculate system clock settings
#if   F_CPU == 48000000
  #define CLK_DIV         1         // DFLL48M / 1
#elif F_CPU == 24000000
  #define CLK_DIV         2         // DFLL48M / 2
#elif F_CPU == 16000000
  #define CLK_DIV         3         // DFLL48M / 3
#elif F_CPU == 12000000
  #define CLK_DIV         4         // DFLL48M / 4
#elif F_CPU ==  6000000
  #define CLK_DIV         8         // DFLL48M / 8
#elif F_CPU ==  8000000
  #define CLK_PRESC       0         // OSC8M / 1
#elif F_CPU ==  4000000
  #define CLK_PRESC       1         // OSC8M / 2
#elif F_CPU ==  2000000
  #define CLK_PRESC       2         // OSC8M / 4
#elif F_CPU ==  1000000
  #define CLK_PRESC       3         // OSC8M / 8
#else
  #warning Unsupported system clock frequency, using internal 48 MHz
  #define CLK_DIV         1
  #undef  F_CPU
  #define F_CPU           48000000
#endif

#ifdef CLK_DIV
  #define CLK_init        CLK_init_DFLL     // DFLL48M as system clock source
  #define CLK_DFLL        1                 // DFLL48M is running
#else
  #define CLK_init        CLK_init_OSC8M    // OSC8M as system clock source
  #define CLK_DFLL        0                 // DFLL48M is not running
#endif

// Generic clock generators
#define GCLK_SYS          0         // main clock (F_CPU)
#define GCLK_REF          1         // 32.768kHz reference for DFLL (SYS_USE_XOSC32K)
#define GCLK_WDT          2         // 1024Hz from OSCULP32K for WDT
#define GCLK_RTC          3         // 1024Hz for RTC and LPT
#define GCLK_48M          4         // 48MHz for USB (only if F_CPU < 48MHz)
#define GCLK_USER         5         // free for the user

// ===================================================================================
// System Clock Functions
// ===================================================================================

// System clock init functions (called automatically on system start)
void CLK_init_DFLL(void);     // init DFLL48M (divided) as system clock source
void CLK_init_OSC8M(void);    // init internal 8MHz oscillator (divided) as system clock
void CLK_reset(void);         // reset system clock to default state

// Internal 8MHz RC oscillator (OSC8M)
#define OSC8M_enable()      (SYSCTRL->OSC8M.reg |=  SYSCTRL_OSC8M_ENABLE)
#define OSC8M_disable()     (SYSCTRL->OSC8M.reg &= ~SYSCTRL_OSC8M_ENABLE)
#define OSC8M_ready()       (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC8MRDY)
#define OSC8M_divider(n)    (SYSCTRL->OSC8M.reg = (SYSCTRL->OSC8M.reg & ~SYSCTRL_OSC8M_PRESC_Msk) \
                                                | SYSCTRL_OSC8M_PRESC(n))

// Digital frequency locked loop (DFLL48M)
#define DFLL_ready()        (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_DFLLRDY)
#define DFLL_locked()       ((SYSCTRL->PCLKSR.reg & (SYSCTRL_PCLKSR_DFLLLCKC | SYSCTRL_PCLKSR_DFLLLCKF)) \
                                               == (SYSCTRL_PCLKSR_DFLLLCKC | SYSCTRL_PCLKSR_DFLLLCKF))
#define DFLL_USB_CRM_on()   (SYSCTRL->DFLLCTRL.reg |=  SYSCTRL_DFLLCTRL_USBCRM)
#define DFLL_USB_CRM_off()  (SYSCTRL->DFLLCTRL.reg &= ~SYSCTRL_DFLLCTRL_USBCRM)

// Internal 32.768kHz RC oscillator (OSC32K)
void OSC32K_enable(void);
#define OSC32K_disable()    (SYSCTRL->OSC32K.reg &= ~SYSCTRL_OSC32K_ENABLE)
#define OSC32K_ready()      (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_OSC32KRDY)

// External 32.768kHz crystal oscillator (XOSC32K) on PA08 (XIN32) / PA09 (XOUT32)
void XOSC32K_enable(void);
#define XOSC32K_disable()   (SYSCTRL->XOSC32K.reg &= ~SYSCTRL_XOSC32K_ENABLE)
#define XOSC32K_ready()     (SYSCTRL->PCLKSR.reg & SYSCTRL_PCLKSR_XOSC32KRDY)

// Generic clock generators (g: 0..5) and channels (c: e.g. SERCOM0_GCLK_ID_CORE)
// Source s: GCLK_SOURCE_OSC8M, GCLK_SOURCE_DFLL48M, GCLK_SOURCE_OSC32K, etc.
void GCLK_setGEN(uint8_t gen, uint8_t src, uint16_t div);
void GCLK_setCH(uint8_t ch, uint8_t gen);
void GCLK_stopCH(uint8_t ch);
#define GCLK_sync()         while(GCLK->STATUS.reg & GCLK_STATUS_SYNCBUSY)

// Clock output (MCO) of GCLK0 on GCLK_IO0 (MCO_PIN)
void MCO_setSYS(void);
void MCO_stop(void);

// ===================================================================================
// Real-Time Clock (RTC) Functions (calendar mode)
// ===================================================================================
#define RTC_YEAR_REF        2000    // reference year (YEAR = 0)

void RTC_init(void);                // init RTC with OSC32K, 1s clock period
void RTC_init_XOSC32K(void);        // init RTC with XOSC32K, 1s clock period
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_setDate(uint8_t year, uint8_t month, uint8_t day);
void RTC_setAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds);
uint32_t RTC_read(void);            // read clock register

#define RTC_getSeconds(t)       (((t) & RTC_MODE2_CLOCK_SECOND_Msk) >> RTC_MODE2_CLOCK_SECOND_Pos)
#define RTC_getMinutes(t)       (((t) & RTC_MODE2_CLOCK_MINUTE_Msk) >> RTC_MODE2_CLOCK_MINUTE_Pos)
#define RTC_getHours(t)         (((t) & RTC_MODE2_CLOCK_HOUR_Msk)   >> RTC_MODE2_CLOCK_HOUR_Pos)
#define RTC_getDay(t)           (((t) & RTC_MODE2_CLOCK_DAY_Msk)    >> RTC_MODE2_CLOCK_DAY_Pos)
#define RTC_getMonth(t)         (((t) & RTC_MODE2_CLOCK_MONTH_Msk)  >> RTC_MODE2_CLOCK_MONTH_Pos)
#define RTC_getYear(t)          ((((t) & RTC_MODE2_CLOCK_YEAR_Msk)  >> RTC_MODE2_CLOCK_YEAR_Pos) \
                                + RTC_YEAR_REF)
#define RTC_readTime()          RTC_read()
#define RTC_readDate()          RTC_read()

#define RTC_isInit()            (RTC->MODE2.CTRL.reg & RTC_MODE2_CTRL_ENABLE)
#define RTC_sync()              while(RTC->MODE2.STATUS.reg & RTC_STATUS_SYNCBUSY)
#define RTC_stopAlarm()         (RTC->MODE2.Mode2Alarm[0].MASK.reg = RTC_MODE2_MASK_SEL_OFF)
#define RTC_readAlarmFlag()     (RTC->MODE2.INTFLAG.reg & RTC_MODE2_INTFLAG_ALARM0)
#define RTC_clearAlarmFlag()    (RTC->MODE2.INTFLAG.reg = RTC_MODE2_INTFLAG_ALARM0)
#define RTC_enableAlarmInt()    (RTC->MODE2.INTENSET.reg = RTC_MODE2_INTENSET_ALARM0, \
                                 NVIC_EnableIRQ(RTC_IRQn))
#define RTC_disableAlarmInt()   (RTC->MODE2.INTENCLR.reg = RTC_MODE2_INTENCLR_ALARM0)

// ===================================================================================
// Low-Power Timer (LPT) Functions (RTC in 32-bit counter mode, 1024Hz)
// ===================================================================================
void LPT_init(void);            // init and enable low-power timer (LPT)
void LPT_shot(uint32_t ms);     // restart low-power timer with period in ms
void LPT_start(uint32_t ms);    // start LPT in continuous mode with period in ms
void LPT_sleep(uint32_t ms);    // put device into STANDBY, wake-up after period in ms

#define LPT_enable()            (RTC->MODE0.CTRL.reg |=  RTC_MODE0_CTRL_ENABLE)
#define LPT_disable()           (RTC->MODE0.CTRL.reg &= ~RTC_MODE0_CTRL_ENABLE)
#define LPT_readReloadFlag()    (RTC->MODE0.INTFLAG.reg & RTC_MODE0_INTFLAG_CMP0)
#define LPT_clearReloadFlag()   (RTC->MODE0.INTFLAG.reg = RTC_MODE0_INTFLAG_CMP0)
#define LPT_enableReloadInt()   (RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0, \
                                 NVIC_EnableIRQ(RTC_IRQn))
#define LPT_disableReloadInt()  (RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0)

#define SLEEP_ms(ms)            (LPT_init(), LPT_sleep(ms))

// ===================================================================================
// SYSTICK Functions
// ===================================================================================
#define STK_enable()      (SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE)
#define STK_disable()     (SysTick->CTRL = 0)

// ===================================================================================
// Delay (DLY) Functions (using SYSTICK)
// ===================================================================================
#define DLY_US_TIME       (F_CPU / 1000000)             // system ticks per us
#define DLY_MS_TIME       (F_CPU / 1000)                // system ticks per ms
void DLY_ticks(uint32_t n);                             // delay n+1 clock cycles
static inline void DLY_us(uint32_t us) {                // delay n microseconds
  uint32_t t;
  while(us >= 1000) { DLY_ticks(DLY_MS_TIME - 1); us -= 1000; } // no 32-bit overflow
  t = us * DLY_MS_TIME / 1000;                          // works also for F_CPU < 1MHz
  if(t) DLY_ticks(t - 1);
}
static inline void DLY_ms(uint32_t ms) {                // delay n milliseconds
  while(ms--) DLY_ticks(DLY_MS_TIME - 1);
}

// ===================================================================================
// Watchdog Timer (IWDG/WDT) Functions
// ===================================================================================
void IWDG_start(uint16_t ms);                           // start WDT with time in ms
void IWDG_reload(uint16_t ms);                          // reload WDT with time in ms
void IWDG_stop(void);                                   // stop WDT
static inline void IWDG_feed(void) {                    // feed the dog (reload time)
  while(WDT->STATUS.reg & WDT_STATUS_SYNCBUSY);
  WDT->CLEAR.reg = WDT_CLEAR_CLEAR_KEY;
}

// ===================================================================================
// Sleep Functions
// ===================================================================================
void SLEEP_WFI_now(void);       // put device into IDLE 0,  wake up by interrupt
void SLEEP_WFE_now(void);       // put device into IDLE 0,  wake up by event
void IDLE2_WFI_now(void);       // put device into IDLE 2,  wake up by interrupt
void IDLE2_WFE_now(void);       // put device into IDLE 2,  wake up by event
void STDBY_WFI_now(void);       // put device into STANDBY, wake up by interrupt
void STDBY_WFE_now(void);       // put device into STANDBY, wake up by event

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
#define RST_now()           NVIC_SystemReset()
#define RST_clearFlags()    // reset cause is updated automatically on every reset
#define RST_wasIWDG()       (PM->RCAUSE.reg & PM_RCAUSE_WDT)
#define RST_wasWDT()        (PM->RCAUSE.reg & PM_RCAUSE_WDT)
#define RST_wasSoftware()   (PM->RCAUSE.reg & PM_RCAUSE_SYST)
#define RST_wasPower()      (PM->RCAUSE.reg & (PM_RCAUSE_POR | PM_RCAUSE_BOD12 | PM_RCAUSE_BOD33))
#define RST_wasPin()        (PM->RCAUSE.reg & PM_RCAUSE_EXT)

// ===================================================================================
// Bootloader (BOOT) Functions
// ===================================================================================
// Works with SAM-BA/UF2 bootloaders with double-tap support (e.g. Arduino/MattairTech)
#define BOOT_MAGIC_ADDR     (HMCRAMC0_ADDR + HMCRAMC0_SIZE - 4)
#define BOOT_MAGIC_VALUE    0x07738135
void BOOT_now(void);        // jump to bootloader

// ===================================================================================
// Cyclic Redundancy Check (CRC) Functions (using the DSU)
// ===================================================================================
uint32_t CRC_calc(const void* addr, uint32_t len, uint32_t init);
#define CRC_calc32(a, n)    (~CRC_calc(a, n, 0xffffffff))

// ===================================================================================
// Event System (EVSYS) Functions
// ===================================================================================
#define EVSYS_init()        (PM->APBCMASK.reg |= PM_APBCMASK_EVSYS)
#define EVSYS_connect(c,g,u) (EVSYS->USER.reg = EVSYS_USER_CHANNEL((c)+1) | EVSYS_USER_USER(u), \
                              EVSYS->CHANNEL.reg = EVSYS_CHANNEL_CHANNEL(c)          \
                                                 | EVSYS_CHANNEL_EVGEN(g)            \
                                                 | EVSYS_CHANNEL_PATH_ASYNCHRONOUS)
#define EVSYS_disconnect(c,u) (EVSYS->USER.reg = EVSYS_USER_USER(u),                 \
                               EVSYS->CHANNEL.reg = EVSYS_CHANNEL_CHANNEL(c))
void EVSYS_trigger(uint8_t ch);   // trigger software event on channel ch

// ===================================================================================
// Direct Memory Access Controller (DMA) Functions
// ===================================================================================
// The DMAC uses one descriptor and one write-back descriptor per channel in SRAM
extern DmacDescriptor DMA_descriptor[DMAC_CH_NUM];
extern DmacDescriptor DMA_writeback[DMAC_CH_NUM];
#define DMA_DESC(c)         (DMA_descriptor[c])
void DMA_init(void);        // init DMAC (can be called multiple times)

// ===================================================================================
// Device Electronic Signature (ESIG)
// ===================================================================================
#define ESIG_FLASHSIZE      (FLASH_SIZE >> 10)            // flash size in KB
#define ESIG_DID            (DSU->DID.reg)                // device identification
#define ESIG_UID0           (*(__I uint32_t*)(0x0080A00C)) // 128-bit unique serial number
#define ESIG_UID1           (*(__I uint32_t*)(0x0080A040))
#define ESIG_UID2           (*(__I uint32_t*)(0x0080A044))
#define ESIG_UID3           (*(__I uint32_t*)(0x0080A048))

// ===================================================================================
// Interrupt (INT) Functions
// ===================================================================================
#define INT_enable()          __enable_irq()
#define INT_disable()         __disable_irq()
#define INT_ATOMIC_BLOCK      for(INT_ATOMIC_RESTORE, __ToDo = 1; __ToDo; __ToDo = 0)
#define INT_ATOMIC_RESTORE    uint32_t __reg_save __attribute__((__cleanup__(__iRestore))) = __iSave()

// Save interrupt status and disable interrupts
static inline uint32_t __iSave(void) {
  uint32_t result;
  __ASM volatile("MRS %0, primask" : "=r" (result));
  __ASM volatile("cpsid i" ::: "memory");
  return result;
}

// Restore interrupt status
static inline void __iRestore(const uint32_t *__s) {
  __ASM volatile("MSR primask, %0" :: "r" (*__s) : "memory");
}

// ===================================================================================
// Imported System Functions from cmsis_gcc.h and core_cm0plus.h
// ===================================================================================
#define NVIC_USER_IRQ_OFFSET       16

// The following EXC_RETURN values are saved the LR on exception entry
#define EXC_RETURN_HANDLER         (0xFFFFFFF1UL)     // return to Handler mode, uses MSP after return
#define EXC_RETURN_THREAD_MSP      (0xFFFFFFF9UL)     // return to Thread mode, uses MSP after return
#define EXC_RETURN_THREAD_PSP      (0xFFFFFFFDUL)     // return to Thread mode, uses PSP after return

// Interrupt Priorities are WORD accessible only under Armv6-M
// The following MACROS handle generation of the register offset and byte masks
#define _BIT_SHIFT(IRQn)         (  ((((uint32_t)(int32_t)(IRQn))         )      &  0x03UL) * 8UL)
#define _SHP_IDX(IRQn)           ( (((((uint32_t)(int32_t)(IRQn)) & 0x0FUL)-8UL) >>    2UL)      )
#define _IP_IDX(IRQn)            (   (((uint32_t)(int32_t)(IRQn))                >>    2UL)      )

#define NVIC_SetPriorityGrouping(X) (void)(X)
#define NVIC_GetPriorityGrouping()  (0U)

// Ignore some GCC warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"

#if defined (__thumb__) && !defined (__thumb2__)
#define __CMSIS_GCC_OUT_REG(r) "=l" (r)
#define __CMSIS_GCC_USE_REG(r) "l" (r)
#else
#define __CMSIS_GCC_OUT_REG(r) "=r" (r)
#define __CMSIS_GCC_USE_REG(r) "r" (r)
#endif

// Enable Interrupts
__attribute__( ( always_inline ) ) __STATIC_INLINE void __enable_irq(void) {
  __ASM volatile ("cpsie i" : : : "memory");
}

// Disable Interrupts
__attribute__( ( always_inline ) ) __STATIC_INLINE void __disable_irq(void) {
  __ASM volatile ("cpsid i" : : : "memory");
}

// Get Control Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_CONTROL(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}

// Set Control Register
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_CONTROL(uint32_t control) {
  __ASM volatile ("MSR control, %0" : : "r" (control) : "memory");
}

// Get IPSR Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_IPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}

// Get APSR Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_APSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, apsr" : "=r" (result) );
  return(result);
}

// Get xPSR Register
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_xPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, xpsr" : "=r" (result) );
  return(result);
}

// Get Process Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_PSP(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, psp\n"  : "=r" (result) );
  return(result);
}

// Set Process Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_PSP(uint32_t topOfProcStack) {
  __ASM volatile ("MSR psp, %0\n" : : "r" (topOfProcStack) : );
}

// Get Main Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_MSP(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, msp\n" : "=r" (result) );
  return(result);
}

// Set Main Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_MSP(uint32_t topOfMainStack) {
  __ASM volatile ("MSR msp, %0\n" : : "r" (topOfMainStack) : );
}

// Get Priority Mask
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_PRIMASK(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, primask" : "=r" (result) );
  return(result);
}

// Set Priority Mask
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_PRIMASK(uint32_t priMask) {
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

// No Operation
__attribute__((always_inline)) __STATIC_INLINE void __NOP(void) {
  __ASM volatile ("nop");
}

// Wait For Interrupt
__attribute__((always_inline)) __STATIC_INLINE void __WFI(void) {
  __ASM volatile ("wfi");
}

// Wait For Event
__attribute__((always_inline)) __STATIC_INLINE void __WFE(void) {
  __ASM volatile ("wfe");
}

// Send Event
__attribute__((always_inline)) __STATIC_INLINE void __SEV(void) {
  __ASM volatile ("sev");
}

// Instruction Synchronization Barrier
__attribute__((always_inline)) __STATIC_INLINE void __ISB(void) {
  __ASM volatile ("isb 0xF":::"memory");
}

// Data Synchronization Barrier
__attribute__((always_inline)) __STATIC_INLINE void __DSB(void) {
  __ASM volatile ("dsb 0xF":::"memory");
}

// Data Memory Barrier
__attribute__((always_inline)) __STATIC_INLINE void __DMB(void) {
  __ASM volatile ("dmb 0xF":::"memory");
}

// Reverse byte order (32 bit)
__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV(uint32_t value) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
  return __builtin_bswap32(value);
#else
  uint32_t result;
  __ASM volatile ("rev %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
#endif
}

// Reverse byte order (16 bit)
__attribute__((always_inline)) __STATIC_INLINE uint32_t __REV16(uint32_t value) {
  uint32_t result;
  __ASM volatile ("rev16 %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
}

// Reverse byte order in signed short value
__attribute__((always_inline)) __STATIC_INLINE int32_t __REVSH(int32_t value) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
  return (short)__builtin_bswap16(value);
#else
  int32_t result;
  __ASM volatile ("revsh %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return(result);
#endif
}

// Rotate Right in unsigned value (32 bit)
__attribute__((always_inline)) __STATIC_INLINE uint32_t __ROR(uint32_t op1, uint32_t op2) {
  return (op1 >> op2) | (op1 << (32U - op2));
}

// Breakpoint
#define __BKPT(value)     __ASM volatile ("bkpt "#value)

// Reverse bit order of value
__attribute__((always_inline)) __STATIC_INLINE uint32_t __RBIT(uint32_t value) {
  uint32_t result;
  int32_t s = 4 /*sizeof(v)*/ * 8 - 1; // extra shift needed at end
  result = value;                      // r will be reversed bits of v; first get LSB of v
  for (value >>= 1U; value; value >>= 1U) {
    result <<= 1U;
    result |= value & 1U;
    s--;
  }
  result <<= s;                        // shift when v's highest bits are zero
  return(result);
}

// Count leading zeros
#define __CLZ             __builtin_clz

#pragma GCC diagnostic pop

// Enable External Interrupt
__STATIC_INLINE void NVIC_EnableIRQ(IRQn_Type IRQn) {
  NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Disable External Interrupt
__STATIC_INLINE void NVIC_DisableIRQ(IRQn_Type IRQn) {
  NVIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Get Pending Interrupt
__STATIC_INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) {
  return((uint32_t)(((NVIC->ISPR[0U] & (1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
}

// Set Pending Interrupt
__STATIC_INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
  NVIC->ISPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Clear Pending Interrupt
__STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
  NVIC->ICPR[0U] = (uint32_t)(1UL << (((uint32_t)(int32_t)IRQn) & 0x1FUL));
}

// Set Interrupt Priority
__STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
  if ((int32_t)(IRQn) < 0) {
    SCB->SHP[_SHP_IDX(IRQn)] = ((uint32_t)(SCB->SHP[_SHP_IDX(IRQn)] & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
  else {
    NVIC->IP[_IP_IDX(IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(IRQn)]  & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
}

// Get Interrupt Priority
__STATIC_INLINE uint32_t NVIC_GetPriority(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) < 0) {
    return((uint32_t)(((SCB->SHP[_SHP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
  }
  else {
    return((uint32_t)(((NVIC->IP[ _IP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
  }
}

// System Reset
__STATIC_INLINE void NVIC_SystemReset(void) {
  __DSB();  // Ensure all outstanding memory accesses included buffered write are completed before reset
  SCB->AIRCR  = ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
  __DSB();                                                          // Ensure completion of memory access
  for(;;) __NOP();                                                  // wait until reset
}

// System Tick Configuration
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks) {
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) return (1UL);        // Reload value impossible
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);                         // set reload register
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); // set Priority for Systick Interrupt
  SysTick->VAL   = 0UL;                                             // Load the SysTick Counter Value
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                   SysTick_CTRL_TICKINT_Msk   |
                   SysTick_CTRL_ENABLE_Msk;                         // Enable SysTick IRQ and SysTick Timer
  return (0UL);                                                     // Function successful
}

#ifdef __cplusplus
};
#endif
