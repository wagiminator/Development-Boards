// ===================================================================================
// Basic System Functions for STM32F030 and STM32F031                         * v0.1 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are 
// set up automatically on system start.
//
// Functions available:
// --------------------
// CLK_init_HSI()           init internal oscillator (non PLL) as system clock source
// CLK_init_HSI_PLL()       init internal oscillator with PLL as system clock source
// CLK_init_HSE()           init external crystal (non PLL) as system clock source
// CLK_init_HSE_PLL()       init external crystal with PLL as system clock source
// CLK_reset()              reset system clock to default state
//
// HSI_enable()             enable internal 16MHz high-speed clock (HSI)
// HSI_disable()            disable HSI
// HSI_ready()              check if HSI is stable
//
// HSE_enable()             enable external high-speed clock (HSE)
// HSE_disable()            disable HSE
// HSE_ready()              check if HSE is stable
// HSE_bypass_on()          enable HSE clock bypass
// HSE_bypass_off()         disable HSE clock bypass
// HSE_CSS_on()             enable HSE clock security system
// HSE_CSS_off()            disable HSE clock security system
//
// LSI_enable()             enable internal 40kHz low-speed clock (LSI)
// LSI_disable()            disable LSI
// LSI_ready()              check if LSI is stable
//
// LSE_enable()             enable external 32.768kHz low-speed clock (LSE)
// LSE_disable()            disable LSE
// LSE_ready()              check if LSE is stable
// LSE_bypass_on()          enable LSE clock bypass
// LSE_bypass_off()         disable LSE clock bypass
//
// PLL_enable()             enable phase-locked loop (PLL)
// PLL_disable()            disable PLL
// PLL_ready()              check if PLL is stable
// PLL_factor(n)            set PLL multiplication factor (PLL must be disabled)
// PLL_setHSI()             set HSI/2 as input for PLL (PLL must be disabled)
// PLL_setHSE()             set HSE as input for PLL (PLL must be disabled)
//
// MCO_setSYS()             enable output system clock (SYS_CLK) on MCO
// MCO_setHSI()             enable output internal high-speed clock (HSI) on MCO
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
// RTC_readAlarmAFlag()     check alarm A flag
// RTC_clearAlarmAFlag()    clear alarm A flag
// RTC_enableAlarmAInt()    enable alarm A interrupt
// RTC_disableAlarmAInt()   disable alarm A interrupt
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
#include "stm32f03x.h"

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
  #define F_CPU           8000000   // default clock frequency
#endif

// Calculate system clock settings
#if SYS_USE_HSE == 0
  #if   F_CPU == 48000000
    #define CLK_init      CLK_init_HSI_PLL  // 8/2MHz
    #define CLK_PLL_MUL   (0b1010<<18)      // *12
    #define CLK_DIV       (0b0000<<4)       // /1 = 48MHz
  #elif F_CPU == 32000000
    #define CLK_init      CLK_init_HSI_PLL  // 8/2MHz
    #define CLK_PLL_MUL   (0b0110<<18)      // *8
    #define CLK_DIV       (0b0000<<4)       // /1 = 32MHz
  #elif F_CPU == 24000000
    #define CLK_init      CLK_init_HSI_PLL  // 8/2MHz
    #define CLK_PLL_MUL   (0b0100<<18)      // *6
    #define CLK_DIV       (0b0000<<4)       // /1 = 24MHz
  #elif F_CPU == 20000000
    #define CLK_init      CLK_init_HSI_PLL  // 8/2MHz
    #define CLK_PLL_MUL   (0b0011<<18)      // *5
    #define CLK_DIV       (0b0000<<4)       // /1 = 20MHz
  #elif F_CPU == 16000000
    #define CLK_init      CLK_init_HSI_PLL  // 8/2MHz
    #define CLK_PLL_MUL   (0b0010<<18)      // *4
    #define CLK_DIV       (0b0000<<4)       // /1 = 16MHz
  #elif F_CPU == 12000000
    #define CLK_init      CLK_init_HSI_PLL  // 8/2MHz
    #define CLK_PLL_MUL   (0b0001<<18)      // *3
    #define CLK_DIV       (0b0000<<4)       // /1 = 12MHz
  #elif F_CPU ==  8000000
    #define CLK_init      CLK_init_HSI      // 8MHz
    #define CLK_DIV       (0b0000<<4)       // /1 = 8MHz
  #elif F_CPU ==  4000000
    #define CLK_init      CLK_init_HSI      // 8MHz
    #define CLK_DIV       (0b1000<<4)       // /2 = 4MHz
  #elif F_CPU ==  2000000
    #define CLK_init      CLK_init_HSI      // 8MHz
    #define CLK_DIV       (0b1001<<4)       // /4 = 2MHz
  #elif F_CPU ==  1000000
    #define CLK_init      CLK_init_HSI      // 8MHz
    #define CLK_DIV       (0b1010<<4)       // /8 = 1MHz
  #else
    #warning Unsupported system clock frequency, using internal 8000000 Hz
    #define CLK_init      CLK_init_HSI      // 8MHz
    #define CLK_DIV       (0b0000<<4)       // /1 = 8MHz
    #undef  F_CPU
    #define F_CPU         8000000  
  #endif

