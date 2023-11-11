// ===================================================================================
// Basic System Functions for STM32L010, STM32L011, STM32L021, STM32L031      * v0.1 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are 
// set up automatically on system start.
//
// Functions available:
// --------------------
// CLK_init_MSI()           init internal multi-speed oscillator as system clock source
// CLK_init_HSI()           init internal oscillator (non PLL) as system clock source
// CLK_init_HSI_PLL()       init internal oscillator with PLL as system clock source
// CLK_init_HSE()           init external crystal (non PLL) as system clock source
// CLK_init_HSE_PLL()       init external crystal with PLL as system clock source
// CLK_reset()              reset system clock to default state
//
// MSI_enable()             enable internal multi-speed clock (MSI)
// MSI_disable()            disable MSI
// MSI_ready()              check if MSI is stable
// MSI_range(n)             set MSI clock range F_MSI = 2^(n+16)
//
// HSI_enable()             enable internal 16MHz high-speed clock (HSI)
// HSI_disable()            disable HSI
// HSI_ready()              check if HSI is stable
// HSI_DIV_enable()         enable HSI division by 4
// HSI_DIV_disable()        disable HSI division by 4
//
// HSE_enable()             enable external high-speed clock (HSE)
// HSE_disable()            disable HSE
// HSE_ready()              check if HSE is stable
// HSE_bypass_on()          enable HSE clock bypass
// HSE_bypass_off()         disable HSE clock bypass
// HSE_CSS_on()             enable HSE clock security system
// HSE_CSS_off()            disable HSE clock security system
//
// LSI_enable()             enable internal 37kHz low-speed clock (LSI)
// LSI_disable()            disable LSI
// LSI_ready()              check if LSI is stable
//
// LSE_enable()             enable external 32.768kHz low-speed clock (LSE)
// LSE_disable()            disable LSE
// LSE_ready()              check if LSE is stable
// LSE_bypass_on()          enable LSE clock bypass
// LSE_bypass_off()         disable LSE clock bypass
// LSE_CSS_on()             enable LSE clock security system
// LSE_CSS_off()            disable LSE clock security system
//
// PLL_enable()             enable phase-locked loop (PLL)
// PLL_disable()            disable PLL
// PLL_ready()              check if PLL is stable
// PLL_factor(n)            set PLL multiplication factor (PLL must be disabled)
// PLL_divider(n)           set PLL output divider (2 - 4) (PLL must be disabled)
// PLL_setHSI()             set HSI as input for PLL (PLL must be disabled)
// PLL_setHSE()             set HSE as input for PLL (PLL must be disabled)
//
// MCO_setSYS()             enable output system clock (SYS_CLK) on MCO
// MCO_setHSI()             enable output internal high-speed clock (HSI) on MCO
// MCO_setMSI()             enable output internal multi-speed clock (HSI) on MCO
// MCO_setHSE()             enable output external high-speed clock (HSE) on MCO
// MCO_setPLL()             enable output PLL on MCO
// MCO_setLSI()             enable output internal low-speed clock (LSI) on MCO
// MCO_setLSE()             enable output external low-speed clock (LSE) on MCO
// MCO_setPRE(n)            set MCO prescaler to 2^n (set before enabling MCO)
// MCO_stop()               disable clock output (MCO)
//
// RTC_init()               init RTC with LSI as clock source and 1s clock period
// RTC_init_LSE()           init RTC with LSE as clock source and 1s clock period
// RTC_setPrescaler(a,s)    set RTC prescaler (async, sync)
// RTC_setTime(h,m,s)       set time (hours, minutes, seconds) in BCD format
// RTC_setDate(y,w,m,d)     set date (year, week day, month, day) in BCD format
// RTC_readTime()           read time register (must be post-processed)
// RTC_readDate()           read date register (must be post-processed)
// RTC_isInit()             check if RTC time/date were initialized (e.g. after reset)
//
// RTC_startWakeupTimer(n)  set wakeup timer with period of n milliseconds
// RTC_stopWakeupTimer()    stop wakeup timer
// RTC_readWakeupFlag()     check wakeup flag (set after each period, must be cleared)
// RTC_clearWakeupFlag()    clear wakeup flag
// RTC_enableWakeupInt()    enable wakeup timer interrupt
// RTC_disableWakeupInt()   disable wakeup timer interrupt
//
// RTC_setAlarmA(w,h,m,s)   set alarm A (week day, hours, minutes, seconds) in BCD
// RTC_setAlarmB(w,h,m,s)   set alarm B (week day, hours, minutes, seconds) in BCD
// RTC_readAlarmAFlag()     check alarm A flag
// RTC_readAlarmAFlag()     check alarm B flag
// RTC_clearAlarmAFlag()    clear alarm A flag
// RTC_clearAlarmBFlag()    clear alarm B flag
// RTC_enableAlarmAInt()    enable alarm A interrupt
// RTC_disableAlarmAInt()   disable alarm A interrupt
// RTC_enableAlarmBInt()    enable alarm B interrupt
// RTC_disableAlarmBInt()   disabele alarm B interrupt
//
// RTC_enableWakeEvent()    enable CPU wakeup with RTC event
// RTC_disableWakeEvent()   disable CPU wakeup with RTC event
// RTC_enableWakeInt()      enable CPU wakeup with RTC interrupt
// RTC_disableWakeInt()     disable CPU wakeup with RTC interrupt
//
// STK_enable()             enable SYSTICK at system frequency
// STK_disable()            disable SYSTICK
//
// DLY_ticks(n)             delay n+1 clock cycles
// DLY_us(n)                delay n microseconds
// DLY_ms(n)                delay n milliseconds
//
// IWDG_start(n)            start independent watchdog timer, n milliseconds, n<=8191
// IWDG_reload(n)           reload watchdog counter with n milliseconds, n<=8191
// IWDG_feed()              feed the dog (reload last time)
//
// LPT_init()               init and enable low-power timer (LPT)
// LPT_shot(n)              start LPT single shot with period of n milliseconds
// LPT_start(n)             start LPT in continuous mode with period of n milliseconds
// LPT_enable()             enable LPT
// LPT_disable()            disable LPT
// LPT_readReloadFlag()     read auto reload flag (set by hardware at end of period)
// LPT_clearReloadFlag()    clear auto reload flag
// LPT_enableReloadInt()    enable auto reload interrupt
// LPT_disableReloadInt()   disable auto reload interrupt
//
// LPT_enableWakeEvent()    enable CPU wakeup with LPT event
// LPT_disableWakeEvent()   disable CPU wakeup with LPT event
// LPT_enableWakeInt()      enable CPU wakeup with LPT interrupt
// LPT_disableWakeInt()     disable CPU wakeup with LPT interrupt
// LPT_sleep(n)             put device in to SLEEP,  wake-up after n milliseconds
//
// SLEEP_WFI_now()          put device into SLEEP,   wake up by interrupt
// SLEEP_WFE_now()          put device into SLEEP,   wake up by event
// STOP_WFI_now()           put device into STOP,    wake up by interrupt
// STOP_WFE_now()           put device into STOP,    wake up by event
// STDBY_WFI_now()          put device into STANDBY, wake up by interrupt
// STDBY_WFE_now()          put device into STANDBY, wake up by event
//
// BOOT_now()               jump to bootloader
// RST_now()                conduct software reset
// RST_clearFlags()         clear all reset flags
// RST_wasWWDG()            check if last reset was caused by window watchdog
// RST_wasIWDG()            check if last reset was caused by independent watchdog
// RST_wasSoftware()        check if last reset was caused by software
// RST_wasPower()           check if last reset was caused by BOR/POR/PDR
// RST_wasPin()             check if last reset was caused by RST pin low
// RST_wasOption()          check if last reset was caused by OPTION byte loader
//
// CRC_enable()             enable cyclic redundancy check (CRC) module
// CRC_disable()            disable CRC module
// CRC_write(w)             add new 32-bit word for CRC calculation
// CRC_write32(w)           add new 32-bit word for CRC calculation (alias)
// CRC_write16(h)           add new 16-bit half-word for CRC calculation
// CRC_write8(b)            add new  8-bit byte for CRC calculation
// CRC_read()               read last 32-bit CRC calculation
// CRC_read32()             read last 32-bit CRC calculation (alias)
// CRC_read16()             read last 16-bit CRC calculation
// CRC_read8()              read last  8-bit CRC calculation
// CRC_reset()              reset CRC calculation
// CRC_setInit(n)           set initial CRC value (default: 0xffffffff)
// CRC_setPoly(n)           set polynomial (default: 0x04c11db7)
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32l0xx.h"

