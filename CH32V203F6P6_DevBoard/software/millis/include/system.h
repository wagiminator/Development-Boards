// ===================================================================================
// Basic System Functions for CH32V203                                        * v1.1 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are 
// set up automatically on system start.
//
// System clock functions available:
// ---------------------------------
// CLK_init_HSI()           init internal oscillator (non PLL) as system clock source
// CLK_init_HSI_PLL()       init internal oscillator with PLL as system clock source
// CLK_init_HSE()           init external crystal (non PLL) as system clock source
// CLK_init_HSE_PLL()       init external crystal (PLL) as system clock source
// CLK_reset()              reset system clock to default state
//
// HSI_enable()             enable internal 8MHz high-speed clock (HSI)
// HSI_disable()            disable HSI
// HSI_ready()              check if HSI is stable
//
// HSE_enable()             enable external high-speed clock (HSE)
// HSE_disable()            disable HSE
// HSE_ready()              check if HSE is stable
// HSE_bypass_on()          enable HSE clock bypass
// HSE_bypass_off()         disable HSE clock bypass
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
// MCO_init()               init clock output to pin PA8
// MCO_setSYS()             output SYS_CLK on pin PA8
// MCO_setHSI()             output internal oscillator on pin PA8
// MCO_setHSE()             output external crystal on pin PA8 (if available)
// MCO_setPLL()             output PLL on pin PA8
//
// Real-Time Clock (RTC) functions available:
// ------------------------------------------
// RTC_init_LSI()           init RTC with internal low-speed clock
// RTC_init_LSE()           init RTC with external low-speed clock
// RTC_set(n)               set RTC counter
// RTC_get()                get RTC counter
// RTC_ALARM_set(n)         set RTC ALARM value
// RTC_ALARM_get()          get RTC ALARM value
//
// Delay (DLY) functions available:
// --------------------------------
// DLY_ticks(n)             delay n system clock cycles
// DLY_us(n)                delay n microseconds
// DLY_ms(n)                delay n milliseconds
//
// Reset (RST) functions available:
// --------------------------------
// RST_now()                conduct software reset
// RST_clearFlags()         clear all reset flags
// RST_wasLowPower()        check if last reset was caused by low power
// RST_wasWWDG()            check if last reset was caused by window watchdog
// RST_wasIWDG()            check if last reset was caused by independent watchdog
// RST_wasSoftware()        check if last reset was caused by software
// RST_wasPower()           check if last reset was caused by power up
// RST_wasPin()             check if last reset was caused by RST pin low
//
// Independent Watchdog Timer (IWDG) functions available:
// ------------------------------------------------------
// IWDG_start(n)            start independent watchdog timer, n cycles, n<=4095
// IWDG_start_ms(n)         start independent watchdog timer, n milliseconds, n<=6552
// IWDG_reload(n)           reload watchdog counter with n cycles, n<=4095
// IWDG_reload_ms(n)        reload watchdog counter with n milliseconds, n<=6552
// IWDG_feed()              feed the dog (reload last time)
//
// Sleep functions available:
// --------------------------
// SLEEP_WFI_now()          put device into sleep, wake up by interrupt
// SLEEP_WFE_now()          put device into sleep, wake up by event
// STOP_WFI_now()           put device into stop, wake up by interrupt
// STOP_WFE_now()           put device into stop, wake up by event
// STDBY_WFI_now()          put device into standby (deep sleep), wake by interrupt
// STDBY_WFE_now()          put device into standby (deep sleep), wake by event
//
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v203.h"

// ===================================================================================
// System Options (set "1" to activate)
// ===================================================================================
#define SYS_CLK_INIT      1         // 1: init system clock on startup
#define SYS_TICK_INIT     1         // 1: init and start SYSTICK on startup
#define SYS_GPIO_EN       1         // 1: enable GPIO ports on startup
#define SYS_CLEAR_BSS     1         // 1: clear uninitialized variables
#define SYS_USE_VECTORS   1         // 1: create interrupt vector table
#define SYS_USE_HSE       0         // 1: use external crystal

// ===================================================================================
// Sytem Clock Defines
// ===================================================================================
// Set system clock frequency
#ifndef F_CPU
  #define F_CPU           48000000  // 48Mhz if not otherwise defined
#endif

// Calculate system clock settings
#if   F_CPU ==   8000000
  #define CLK_NO_PLL
#elif F_CPU ==  16000000
  #define CLK_MUL         RCC_PLLMULL2
#elif F_CPU ==  24000000
  #define CLK_MUL         RCC_PLLMULL3
#elif F_CPU ==  32000000
  #define CLK_MUL         RCC_PLLMULL4
#elif F_CPU ==  40000000
  #define CLK_MUL         RCC_PLLMULL5