#else
  #define CLK_init        CLK_init_HSE
  #define CLK_DIV         (0b0000<<4)     // /1
#endif

#ifndef CLK_PLL_MUL
  #define CLK_PLL_MUL     0
#endif

// ===================================================================================
// System Clock Functions
// ===================================================================================

// System clock init functions (called automatically on system start)
void CLK_init_HSI(void);      // init internal oscillator (non PLL) as system clock source
void CLK_init_HSI_PLL(void);  // init internal oscillator with PLL as system clock source
void CLK_init_HSE(void);      // init external crystal (non PLL) as system clock source
void CLK_init_HSE_PLL(void);  // init external crystal with PLL as system clock source
void CLK_reset(void);         // reset system clock to default state

// Internal high-speed clock (HSI) functions
#define HSI_enable()      RCC->CR |=  RCC_CR_HSION        // enable HSI
#define HSI_disable()     RCC->CR &= ~RCC_CR_HSION        // disable HSI
#define HSI_ready()       (RCC->CR & RCC_CR_HSIRDY)       // check if HSI is stable

// External high-speed clock (HSE) functions
#define HSE_enable()      RCC->CR |=  RCC_CR_HSEON        // enable HSE
#define HSE_disable()     RCC->CR &= ~RCC_CR_HSEON        // disable HSE
#define HSE_ready()       (RCC->CR & RCC_CR_HSERDY)       // check if HSE is stable
#define HSE_bypass_on()   RCC->CR |=  RCC_CR_HSEBYP       // enable HSE clock bypass
#define HSE_bypass_off()  RCC->CR &= ~RCC_CR_HSEBYP       // disable HSE clock bypass
#define HSE_CSS_on()      RCC->CR |=  RCC_CR_CSSON        // enable HSE clock security
#define HSE_CSS_off()     RCC->CR &= ~RCC_CR_CSSON        // enable HSE clock security

// Internal 40kHz low-speed clock (LSI) functions
#define LSI_enable()      RCC->CSR |=  RCC_CSR_LSION      // enable LSI
#define LSI_disable()     RCC->CSR &= ~RCC_CSR_LSION      // disable LSI
#define LSI_ready()       (RCC->CSR & RCC_CSR_LSIRDY)     // check if LSI is stable

// External 32.768kHz low-speed clock (LSE) functions
#define LSE_enable()      RCC->BDCR |=  RCC_BDCR_LSEON    // enable LSE
#define LSE_disable()     RCC->BDCR &= ~RCC_BDCR_LSEON    // disable LSE
#define LSE_ready()       (RCC->BDCR & RCC_BDCR_LSERDY)   // check if LSE is stable
#define LSE_bypass_on()   RCC->BDCR |=  RCC_BDCR_LSEBYP   // enable LSE clock bypass
#define LSE_bypass_off()  RCC->BDCR &= ~RCC_BDCR_LSEBYP   // disable LSE clock bypass