// ===================================================================================
// System Options (set "1" to activate)
// ===================================================================================
#define SYS_CLK_INIT      1         // 1: init system clock on startup
#define SYS_TICK_INIT     1         // 1: init and start SYSTICK on startup
#define SYS_GPIO_EN       1         // 1: enable GPIO ports A-C on startup
#define SYS_CLEAR_BSS     0         // 1: clear uninitialized variables
#define SYS_USE_VECTORS   0         // 1: create interrupt vector table
#define SYS_USE_HSE       0         // 1: use external crystal

// ===================================================================================
// Sytem Clock Defines
// ===================================================================================

// Set system clock frequency
#ifndef F_CPU
  #define F_CPU           2097152   // default clock frequency
#endif

// Calculate system clock settings
#if SYS_USE_HSE == 0
  #if   F_CPU == 32000000
    #define CLK_init      CLK_init_HSI_PLL  // 16MHz
    #define CLK_PLL_MUL   (0b0001<<18)      // *4
    #define CLK_PLL_DIV   (0b01<<22)        // /2 = 32MHz
  #elif F_CPU == 24000000
    #define CLK_init      CLK_init_HSI_PLL  // 16MHz
    #define CLK_PLL_MUL   (0b0000<<18)      // *3
    #define CLK_PLL_DIV   (0b01<<22)        // /2 = 24MHz
  #elif F_CPU == 16000000
    #define CLK_init      CLK_init_HSI      // 16MHz
    #define CLK_HSI_DIV   0                 // /1 = 16MHz
  #elif F_CPU == 12000000
    #define CLK_init      CLK_init_HSI_PLL  // 16MHz
    #define CLK_PLL_MUL   (0b0000<<18)      // *3
    #define CLK_PLL_DIV   (0b11<<22)        // /4 = 12MHz
  #elif F_CPU ==  8000000
    #define CLK_init      CLK_init_HSI_PLL  // 16MHz
    #define CLK_PLL_MUL   (0b0010<<18)      // *6
    #define CLK_PLL_DIV   (0b10<<22)        // /3 = 8MHz
  #elif F_CPU ==  4000000
    #define CLK_init      CLK_init_HSI      // 16MHz
    #define CLK_HSI_DIV   (1<<3)            // /4 = 4MHz
  #elif F_CPU ==  4194302
    #define CLK_init      CLK_init_MSI
    #define CLK_MSI_DIV   (0b110<<13)
  #elif F_CPU ==  2097152
    #define CLK_init      CLK_init_MSI
    #define CLK_MSI_DIV   (0b101<<13)
  #elif F_CPU ==  1048576
    #define CLK_init      CLK_init_MSI
    #define CLK_MSI_DIV   (0b100<<13)
  #elif F_CPU ==   524288
    #define CLK_init      CLK_init_MSI
    #define CLK_MSI_DIV   (0b011<<13)
  #elif F_CPU ==   262144
    #define CLK_init      CLK_init_MSI
    #define CLK_MSI_DIV   (0b010<<13)
  #elif F_CPU ==   131072
    #define CLK_init      CLK_init_MSI
    #define CLK_MSI_DIV   (0b001<<13)
  #elif F_CPU ==    65536
    #define CLK_init      CLK_init_MSI
    #define CLK_MSI_DIV   (0b000<<13)
  #else
    #warning Unsupported system clock frequency, using internal 2097152 Hz
    #define CLK_init      CLK_init_MSI
    #define CLK_MSI_DIV   (0b101<<13)
    #undef  F_CPU
    #define F_CPU         2097152  
  #endif