#elif F_CPU ==  48000000
  #define CLK_MUL         RCC_PLLMULL6
#elif F_CPU ==  56000000
  #define CLK_MUL         RCC_PLLMULL7
#elif F_CPU ==  64000000
  #define CLK_MUL         RCC_PLLMULL8
#elif F_CPU ==  72000000
  #define CLK_MUL         RCC_PLLMULL9
#elif F_CPU ==  80000000
  #define CLK_MUL         RCC_PLLMULL10
#elif F_CPU ==  88000000
  #define CLK_MUL         RCC_PLLMULL11
#elif F_CPU ==  96000000
  #define CLK_MUL         RCC_PLLMULL12
#elif F_CPU == 104000000
  #define CLK_MUL         RCC_PLLMULL13
#elif F_CPU == 112000000
  #define CLK_MUL         RCC_PLLMULL14
#elif F_CPU == 120000000
  #define CLK_MUL         RCC_PLLMULL15
#elif F_CPU == 128000000
  #define CLK_MUL         RCC_PLLMULL16
#elif F_CPU == 136000000
  #define CLK_MUL         RCC_PLLMULL17
#elif F_CPU == 144000000
  #define CLK_MUL         RCC_PLLMULL18
#else
  #warning Unsupported system clock frequency, using internal 48MHz
  #define CLK_MUL         RCC_PLLMULL6
  #undef  F_CPU
  #define F_CPU           48000000
#endif

#if SYS_USE_HSE > 0
  #if F_CPU != 8000000
    #define CLK_init      CLK_init_HSE_PLL
  #else
    #define CLK_init      CLK_init_HSE
  #endif
#else
  #if F_CPU != 8000000
    #define CLK_init      CLK_init_HSI_PLL
  #else
    #define CLK_init      CLK_init_HSI
  #endif
#endif

// ===================================================================================
// System Clock Functions
// ===================================================================================

// System clock init functions (called automatically on system start)
void CLK_init_HSI(void);      // init internal oscillator (non PLL) as system clock source
void CLK_init_HSI_PLL(void);  // init internal oscillator with PLL as system clock source
void CLK_init_HSE(void);      // init external crystal (non PLL) as system clock source
void CLK_init_HSE_PLL(void);  // init external crystal (PLL) as system clock source
void CLK_reset(void);         // reset system clock to default state

// Internal 8MHz high-speed clock (HSI) functions
#define HSI_enable()      RCC->CTLR |= RCC_HSION        // enable HSI
#define HSI_disable()     RCC->CTLR &= ~RCC_HSION       // disable HSI
#define HSI_ready()       (RCC->CTLR & RCC_HSIRDY)      // check if HSI is stable

// External high-speed clock (HSE) functions
#define HSE_enable()      RCC->CTLR |= RCC_HSEON        // enable HSE
#define HSE_disable()     RCC->CTLR &= ~RCC_HSEON       // disable HSE
#define HSE_ready()       (RCC->CTLR & RCC_HSERDY)      // check if HSE is stable
#define HSE_bypass_on()   RCC->CTLR |= RCC_HSEBYP       // enable HSE clock bypass
#define HSE_bypass_off()  RCC->CTLR &= ~RCC_HSEBYP      // disable HSE clock bypass

// Internal 40kHz low-speed clock (LSI) functions
#define LSI_enable()      RCC->RSTSCKR |= RCC_LSION     // enable LSI
#define LSI_disable()     RCC->RSTSCKR &= ~RCC_LSION    // disable LSI
#define LSI_ready()       (RCC->RSTSCKR & RCC_LSIRDY)   // check if LSI is stable

// External 32.768kHz low-speed clock (LSE) functions
#define LSE_enable()      RCC->BDCTLR |= RCC_LSEON      // enable LSE
#define LSE_disable()     RCC->BDCTLR &= ~RCC_LSEON     // disable LSE
#define LSE_ready()       (RCC->BDCTLR & RCC_LSERDY)    // check if LSE is stable
#define LSE_bypass_on()   RCC->BDCTLR |= RCC_LSEBYP     // enable LSE clock bypass
#define LSE_bypass_off()  RCC->BDCTLR &= ~RCC_LSEBYP    // disable LSE clock bypass