// PLL functions
#define PLL_enable()      RCC->CR |=  RCC_CR_PLLON        // enable PLL
#define PLL_disable()     RCC->CR &= ~RCC_CR_PLLON        // disable PLL
#define PLL_ready()       (RCC->CR & RCC_CR_PLLRDY)       // check if PLL is stable
#define PLL_factor(n)     (RCC->CFGR & RCC_CFGR_PLLMUL) | (((n-2)&15) << 18)
#define PLL_setHSI()      RCC->CFGR &= ~RCC_CFGR_PLLSRC   // set HSI/2 as PLL input
#define PLL_setHSE()      RCC->CFGR |=  RCC_CFGR_PLLSRC   // set HSE as PLL input

// Clock output functions (MCO)
#define MCO_setSYS()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCO) | (0b0100 << 24)
#define MCO_setHSI()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCO) | (0b0101 << 24)
#define MCO_setHSE()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCO) | (0b0110 << 24)
#define MCO_setPLL()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCO) | (0b0111 << 24)
#define MCO_setLSI()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCO) | (0b0010 << 24)
#define MCO_setLSE()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCO) | (0b0011 << 24)
#define MCO_setPRE(n)     RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOPRE) | (((n)&7) << 28)
#define MCO_stop()        RCC->CFGR &= ~RCC_CFGR_MCO

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
void RTC_startWakeupTimer(uint16_t ms);

#define RTC_isInit()            (RTC->ISR & RTC_ISR_INITS)
#define RTC_stopWakeupTimer()   RTC->CR &= ~RTC_CR_WUTE
#define RTC_readWakeupFlag()    (RTC->ISR & RTC_ISR_WUTF)
#define RTC_readAlarmAFlag()    (RTC->ISR & RTC_ISR_ALRAF)
#define RTC_clearWakeupFlag()   RTC->ISR &= ~RTC_ISR_WUTF
#define RTC_clearAlarmAFlag()   RTC->ISR &= ~RTC_ISR_ALRAF
#define RTC_enableWakeupInt()   RTC->CR |=  RTC_CR_WUTIE
#define RTC_disableWakeupInt()  RTC->CR &= ~RTC_CR_WUTIE
#define RTC_enableAlarmAInt()   RTC->CR |=  RTC_CR_ALRAIE
#define RTC_disableAlarmAInt()  RTC->CR &= ~RTC_CR_ALRAIE
#define RTC_enableWakeEvent()   {EXTI->EMR |= EXTI_EMR_MR17;EXTI->RTSR |= EXTI_RTSR_TR17;}
#define RTC_disableWakeEvent()  EXTI->EMR &= ~EXTI_EMR_MR17
#define RTC_enableWakeInt()     {EXTI->IMR |= EXTI_IMR_MR17;EXTI->RTSR |= EXTI_RTSR_TR17;}
#define RTC_disableWakeInt()    EXTI->IMR &= ~EXTI_IMR_MR17

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
#define BOOT_ADDR           0x1FFFEC00
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
#define ESIG_FLASHSIZE      (*(__I uint16_t*)(0x1FFFF7CC))
#define ESIG_UID1           (*(__I uint32_t*)(0x1FFFF7AC))
#define ESIG_UID2           (*(__I uint32_t*)(0x1FFFF7B0))
#define ESIG_UID3           (*(__I uint32_t*)(0x1FFFF7B4))

// ===================================================================================
// Imported System Functions from cmsis_gcc.h and core_cm0.h
// ===================================================================================
#define NVIC_SetPriorityGrouping    __NVIC_SetPriorityGrouping
#define NVIC_GetPriorityGrouping    __NVIC_GetPriorityGrouping
#define NVIC_EnableIRQ              __NVIC_EnableIRQ
#define NVIC_GetEnableIRQ           __NVIC_GetEnableIRQ
#define NVIC_DisableIRQ             __NVIC_DisableIRQ
#define NVIC_GetPendingIRQ          __NVIC_GetPendingIRQ
#define NVIC_SetPendingIRQ          __NVIC_SetPendingIRQ
#define NVIC_ClearPendingIRQ        __NVIC_ClearPendingIRQ
#define NVIC_SetPriority            __NVIC_SetPriority
#define NVIC_GetPriority            __NVIC_GetPriority
#define NVIC_SystemReset            __NVIC_SystemReset
#define NVIC_SetVector              __NVIC_SetVector
#define NVIC_GetVector              __NVIC_GetVector