#else
  #define CLK_init        CLK_init_HSE
#endif

#ifndef CLK_MSI_DIV
  #define CLK_MSI_DIV     (0b101<<13)
#endif
#ifndef CLK_HSI_DIV
  #define CLK_HSI_DIV     0
#endif
#ifndef CLK_PLL_DIV
  #define CLK_PLL_DIV     0
#endif
#ifndef CLK_PLL_MUL
  #define CLK_PLL_MUL     0
#endif

// ===================================================================================
// System Clock Functions
// ===================================================================================

// System clock init functions (called automatically on system start)
void CLK_init_MSI(void);      // init internal multi-speed oscillator as system clock source
void CLK_init_HSI(void);      // init internal oscillator (non PLL) as system clock source
void CLK_init_HSI_PLL(void);  // init internal oscillator with PLL as system clock source
void CLK_init_HSE(void);      // init external crystal (non PLL) as system clock source
void CLK_init_HSE_PLL(void);  // init external crystal with PLL as system clock source
void CLK_reset(void);         // reset system clock to default state

// Internal high-speed clock (HSI) functions
#define HSI_enable()      RCC->CR |=  RCC_CR_HSION        // enable HSI
#define HSI_disable()     RCC->CR &= ~RCC_CR_HSION        // disable HSI
#define HSI_ready()       (RCC->CR & RCC_CR_HSIRDY)       // check if HSI is stable
#define HSI_DIV_enable()  RCC->CR |=  RCC_CR_HSIDIVEN     // enable HSI division by 4
#define HSI_DIV_disable() RCC->CR &= ~RCC_CR_HSIDIVEN     // disable HSI division by 4