// Clock output functions (pin PA8)
#define MCO_setSYS()      RCC->CFGR0 = (RCC->CFGR0 & ~RCC_CFGR0_MCO) | RCC_CFGR0_MCO_SYSCLK
#define MCO_setHSI()      RCC->CFGR0 = (RCC->CFGR0 & ~RCC_CFGR0_MCO) | RCC_CFGR0_MCO_HSI
#define MCO_setHSE()      RCC->CFGR0 = (RCC->CFGR0 & ~RCC_CFGR0_MCO) | RCC_CFGR0_MCO_HSE
#define MCO_setPLL()      RCC->CFGR0 = (RCC->CFGR0 & ~RCC_CFGR0_MCO) | RCC_CFGR0_MCO_PLL
#define MCO_stop()        RCC->CFGR0 &= ~RCC_CFGR0_MCO  // stop clock output to pin PA8
void MCO_init(void);                                    // init clock output to pin PA8

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================
void RTC_init_LSI(void);          // init RTC with internal low-speed clock
void RTC_init_LSE(void);          // init RTC with external low-speed clock
void RTC_set(uint32_t n);         // set RTC counter
void RTC_ALARM_set(uint32_t n);   // set RTC ALARM
#define RTC_get()         (((uint32_t)RTC->CNTH << 16) & RTC->CNTL)   // get RTC counter
#define RTC_ALARM_get()   (((uint32_t)RTC->ALRMH << 16) & RTC->ALRML) // get RTC ALARM

// ===================================================================================
// Delay (DLY) Functions
// ===================================================================================
#define STK_init()        STK->CTLR = STK_CTLR_STE | STK_CTLR_STCLK // init SYSTICK @ F_CPU
#define DLY_US_TIME       (F_CPU / 1000000)             // system ticks per us
#define DLY_MS_TIME       (F_CPU / 1000)                // system ticks per ms
#define DLY_us(n)         DLY_ticks((n) * DLY_US_TIME)  // delay n microseconds
#define DLY_ms(n)         DLY_ticks((n) * DLY_MS_TIME)  // delay n milliseconds
#define DLY_cycles        DLY_ticks                     // alias
void DLY_ticks(uint32_t n);                             // delay n system ticks

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
#define RST_now()         PFIC->CFGR    = PFIC_RESETSYS | PFIC_KEY3
#define RST_clearFlags()  RCC->RSTSCKR |= RCC_RMVF
#define RST_wasLowPower() (RCC->RSTSCKR & RCC_LPWRRSTF)
#define RST_wasWWDG()     (RCC->RSTSCKR & RCC_WWDGRSTF)
#define RST_wasIWDG()     (RCC->RSTSCKR & RCC_IWDGRSTF)
#define RST_wasSoftware() (RCC->RSTSCKR & RCC_SFTRSTF)
#define RST_wasPower()    (RCC->RSTSCKR & RCC_PORRSTF)
#define RST_wasPin()      (RCC->RSTSCKR & RCC_PINRSTF)

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================
void IWDG_start(uint16_t ticks);                           // start IWDG with n ticks
void IWDG_reload(uint16_t ticks);                          // reload IWDG with n ticks
#define IWDG_start_ms(n)  IWDG_start((n)*LSI_VALUE/64000)  // start IWDG timer with n ms
#define IWDG_reload_ms(n) IWDG_reload((n)*LSI_VALUE/64000) // reload IWDG timer with n ms
#define IWDG_feed()       IWDG->CTLR = 0xAAAA              // feed the dog (reload time)

// ===================================================================================
// Sleep Functions
// ===================================================================================
void SLEEP_WFI_now(void);   // put device into sleep, wake up by interrupt
void SLEEP_WFE_now(void);   // put device into sleep, wake up by event
void STOP_WFI_now(void);    // put device into stop, wake up by interrupt
void STOP_WFE_now(void);    // put device into stop, wake up by event
void STDBY_WFI_now(void);   // put device into standby (deep sleep), wake up interrupt
void STDBY_WFE_now(void);   // put device into standby (deep sleep), wake up event

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
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, 0x800       \n"
    "csrw 0x800, %1       \n"
    : "=&r" (result) : "r" (0x6000)
  );
  return result;
}

// Restore interrupt status
static inline void __iRestore(const uint32_t *__s) {
  __asm volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw 0x800, %0" : : "r" (*__s)
  );
}

// ===================================================================================
// Imported System Functions
// ===================================================================================
// Enable Global Interrupt
static inline void __enable_irq(void) {
  __asm volatile (
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw 0x800, %0" : : "r" (0x6088)
  );
}

// Disable Global Interrupt
static inline void __disable_irq(void) {
  __asm volatile (
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw 0x800, %0" : : "r" (0x6000)
  );
}

// No OPeration
static inline void __NOP(void) {
  __asm volatile ("nop");
}