#define NVIC_USER_IRQ_OFFSET          16

#define EXC_RETURN_HANDLER         (0xFFFFFFF1UL) 
#define EXC_RETURN_THREAD_MSP      (0xFFFFFFF9UL) 
#define EXC_RETURN_THREAD_PSP      (0xFFFFFFFDUL)

#define _BIT_SHIFT(IRQn)         (  ((((uint32_t)(int32_t)(IRQn))         )      &  0x03UL) * 8UL)
#define _SHP_IDX(IRQn)           ( (((((uint32_t)(int32_t)(IRQn)) & 0x0FUL)-8UL) >>    2UL)      )
#define _IP_IDX(IRQn)            (   (((uint32_t)(int32_t)(IRQn))                >>    2UL)      )

#define __NVIC_SetPriorityGrouping(X) (void)(X)
#define __NVIC_GetPriorityGrouping()  (0U)

// Ignore some GCC warnings
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wsign-conversion"
#pragma GCC diagnostic ignored "-Wconversion"
#pragma GCC diagnostic ignored "-Wunused-parameter"

// Enable IRQ Interrupts
__STATIC_FORCEINLINE void __enable_irq(void) {
  __ASM volatile ("cpsie i" : : : "memory");
}

// Disable IRQ Interrupts
__STATIC_FORCEINLINE void __disable_irq(void) {
  __ASM volatile ("cpsid i" : : : "memory");
}

// Get Control Register
__STATIC_FORCEINLINE uint32_t __get_CONTROL(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, control" : "=r" (result) );
  return(result);
}

// Set Control Register
__STATIC_FORCEINLINE void __set_CONTROL(uint32_t control) {
  __ASM volatile ("MSR control, %0" : : "r" (control) : "memory");
}

// Get IPSR Register
__STATIC_FORCEINLINE uint32_t __get_IPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, ipsr" : "=r" (result) );
  return(result);
}

// Get APSR Register
__STATIC_FORCEINLINE uint32_t __get_APSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, apsr" : "=r" (result) );
  return(result);
}

// Get xPSR Register
__STATIC_FORCEINLINE uint32_t __get_xPSR(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, xpsr" : "=r" (result) );
  return(result);
}

// Get Process Stack Pointer
__STATIC_FORCEINLINE uint32_t __get_PSP(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, psp"  : "=r" (result) );
  return(result);
}

// Set Process Stack Pointer
__STATIC_FORCEINLINE void __set_PSP(uint32_t topOfProcStack) {
  __ASM volatile ("MSR psp, %0" : : "r" (topOfProcStack) : );
}

// Get Main Stack Pointer
__STATIC_FORCEINLINE uint32_t __get_MSP(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, msp" : "=r" (result) );
  return(result);
}

// Set Main Stack Pointer
__STATIC_FORCEINLINE void __set_MSP(uint32_t topOfMainStack) {
  __ASM volatile ("MSR msp, %0" : : "r" (topOfMainStack) : );
}

// Get Priority Mask
__STATIC_FORCEINLINE uint32_t __get_PRIMASK(void) {
  uint32_t result;
  __ASM volatile ("MRS %0, primask" : "=r" (result) :: "memory");
  return(result);
}

// Set Priority Mask
__STATIC_FORCEINLINE void __set_PRIMASK(uint32_t priMask) {
  __ASM volatile ("MSR primask, %0" : : "r" (priMask) : "memory");
}

// Get FPSCR
__STATIC_FORCEINLINE uint32_t __get_FPSCR(void) {
  return(0U);
}

// Set FPSCR
__STATIC_FORCEINLINE void __set_FPSCR(uint32_t fpscr) {
  (void)fpscr;
}

#if defined (__thumb__) && !defined (__thumb2__)
#define __CMSIS_GCC_OUT_REG(r) "=l" (r)
#define __CMSIS_GCC_RW_REG(r) "+l" (r)
#define __CMSIS_GCC_USE_REG(r) "l" (r)
#else
#define __CMSIS_GCC_OUT_REG(r) "=r" (r)
#define __CMSIS_GCC_RW_REG(r) "+r" (r)
#define __CMSIS_GCC_USE_REG(r) "r" (r)
#endif