// External high-speed clock (HSE) functions
#define HSE_enable()      RCC->CR |=  RCC_CR_HSEON        // enable HSE
#define HSE_disable()     RCC->CR &= ~RCC_CR_HSEON        // disable HSE
#define HSE_ready()       (RCC->CR & RCC_CR_HSERDY)       // check if HSE is stable
#define HSE_bypass_on()   RCC->CR |=  RCC_CR_HSEBYP       // enable HSE clock bypass
#define HSE_bypass_off()  RCC->CR &= ~RCC_CR_HSEBYP       // disable HSE clock bypass
#define HSE_CSS_on()      RCC->CR |=  RCC_CR_CSSHSEON     // enable HSE clock security
#define HSE_CSS_off()     RCC->CR &= ~RCC_CR_CSSHSEON     // enable HSE clock security

// Internal multi-speed clock (MSI) functions
#define MSI_enable()      RCC->CR |=  RCC_CR_MSION        // enable MSI
#define MSI_disable()     RCC->CR &= ~RCC_CR_MSION        // disable MSI
#define MSI_ready()       (RCC->CR & RCC_CR_MSIRDY)       // check if MSI is stable
#define MSI_range(n)      (RCC->ICSCR & ~RCC_ICSCR_MSIRANGE) | (((n)&7) << 13)

// Internal 37kHz low-speed clock (LSI) functions
#define LSI_enable()      RCC->CSR |=  RCC_CSR_LSION      // enable LSI
#define LSI_disable()     RCC->CSR &= ~RCC_CSR_LSION      // disable LSI
#define LSI_ready()       (RCC->CSR & RCC_CSR_LSIRDY)     // check if LSI is stable

// External 32.768kHz low-speed clock (LSE) functions
#define LSE_enable()      RCC->CSR |=  RCC_CSR_LSEON      // enable LSE
#define LSE_disable()     RCC->CSR &= ~RCC_CSR_LSEON      // disable LSE
#define LSE_ready()       (RCC->CSR & RCC_CSR_LSERDY)     // check if LSE is stable
#define LSE_bypass_on()   RCC->CSR |=  RCC_CSR_LSEBYP     // enable LSE clock bypass
#define LSE_bypass_off()  RCC->CSR &= ~RCC_CSR_LSEBYP     // disable LSE clock bypass
#define LSE_CSS_on()      RCC->CSR |=  RCC_CSR_CSSLSEON   // enable LSE clock security
#define LSE_CSS_off()     RCC->CSR &= ~RCC_CSR_CSSLSEON   // enable LSE clock security