// Enable NVIC interrupt (interrupt numbers)
static inline void NVIC_EnableIRQ(IRQn_Type IRQn) {
  NVIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Disable NVIC interrupt (interrupt numbers)
static inline void NVIC_DisableIRQ(IRQn_Type IRQn) {
  NVIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Get Interrupt Enable State
static inline uint32_t NVIC_GetStatusIRQ(IRQn_Type IRQn) {
  return((uint32_t) ((NVIC->ISR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Get Interrupt Pending State
static inline uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) {
  return((uint32_t) ((NVIC->IPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Set Interrupt Pending
static inline void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
  NVIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Clear Interrupt Pending
static inline void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
  NVIC->IPRR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Get Interrupt Active State
static inline uint32_t NVIC_GetActive(IRQn_Type IRQn) {
  return((uint32_t)((NVIC->IACTR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Set Interrupt Priority
static inline void NVIC_SetPriority(IRQn_Type IRQn, uint8_t priority) {
  NVIC->IPRIOR[(uint32_t)(IRQn)] = priority;
}

// Wait for Interrupt
__attribute__( ( always_inline ) ) static inline void __WFI(void) {
  NVIC->SCTLR &= ~(1<<3);   // wfi
  asm volatile ("wfi");
}

// Wait for Events
__attribute__( ( always_inline ) ) static inline void __WFE(void) {
  uint32_t t;
  t = NVIC->SCTLR;
  NVIC->SCTLR |= (1<<3)|(1<<5);     // (wfi->wfe)+(__sev)
  NVIC->SCTLR = (NVIC->SCTLR & ~(1<<5)) | ( t & (1<<5));
  asm volatile ("wfi");
  asm volatile ("wfi");
}

// Set VTF Interrupt
static inline void SetVTFIRQ(uint32_t addr, IRQn_Type IRQn, uint8_t num, FunctionalState NewState) {
  if(num > 1)  return;
  if(NewState != DISABLE) {
    NVIC->VTFIDR[num] = IRQn;
    NVIC->VTFADDR[num] = ((addr&0xFFFFFFFE)|0x1);
  }
  else {
    NVIC->VTFIDR[num] = IRQn;
    NVIC->VTFADDR[num] = ((addr&0xFFFFFFFE)&(~0x1));
  }
}

// Initiate a system reset request
static inline void NVIC_SystemReset(void) {
  NVIC->CFGR = NVIC_KEY3|(1<<7);
}

// Return the Machine Status Register
static inline uint32_t __get_MSTATUS(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mstatus" : "=r" (result)
  );
  return result;
}

// Set the Machine Status Register
static inline void __set_MSTATUS(uint32_t value) {
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw mstatus, %0" : : "r" (value)
  );
}

// Return the Machine ISA Register
static inline uint32_t __get_MISA(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, misa" : "=r" (result)
  );
  return result;
}

// Set the Machine ISA Register
static inline void __set_MISA(uint32_t value) {
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw misa, %0" : : "r" (value)
  );
}

// Return the Machine Trap-Vector Base-Address Register
static inline uint32_t __get_MTVEC(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mtvec" : "=r" (result)
  );
  return result;
}

// Set the Machine Trap-Vector Base-Address Register
static inline void __set_MTVEC(uint32_t value) {
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw mtvec, %0" : : "r" (value)
  );
}

// Return the Machine Seratch Register
static inline uint32_t __get_MSCRATCH(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mscratch" : "=r" (result)
  );
  return result;
}

// Set the Machine Seratch Register
static inline void __set_MSCRATCH(uint32_t value) {
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw mscratch, %0" : : "r" (value)
  );
}

// Return the Machine Exception Program Register
static inline uint32_t __get_MEPC(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mepc" : "=r" (result)
  );
  return result;
}

// Set the Machine Exception Program Register
static inline void __set_MEPC(uint32_t value) {
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw mepc, %0" : : "r" (value)
  );
}

// Return the Machine Cause Register
static inline uint32_t __get_MCAUSE(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mcause" : "=r" (result)
  );
  return result;
}

// Set the Machine Cause Register
static inline void __set_MCAUSE(uint32_t value) {
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrw mcause, %0" : : "r" (value)
  );
}

// Return Vendor ID Register
static inline uint32_t __get_MVENDORID(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mvendorid" : "=r" (result)
  );
  return result;
}

// Return Machine Architecture ID Register
static inline uint32_t __get_MARCHID(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, marchid" : "=r" (result)
  );
  return result;
}

// Return Machine Implementation ID Register
static inline uint32_t __get_MIMPID(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mimpid" : "=r" (result)
  );
  return result;
}

// Return Hart ID Register
static inline uint32_t __get_MHARTID(void) {
  uint32_t result;
  __ASM volatile(
    #if __GNUC__ > 10
    ".option arch, +zicsr \n"
    #endif
    "csrr %0, mhartid" : "=r" (result)
  );
  return result;
}

// Return SP Register
static inline uint32_t __get_SP(void) {
  uint32_t result;
  __ASM volatile("mv %0, sp" : "=r" (result));
  return result;
}

#ifdef __cplusplus
};
#endif