// No Operation
#define __NOP()                             __ASM volatile ("nop")

// Wait For Interrupt
#define __WFI()                             __ASM volatile ("wfi")

// Wait For Event
#define __WFE()                             __ASM volatile ("wfe")

// Send Event
#define __SEV()                             __ASM volatile ("sev")

// Instruction Synchronization Barrier
__STATIC_FORCEINLINE void __ISB(void) {
  __ASM volatile ("isb 0xF":::"memory");
}

// Data Synchronization Barrier
__STATIC_FORCEINLINE void __DSB(void) {
  __ASM volatile ("dsb 0xF":::"memory");
}

// Data Memory Barrier
__STATIC_FORCEINLINE void __DMB(void) {
  __ASM volatile ("dmb 0xF":::"memory");
}

// Reverse byte order (32 bit)
__STATIC_FORCEINLINE uint32_t __REV(uint32_t value) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
  return __builtin_bswap32(value);
#else
  uint32_t result;
  __ASM volatile ("rev %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return result;
#endif
}

// Reverse byte order (16 bit)
__STATIC_FORCEINLINE uint32_t __REV16(uint32_t value) {
  uint32_t result;
  __ASM volatile ("rev16 %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return result;
}

// Reverse byte order (16 bit)
__STATIC_FORCEINLINE int16_t __REVSH(int16_t value) {
#if (__GNUC__ > 4) || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)
  return (int16_t)__builtin_bswap16(value);
#else
  int16_t result;
  __ASM volatile ("revsh %0, %1" : __CMSIS_GCC_OUT_REG (result) : __CMSIS_GCC_USE_REG (value) );
  return result;
#endif
}

// Rotate Right in unsigned value (32 bit)
__STATIC_FORCEINLINE uint32_t __ROR(uint32_t op1, uint32_t op2) {
  op2 %= 32U;
  if (op2 == 0U) return op1;
  return (op1 >> op2) | (op1 << (32U - op2));
}

// Breakpoint
#define __BKPT(value)                       __ASM volatile ("bkpt "#value)

// Reverse bit order of value
__STATIC_FORCEINLINE uint32_t __RBIT(uint32_t value) {
  uint32_t result;
  return result;
}

// Count leading zeros
#define __CLZ             (uint8_t)__builtin_clz

#pragma GCC diagnostic pop

// Enable Interrupt
__STATIC_INLINE void __NVIC_EnableIRQ(IRQn_Type IRQn) {
  if((int32_t)(IRQn) >= 0) NVIC->ISER[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
}

// Get Interrupt Enable status
__STATIC_INLINE uint32_t __NVIC_GetEnableIRQ(IRQn_Type IRQn) {
  return((uint32_t)(((NVIC->ISER[0U] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
}

//Disable Interrupt
__STATIC_INLINE void __NVIC_DisableIRQ(IRQn_Type IRQn) {
  NVIC->ICER[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
  __DSB();
  __ISB();
}

// Get Pending Interrupt
__STATIC_INLINE uint32_t __NVIC_GetPendingIRQ(IRQn_Type IRQn) {
  return((uint32_t)(((NVIC->ISPR[0U] & (1UL << (((uint32_t)IRQn) & 0x1FUL))) != 0UL) ? 1UL : 0UL));
}

// Set Pending Interrupt
__STATIC_INLINE void __NVIC_SetPendingIRQ(IRQn_Type IRQn) {
  NVIC->ISPR[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
}

// Clear Pending Interrupt
__STATIC_INLINE void __NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
  NVIC->ICPR[0U] = (uint32_t)(1UL << (((uint32_t)IRQn) & 0x1FUL));
}

// Set Interrupt Priority
__STATIC_INLINE void __NVIC_SetPriority(IRQn_Type IRQn, uint32_t priority) {
  if((int32_t)(IRQn) >= 0) {
    NVIC->IP[_IP_IDX(IRQn)]  = ((uint32_t)(NVIC->IP[_IP_IDX(IRQn)]  & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
  else {
    SCB->SHP[_SHP_IDX(IRQn)] = ((uint32_t)(SCB->SHP[_SHP_IDX(IRQn)] & ~(0xFFUL << _BIT_SHIFT(IRQn))) |
       (((priority << (8U - __NVIC_PRIO_BITS)) & (uint32_t)0xFFUL) << _BIT_SHIFT(IRQn)));
  }
}

// Get Interrupt Priority
__STATIC_INLINE uint32_t __NVIC_GetPriority(IRQn_Type IRQn) {
  if((int32_t)(IRQn) >= 0) {
    return((uint32_t)(((NVIC->IP[ _IP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
  }
  else {
    return((uint32_t)(((SCB->SHP[_SHP_IDX(IRQn)] >> _BIT_SHIFT(IRQn) ) & (uint32_t)0xFFUL) >> (8U - __NVIC_PRIO_BITS)));
  }
}

// Encode Priority
__STATIC_INLINE uint32_t NVIC_EncodePriority(uint32_t PriorityGroup, uint32_t PreemptPriority, uint32_t SubPriority) {
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(__NVIC_PRIO_BITS)) ? (uint32_t)(__NVIC_PRIO_BITS) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(__NVIC_PRIO_BITS)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(__NVIC_PRIO_BITS));

  return (
           ((PreemptPriority & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL)) << SubPriorityBits) |
           ((SubPriority     & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL)))
         );
}

// Decode Priority
__STATIC_INLINE void NVIC_DecodePriority (uint32_t Priority, uint32_t PriorityGroup, uint32_t* const pPreemptPriority, uint32_t* const pSubPriority) {
  uint32_t PriorityGroupTmp = (PriorityGroup & (uint32_t)0x07UL);
  uint32_t PreemptPriorityBits;
  uint32_t SubPriorityBits;

  PreemptPriorityBits = ((7UL - PriorityGroupTmp) > (uint32_t)(__NVIC_PRIO_BITS)) ? (uint32_t)(__NVIC_PRIO_BITS) : (uint32_t)(7UL - PriorityGroupTmp);
  SubPriorityBits     = ((PriorityGroupTmp + (uint32_t)(__NVIC_PRIO_BITS)) < (uint32_t)7UL) ? (uint32_t)0UL : (uint32_t)((PriorityGroupTmp - 7UL) + (uint32_t)(__NVIC_PRIO_BITS));

  *pPreemptPriority = (Priority >> SubPriorityBits) & (uint32_t)((1UL << (PreemptPriorityBits)) - 1UL);
  *pSubPriority     = (Priority                   ) & (uint32_t)((1UL << (SubPriorityBits    )) - 1UL);
}

// Set Interrupt Vector
__STATIC_INLINE void __NVIC_SetVector(IRQn_Type IRQn, uint32_t vector) {
  uint32_t *vectors = (uint32_t *)0x0U;
  vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET] = vector;
}

// Get Interrupt Vector
__STATIC_INLINE uint32_t __NVIC_GetVector(IRQn_Type IRQn) {
  uint32_t *vectors = (uint32_t *)0x0U;
  return vectors[(int32_t)IRQn + NVIC_USER_IRQ_OFFSET];
}

// System Reset
__NO_RETURN __STATIC_INLINE void __NVIC_SystemReset(void) {
  __DSB();
  SCB->AIRCR  = ((0x5FAUL << SCB_AIRCR_VECTKEY_Pos) | SCB_AIRCR_SYSRESETREQ_Msk);
  __DSB(); 
  for(;;) __NOP();
}

#if defined (__Vendor_SysTickConfig) && (__Vendor_SysTickConfig == 0U)
// System Tick Configuration
__STATIC_INLINE uint32_t SysTick_Config(uint32_t ticks) {
  if ((ticks - 1UL) > SysTick_LOAD_RELOAD_Msk) return (1UL);
  SysTick->LOAD  = (uint32_t)(ticks - 1UL);
  NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL);
  SysTick->VAL   = 0UL;
  SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  return (0UL);
}
#endif

#ifdef __cplusplus
};
#endif