// PLL functions
#define PLL_enable()      RCC->CR |=  RCC_CR_PLLON        // enable PLL
#define PLL_disable()     RCC->CR &= ~RCC_CR_PLLON        // disable PLL
#define PLL_ready()       (RCC->CR & RCC_CR_PLLRDY)       // check if PLL is stable
#define PLL_factor(n)     (RCC->CFGR & RCC_CFGR_PLLMUL) | (((n)&15) << 18)
#define PLL_divider(n)    (RCC->CFGR & ~RCC_CFGR_PLLDIV) | (((n-1)&3) << 4) // 2-4
#define PLL_setHSI()      RCC->CFGR &= ~RCC_CFGR_PLLSRC   // set HSI as PLL input
#define PLL_setHSE()      RCC->CFGR |=  RCC_CFGR_PLLSRC   // set HSE as PLL input

// Clock output functions (MCO)
#define MCO_setSYS()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0001 << 24)
#define MCO_setHSI()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0010 << 24)
#define MCO_setMSI()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0011 << 24)
#define MCO_setHSE()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0100 << 24)
#define MCO_setPLL()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0101 << 24)
#define MCO_setLSI()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0110 << 24)
#define MCO_setLSE()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0111 << 24)
#define MCO_setPRE(n)     RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOPRE) | (((n)&7) << 28)
#define MCO_stop()        RCC->CFGR &= ~RCC_CFGR_MCOSEL

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================
void RTC_init(void);      // init RTC with LSI as clock source and 1s clock period
void RTC_init_LSE(void);  // init RTC with LSE as clock source and 1s clock period
void RTC_setPrescaler(uint8_t async, uint16_t sync);
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_setDate(uint8_t year, uint8_t weekday, uint8_t month, uint8_t day);
uint32_t RTC_readTime(void);
uint32_t RTC_readDate(void);
void RTC_setAlarmA(uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_setAlarmB(uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds);
void RTC_startWakeupTimer(uint16_t ms);

#define RTC_isInit()            (RTC->ISR & RTC_ISR_INITS)
#define RTC_stopWakeupTimer()   RTC->CR &= ~RTC_CR_WUTE
#define RTC_readWakeupFlag()    (RTC->ISR & RTC_ISR_WUTF)
#define RTC_readAlarmAFlag()    (RTC->ISR & RTC_ISR_ALRAF)
#define RTC_readAlarmBFlag()    (RTC->ISR & RTC_ISR_ALRBF)
#define RTC_clearWakeupFlag()   RTC->ISR &= ~RTC_ISR_WUTF
#define RTC_clearAlarmAFlag()   RTC->ISR &= ~RTC_ISR_ALRAF
#define RTC_clearAlarmBFlag()   RTC->ISR &= ~RTC_ISR_ALRBF
#define RTC_enableWakeupInt()   RTC->CR |=  RTC_CR_WUTIE
#define RTC_disableWakeupInt()  RTC->CR &= ~RTC_CR_WUTIE
#define RTC_enableAlarmAInt()   RTC->CR |=  RTC_CR_ALRAIE
#define RTC_disableAlarmAInt()  RTC->CR &= ~RTC_CR_ALRAIE
#define RTC_enableAlarmBInt()   RTC->CR |=  RTC_CR_ALRBIE
#define RTC_disableAlarmBInt()  RTC->CR &= ~RTC_CR_ALRBIE
#define RTC_enableWakeEvent()   EXTI->EMR |=  ((uint32_t)1 << 20)
#define RTC_disableWakeEvent()  EXTI->EMR &= ~((uint32_t)1 << 20)
#define RTC_enableWakeInt()     EXTI->IMR |=  ((uint32_t)1 << 20)
#define RTC_disableWakeInt()    EXTI->IMR &= ~((uint32_t)1 << 20)

// ===================================================================================
// SYSTICK Functions
// ===================================================================================
#define STK_enable()      SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE
#define STK_disable()     SysTick->CTRL = 0

// ===================================================================================
// Delay Functions (using SYSTICK)
// ===================================================================================
#define DLY_US_TIME       (F_CPU / 1000000)             // system ticks per us
#define DLY_MS_TIME       (F_CPU / 1000)                // system ticks per ms
#define DLY_us(n)         DLY_ticks((n)*DLY_US_TIME-1)  // delay n microseconds
void DLY_ticks(uint32_t n);                             // delay n+1 clock cycles
static inline void DLY_ms(uint32_t ms) {                // delay n milliseconds
  while(ms--) DLY_us(1000);
}

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================
void IWDG_start(uint16_t ms);                           // start IWDG with time in ms
void IWDG_reload(uint16_t ms);                          // reload IWDG with time in ms
#define IWDG_feed()       IWDG->KR = 0xAAAA             // feed the dog (reload time)

// ===================================================================================
// Low Power Timer (LPTIM) Functions
// ===================================================================================
void LPT_init(void);            // init and enable low-power timer (LPT)
void LPT_shot(uint16_t ms);     // start low-power timer single shot with period in ms
void LPT_start(uint16_t ms);    // start LPT in continuous mode with period in ms
void LPT_sleep(uint16_t ms);    // put device in to SLEEP,  wake-up after period in ms

#define LPT_enable()            LPTIM1->CR  |=  LPTIM_CR_ENABLE
#define LPT_disable()           LPTIM1->CR  &= ~LPTIM_CR_ENABLE
#define LPT_readReloadFlag()    (LPTIM1->ISR &  LPTIM_ISR_ARRM)
#define LPT_clearReloadFlag()   LPTIM1->ICR  =  LPTIM_ICR_ARRMCF
#define LPT_enableReloadInt()   LPTIM1->IER |=  LPTIM_IER_ARRMIE
#define LPT_disableReloadInt()  LPTIM1->IER &= ~LPTIM_IER_ARRMIE
#define LPT_enableWakeEvent()   EXTI->EMR   |=  ((uint32_t)1 << 29)
#define LPT_disableWakeEvent()  EXTI->EMR   &= ~((uint32_t)1 << 29)
#define LPT_enableWakeInt()     EXTI->IMR   |=  ((uint32_t)1 << 29)
#define LPT_disableWakeInt()    EXTI->IMR   &= ~((uint32_t)1 << 29)

// ===================================================================================
// Sleep Functions
// ===================================================================================
void SLEEP_WFI_now(void);       // put device into SLEEP,   wake up by interrupt
void SLEEP_WFE_now(void);       // put device into SLEEP,   wake up by event
void STOP_WFI_now(void);        // put device into STOP,    wake up interrupt
void STOP_WFE_now(void);        // put device into STOP,    wake up event
void STDBY_WFI_now(void);       // put device into STANDBY, wake up interrupt
void STDBY_WFE_now(void);       // put device into STANDBY, wake up event

// ===================================================================================
// Reset Functions
// ===================================================================================
#define RST_now()           NVIC_SystemReset()
#define RST_clearFlags()    RCC->CSR |= RCC_CSR_RMVF
#define RST_wasLowPower()   (RCC->CSR & RCC_CSR_LPWRRSTF)
#define RST_wasWWDG()       (RCC->CSR & RCC_CSR_WWDGRSTF)
#define RST_wasIWDG()       (RCC->CSR & RCC_CSR_IWDGRSTF)
#define RST_wasSoftware()   (RCC->CSR & RCC_CSR_SFTRSTF)
#define RST_wasPower()      (RCC->CSR & RCC_CSR_PORRSTF)
#define RST_wasPin()        (RCC->CSR & RCC_CSR_PINRSTF)
#define RST_wasOption()     (RCC->CSR & RCC_CSR_OBLRSTF)
#define RST_wasFirewall()   (RCC->CSR & RCC_CSR_FWRSTF)

// ===================================================================================
// Bootloader Functions
// ===================================================================================
#define BOOT_ADDR           0x1ff00000
void BOOT_now(void);        // jump to bootloader

// ===================================================================================
// Cyclic Redundancy Check (CRC) Functions
// ===================================================================================
#define CRC_enable()        RCC->AHBENR |=  RCC_AHBENR_CRCEN
#define CRC_disable         RCC->AHBENR &= ~RCC_AHBENR_CRCEN
#define CRC_write(w)        CRC->DR = (uint32_t)(w)   // add new word for calculation
#define CRC_read()          (CRC->DR)                 // read last calculation
#define CRC_reset()         CRC->CR |= 1              // reset calculation
#define CRC_setInit(n)      CRC->INIT = (n)           // set initial CRC value
#define CRC_setPoly(n)      CRC->POL = (n)            // set polynomial
#define CRC_data32          (*(__IO uint32_t*)CRC_BASE)
#define CRC_data16          (*(__IO uint16_t*)CRC_BASE)
#define CRC_data8           (*(__IO uint8_t*)CRC_BASE)
#define CRC_write32(w)      CRC_data32 = (uint32_t)(w)
#define CRC_write16(h)      CRC_data16 = (uint16_t)(h)
#define CRC_write8(b)       CRC_data8  = (uint8_t)(b)
#define CRC_read32()        CRC_data32
#define CRC_read16()        CRC_data16
#define CRC_read8()         CRC_data8

// ===================================================================================
// Device Electronic Signature (ESIG)
// ===================================================================================
#define ESIG_FLASHSIZE      (*(__I uint16_t*)(0x1FF8007C))
#define ESIG_UID1           (*(__I uint32_t*)(0x1FF80050))
#define ESIG_UID2           (*(__I uint32_t*)(0x1FF80054))
#define ESIG_UID3           (*(__I uint32_t*)(0x1FF80064))

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
  register uint32_t result;
  __ASM volatile ("MRS %0, psp\n"  : "=r" (result) );
  return(result);
}

// Set Process Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_PSP(uint32_t topOfProcStack) {
  __ASM volatile ("MSR psp, %0\n" : : "r" (topOfProcStack) : "sp");
}

// Get Main Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE uint32_t __get_MSP(void) {
  register uint32_t result;
  __ASM volatile ("MRS %0, msp\n" : "=r" (result) );
  return(result);
}

// Set Main Stack Pointer
__attribute__( ( always_inline ) ) __STATIC_INLINE void __set_MSP(uint32_t topOfMainStack) {
  __ASM volatile ("MSR msp, %0\n" : : "r" (topOfMainStack) : "sp");
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
  NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
}

// Disable External Interrupt
__STATIC_INLINE void NVIC_DisableIRQ(IRQn_Type IRQn) {
  NVIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  __DSB();
  __ISB();
}

// Get Pending Interrupt
__STATIC_INLINE uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) {
  return((uint32_t)(((NVIC->ISPR[0U] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
}

// Set Pending Interrupt
__STATIC_INLINE void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
  NVIC->ISPR[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
}

// Clear Pending Interrupt
__STATIC_INLINE void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
  NVIC->ICPR[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
}

// Set Interrupt Priority
__STATIC_INLINE void NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
  if ((int32_t)(IRQn) >= 0) {
    NVIC->IP[_IP_IDX(IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(IRQn)]  & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
      (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
  else {
    SCB->SHP[_SHP_IDX(IRQn)] = ((uint32_t)(SCB->SHP[_SHP_IDX(IRQn)] & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
      (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
}

// Get Interrupt Priority
__STATIC_INLINE uint32_t NVIC_GetPriority(IRQn_Type IRQn) {
  if ((int32_t)(IRQn) >= 0) {
    return((uint32_t)(((NVIC->IP[ _IP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
  }
  else {
    return((uint32_t)(((SCB->SHP[_SHP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
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
