// ===================================================================================
// Basic System Functions for STM32C011 and STM32C031                         * v1.0 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are 
// set up automatically on system start.
//
// System clock functions available:
// ---------------------------------
// CLK_init_HSI()           init internal oscillator as system clock source
// CLK_init_HSE()           init external crystal as system clock source
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
// LSI_enable()             enable internal 32kHz low-speed clock (LSI)
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
// MCO_setSYS()             enable output system clock (SYS_CLK) on MCO
// MCO_setHSI()             enable output internal high-speed clock (HSI) on MCO
// MCO_setHSE()             enable output external high-speed clock (HSE) on MCO
// MCO_setLSI()             enable output internal low-speed clock (LSI) on MCO
// MCO_setLSE()             enable output external low-speed clock (LSE) on MCO
// MCO_setPRE(n)            set MCO prescaler to 2^n (set before enabling MCO)
// MCO_stop()               disable clock output (MCO)
//
// Real-Time Clock (RTC) functions available:
// ------------------------------------------
// RTC_init()               init RTC with LSI as clock source and 1s clock period
// RTC_init_LSE()           init RTC with LSE as clock source and 1s clock period
// RTC_setPrescaler(a,s)    set RTC prescaler (async, sync)
// RTC_setTime(h,m,s)       set time (hours, minutes, seconds) in BCD format
// RTC_setDate(y,w,m,d)     set date (year, week day, month, day) in BCD format
// RTC_readTime()           read time register (must be post-processed)
// RTC_readDate()           read date register (must be post-processed)
// RTC_isInit()             check if RTC time/date were initialized (e.g. after reset)
//
// RTC_setAlarm(w,h,m,s)    set alarm (week day, hours, minutes, seconds) in BCD
// RTC_readAlarmFlag()      check alarm flag
// RTC_clearAlarmFlag()     clear alarm flag
// RTC_enableAlarmInt()     enable alarm interrupt
// RTC_disableAlarmInt()    disable alarm interrupt
//
// RTC_enableWakeEvent()    enable CPU wakeup with RTC event
// RTC_disableWakeEvent()   disable CPU wakeup with RTC event
// RTC_enableWakeInt()      enable CPU wakeup with RTC interrupt
// RTC_disableWakeInt()     disable CPU wakeup with RTC interrupt
//
// Delay (DLY) and SysTick (STK) functions available:
// --------------------------------------------------
// DLY_ticks(n)             delay n+1 clock cycles
// DLY_us(n)                delay n microseconds
// DLY_ms(n)                delay n milliseconds
//
// STK_enable()             enable SYSTICK at system frequency
// STK_disable()            disable SYSTICK
//
// Independent Watchdog Timer (IWDG) functions available:
// ------------------------------------------------------
// IWDG_start(n)            start independent watchdog timer, n milliseconds, n<=8191
// IWDG_reload(n)           reload watchdog counter with n milliseconds, n<=8191
// IWDG_feed()              feed the dog (reload last time)
//
// Sleep functions available:
// --------------------------
// SLEEP_WFI_now()          put device into SLEEP,    wake up by interrupt
// SLEEP_WFE_now()          put device into SLEEP,    wake up by event
// STOP_WFI_now()           put device into STOP,     wake up by interrupt
// STOP_WFE_now()           put device into STOP,     wake up by event
// STDBY_WFI_now()          put device into STANDBY,  wake up by interrupt
// STDBY_WFE_now()          put device into STANDBY,  wake up by event
// SHTDWN_WFI_now()         put device into SHUTDOWN, wake up by interrupt
// SHTDWN_WFE_now()         put device into SHUTDOWN, wake up by event
//
// Reset (RST) and Bootloader (BOOT) functions available:
// ------------------------------------------------------
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
// Backup Registers (BKP) functions available:
// -------------------------------------------
// BKP_read(reg)            read backup register (0..3)
// BKP_write(reg, val)      write value (16-bit) to backup register (0..3)
//
// Cyclic Redundancy Check (CRC) functions available:
// --------------------------------------------------
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
// Interrupt (INT) functions available:
// ------------------------------------
// INT_enable()             global interrupt enable
// INT_disable()            global interrupt disable
// INT_ATOMIC_BLOCK { }     execute block without being interrupted
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "stm32c0xx.h"

// ===================================================================================
// System Options (set "1" to activate)
// ===================================================================================
#define SYS_CLK_INIT      1         // 1: init system clock on startup
#define SYS_TICK_INIT     1         // 1: init and start SYSTICK on startup
#define SYS_GPIO_EN       1         // 1: enable GPIO ports on startup
#define SYS_CLEAR_BSS     0         // 1: clear uninitialized variables
#define SYS_USE_VECTORS   0         // 1: create interrupt vector table
#define SYS_USE_HSE       0         // 1: use external crystal

// ===================================================================================
// Sytem Clock Defines
// ===================================================================================

// Set system clock frequency
#ifndef F_CPU
  #define F_CPU           12000000   // 12Mhz if not otherwise defined
#endif

// Calculate system clock settings
#if SYS_USE_HSE == 0
  #if   F_CPU == 48000000
    #define CLK_DIV       (0b000<<11)
  #elif F_CPU == 24000000
    #define CLK_DIV       (0b001<<11)
  #elif F_CPU == 12000000
     #define CLK_DIV      (0b010<<11)
  #elif F_CPU ==  6000000
     #define CLK_DIV      (0b011<<11)
  #elif F_CPU ==  3000000
     #define CLK_DIV      (0b100<<11)
  #elif F_CPU ==  1500000
     #define CLK_DIV      (0b101<<11)
  #elif F_CPU ==   750000
     #define CLK_DIV      (0b110<<11)
  #elif F_CPU ==   375000
     #define CLK_DIV      (0b111<<11)
  #else
     #warning Unsupported system clock frequency, using internal 12 MHz
     #define CLK_DIV      (0b010<<11)
     #undef  F_CPU
     #define F_CPU        12000000     
  #endif
  #define CLK_init        CLK_init_HSI
#else
  #error HSE is not implemented yet
#endif

// ===================================================================================
// System Clock Functions
// ===================================================================================

// System clock init functions (called automatically on system start)
void CLK_init_HSI(void);      // init internal oscillator as system clock source
void CLK_init_HSE(void);      // init external crystal as system clock source
void CLK_reset(void);         // reset system clock to default state

// Internal high-speed clock (HSI) functions
#define HSI_enable()      RCC->CR |=  RCC_CR_HSION        // enable HSI
#define HSI_disable()     RCC->CR &= ~RCC_CR_HSION        // disable HSI
#define HSI_ready()       (RCC->CR & RCC_CR_HSIRDY)       // check if HSI is stable
#define HSI_divider(n)    (RCC->CR & RCC_CR_HSIDIV) | (((n)&7) << 11) // 2^n

// External high-speed clock (HSE) functions
#define HSE_enable()      RCC->CR |=  RCC_CR_HSEON        // enable HSE
#define HSE_disable()     RCC->CR &= ~RCC_CR_HSEON        // disable HSE
#define HSE_ready()       (RCC->CR & RCC_CR_HSERDY)       // check if HSE is stable
#define HSE_bypass_on()   RCC->CR |=  RCC_CR_HSEBYP       // enable HSE clock bypass
#define HSE_bypass_off()  RCC->CR &= ~RCC_CR_HSEBYP       // disable HSE clock bypass
#define HSE_CSS_on()      RCC->CR |=  RCC_CR_CSSON        // enable HSE clock security
#define HSE_CSS_off()     RCC->CR &= ~RCC_CR_CSSON        // enable HSE clock security

// Internal 32kHz low-speed clock (LSI) functions
#define LSI_enable()      RCC->CSR2 |=  RCC_CSR2_LSION    // enable LSI
#define LSI_disable()     RCC->CSR2 &= ~RCC_CSR2_LSION    // disable LSI
#define LSI_ready()       (RCC->CSR2 & RCC_CSR2_LSIRDY)   // check if LSI is stable

// External 32.768kHz low-speed clock (LSE) functions
#define LSE_enable()      RCC->CSR1 |=  RCC_CSR1_LSEON    // enable LSE
#define LSE_disable()     RCC->CSR1 &= ~RCC_CSR1_LSEON    // disable LSE
#define LSE_ready()       (RCC->CSR1 & RCC_CSR1_LSERDY)   // check if LSE is stable
#define LSE_bypass_on()   RCC->CSR1 |=  RCC_CSR1_LSEBYP   // enable LSE clock bypass
#define LSE_bypass_off()  RCC->CSR1 &= ~RCC_CSR1_LSEBYP   // disable LSE clock bypass
#define LSE_CSS_on()      RCC->CSR1 |=  RCC_CSR1_LSECSSON // enable LSE clock security
#define LSE_CSS_off()     RCC->CSR1 &= ~RCC_CSR1_LSECSSON // enable LSE clock security

// Clock output functions (MCO)
#define MCO_setSYS()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0001 << 24)
#define MCO_setHSI()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0011 << 24)
#define MCO_setHSE()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0100 << 24)
#define MCO_setLSI()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0110 << 24)
#define MCO_setLSE()      RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOSEL) | (0b0111 << 24)
#define MCO_setPRE(n)     RCC->CFGR = (RCC->CFGR & ~RCC_CFGR_MCOPRE) | (((n)&15) << 28)
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
void RTC_setAlarm(uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds);

#define RTC_isInit()            (RTC->ICSR & RTC_ICSR_INITS)
#define RTC_readAlarmFlag()     (RTC->SR & RTC_SR_ALRAF)
#define RTC_clearWakeupFlag()   RTC->SCR = RTC_SCR_CWUTF
#define RTC_clearAlarmFlag()    RTC->SCR = RTC_SCR_CALRAF
#define RTC_enableWakeupInt()   RTC->CR |=  RTC_CR_WUTIE
#define RTC_disableWakeupInt()  RTC->CR &= ~RTC_CR_WUTIE
#define RTC_enableAlarmInt()    RTC->CR |=  RTC_CR_ALRAIE
#define RTC_disableAlarmInt()   RTC->CR &= ~RTC_CR_ALRAIE
#define RTC_enableWakeEvent()   EXTI->EMR1  |=  ((uint32_t)1 << 19)
#define RTC_disableWakeEvent()  EXTI->EMR1  &= ~((uint32_t)1 << 19)
#define RTC_enableWakeInt()     EXTI->IMR1  |=  ((uint32_t)1 << 19)
#define RTC_disableWakeInt()    EXTI->IMR1  &= ~((uint32_t)1 << 19)

// ===================================================================================
// SYSTICK Functions
// ===================================================================================
#define STK_enable()      SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE
#define STK_disable()     SysTick->CTRL = 0

// ===================================================================================
// Delay (DLY) Functions (using SYSTICK)
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
void SLEEP_WFI_now(void);       // put device into SLEEP,    wake up by interrupt
void SLEEP_WFE_now(void);       // put device into SLEEP,    wake up by event
void STOP_WFI_now(void);        // put device into STOP,     wake up interrupt
void STOP_WFE_now(void);        // put device into STOP,     wake up event
void STDBY_WFI_now(void);       // put device into STANDBY,  wake up interrupt
void STDBY_WFE_now(void);       // put device into STANDBY,  wake up event
void SHTDWN_WFI_now(void);      // put device into SHUTDOWN, wake up interrupt
void SHTDWN_WFE_now(void);      // put device into SHUTDOWN, wake up event

// ===================================================================================
// Reset (RST) Functions
// ===================================================================================
#define RST_now()           NVIC_SystemReset()
#define RST_clearFlags()    RCC->CSR2 |= RCC_CSR2_RMVF
#define RST_wasLowPower()   (RCC->CSR2 & RCC_CSR2_LPWRRSTF)
#define RST_wasWWDG()       (RCC->CSR2 & RCC_CSR2_WWDGRSTF)
#define RST_wasIWDG()       (RCC->CSR2 & RCC_CSR2_IWDGRSTF)
#define RST_wasSoftware()   (RCC->CSR2 & RCC_CSR2_SFTRSTF)
#define RST_wasPower()      (RCC->CSR2 & RCC_CSR2_PWRRSTF)
#define RST_wasPin()        (RCC->CSR2 & RCC_CSR2_PINRSTF)
#define RST_wasOption()     (RCC->CSR2 & RCC_CSR2_OBLRSTF)

// ===================================================================================
// Bootloader (BOOT) Functions
// ===================================================================================
#define BOOT_ADDR           0x1fff0000
void BOOT_now(void);        // jump to bootloader

// ===================================================================================
// Backup Registers (BKP) Functions
// ===================================================================================
uint16_t BKP_read(uint8_t reg);             // read backup register
void BKP_write(uint8_t reg, uint16_t val);  // Write backup register

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
#define ESIG_FLASHSIZE      (*(__I uint16_t*)(0x1FFF75A0))
#define ESIG_UID1           (*(__I uint32_t*)(0x1FFF7550))
#define ESIG_UID2           (*(__I uint32_t*)(0x1FFF7554))
#define ESIG_UID3           (*(__I uint32_t*)(0x1FFF7558))

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

#if defined (__MPU_PRESENT) && (__MPU_PRESENT == 1U)

#define ARM_MPU_REGION_SIZE_32B      ((uint8_t)0x04U) ///!< MPU Region Size 32 Bytes
#define ARM_MPU_REGION_SIZE_64B      ((uint8_t)0x05U) ///!< MPU Region Size 64 Bytes
#define ARM_MPU_REGION_SIZE_128B     ((uint8_t)0x06U) ///!< MPU Region Size 128 Bytes
#define ARM_MPU_REGION_SIZE_256B     ((uint8_t)0x07U) ///!< MPU Region Size 256 Bytes
#define ARM_MPU_REGION_SIZE_512B     ((uint8_t)0x08U) ///!< MPU Region Size 512 Bytes
#define ARM_MPU_REGION_SIZE_1KB      ((uint8_t)0x09U) ///!< MPU Region Size 1 KByte
#define ARM_MPU_REGION_SIZE_2KB      ((uint8_t)0x0AU) ///!< MPU Region Size 2 KBytes
#define ARM_MPU_REGION_SIZE_4KB      ((uint8_t)0x0BU) ///!< MPU Region Size 4 KBytes
#define ARM_MPU_REGION_SIZE_8KB      ((uint8_t)0x0CU) ///!< MPU Region Size 8 KBytes
#define ARM_MPU_REGION_SIZE_16KB     ((uint8_t)0x0DU) ///!< MPU Region Size 16 KBytes
#define ARM_MPU_REGION_SIZE_32KB     ((uint8_t)0x0EU) ///!< MPU Region Size 32 KBytes
#define ARM_MPU_REGION_SIZE_64KB     ((uint8_t)0x0FU) ///!< MPU Region Size 64 KBytes
#define ARM_MPU_REGION_SIZE_128KB    ((uint8_t)0x10U) ///!< MPU Region Size 128 KBytes
#define ARM_MPU_REGION_SIZE_256KB    ((uint8_t)0x11U) ///!< MPU Region Size 256 KBytes
#define ARM_MPU_REGION_SIZE_512KB    ((uint8_t)0x12U) ///!< MPU Region Size 512 KBytes
#define ARM_MPU_REGION_SIZE_1MB      ((uint8_t)0x13U) ///!< MPU Region Size 1 MByte
#define ARM_MPU_REGION_SIZE_2MB      ((uint8_t)0x14U) ///!< MPU Region Size 2 MBytes
#define ARM_MPU_REGION_SIZE_4MB      ((uint8_t)0x15U) ///!< MPU Region Size 4 MBytes
#define ARM_MPU_REGION_SIZE_8MB      ((uint8_t)0x16U) ///!< MPU Region Size 8 MBytes
#define ARM_MPU_REGION_SIZE_16MB     ((uint8_t)0x17U) ///!< MPU Region Size 16 MBytes
#define ARM_MPU_REGION_SIZE_32MB     ((uint8_t)0x18U) ///!< MPU Region Size 32 MBytes
#define ARM_MPU_REGION_SIZE_64MB     ((uint8_t)0x19U) ///!< MPU Region Size 64 MBytes
#define ARM_MPU_REGION_SIZE_128MB    ((uint8_t)0x1AU) ///!< MPU Region Size 128 MBytes
#define ARM_MPU_REGION_SIZE_256MB    ((uint8_t)0x1BU) ///!< MPU Region Size 256 MBytes
#define ARM_MPU_REGION_SIZE_512MB    ((uint8_t)0x1CU) ///!< MPU Region Size 512 MBytes
#define ARM_MPU_REGION_SIZE_1GB      ((uint8_t)0x1DU) ///!< MPU Region Size 1 GByte
#define ARM_MPU_REGION_SIZE_2GB      ((uint8_t)0x1EU) ///!< MPU Region Size 2 GBytes
#define ARM_MPU_REGION_SIZE_4GB      ((uint8_t)0x1FU) ///!< MPU Region Size 4 GBytes

#define ARM_MPU_AP_NONE 0U ///!< MPU Access Permission no access
#define ARM_MPU_AP_PRIV 1U ///!< MPU Access Permission privileged access only
#define ARM_MPU_AP_URO  2U ///!< MPU Access Permission unprivileged access read-only
#define ARM_MPU_AP_FULL 3U ///!< MPU Access Permission full access
#define ARM_MPU_AP_PRO  5U ///!< MPU Access Permission privileged access read-only
#define ARM_MPU_AP_RO   6U ///!< MPU Access Permission read-only access

// MPU Region Base Address Register Value
#define ARM_MPU_RBAR(Region, BaseAddress) \
  (((BaseAddress) & MPU_RBAR_ADDR_Msk) |  \
   ((Region) & MPU_RBAR_REGION_Msk)    |  \
   (MPU_RBAR_VALID_Msk))

// MPU Memory Access Attributes 
#define ARM_MPU_ACCESS_(TypeExtField, IsShareable, IsCacheable, IsBufferable)   \
  ((((TypeExtField) << MPU_RASR_TEX_Pos) & MPU_RASR_TEX_Msk)                  | \
   (((IsShareable)  << MPU_RASR_S_Pos)   & MPU_RASR_S_Msk)                    | \
   (((IsCacheable)  << MPU_RASR_C_Pos)   & MPU_RASR_C_Msk)                    | \
   (((IsBufferable) << MPU_RASR_B_Pos)   & MPU_RASR_B_Msk))

// MPU Region Attribute and Size Register Value
#define ARM_MPU_RASR_EX(DisableExec, AccessPermission, AccessAttributes, SubRegionDisable, Size)    \
  ((((DisableExec)      << MPU_RASR_XN_Pos)   & MPU_RASR_XN_Msk)                                  | \
   (((AccessPermission) << MPU_RASR_AP_Pos)   & MPU_RASR_AP_Msk)                                  | \
   (((AccessAttributes) & (MPU_RASR_TEX_Msk | MPU_RASR_S_Msk | MPU_RASR_C_Msk | MPU_RASR_B_Msk))) | \
   (((SubRegionDisable) << MPU_RASR_SRD_Pos)  & MPU_RASR_SRD_Msk)                                 | \
   (((Size)             << MPU_RASR_SIZE_Pos) & MPU_RASR_SIZE_Msk)                                | \
   (((MPU_RASR_ENABLE_Msk))))

// MPU Region Attribute and Size Register Value                     
#define ARM_MPU_RASR(DisableExec, AccessPermission, TypeExtField, IsShareable, IsCacheable, IsBufferable, SubRegionDisable, Size) \
  ARM_MPU_RASR_EX(DisableExec, AccessPermission, ARM_MPU_ACCESS_(TypeExtField, IsShareable, IsCacheable, IsBufferable), SubRegionDisable, Size)

// MPU Memory Access Attribute for strongly ordered memory.
#define ARM_MPU_ACCESS_ORDERED ARM_MPU_ACCESS_(0U, 1U, 0U, 0U)

// MPU Memory Access Attribute for device memory.
#define ARM_MPU_ACCESS_DEVICE(IsShareable) ((IsShareable) ? ARM_MPU_ACCESS_(0U, 1U, 0U, 1U) : ARM_MPU_ACCESS_(2U, 0U, 0U, 0U))

// MPU Memory Access Attribute for normal memory.
#define ARM_MPU_ACCESS_NORMAL(OuterCp, InnerCp, IsShareable) ARM_MPU_ACCESS_((4U | (OuterCp)), IsShareable, ((InnerCp) & 2U), ((InnerCp) & 1U))

// MPU Memory Access Attribute non-cacheable policy.
#define ARM_MPU_CACHEP_NOCACHE 0U

// MPU Memory Access Attribute write-back, write and read allocate policy.
#define ARM_MPU_CACHEP_WB_WRA 1U

// MPU Memory Access Attribute write-through, no write allocate policy.
#define ARM_MPU_CACHEP_WT_NWA 2U

// MPU Memory Access Attribute write-back, no write allocate policy.
#define ARM_MPU_CACHEP_WB_NWA 3U

// Struct for a single MPU Region
typedef struct {
  uint32_t RBAR; //!< The region base address register value (RBAR)
  uint32_t RASR; //!< The region attribute and size register value (RASR) \ref MPU_RASR
} ARM_MPU_Region_t;
    
// Enable the MPU.
__STATIC_INLINE void ARM_MPU_Enable(uint32_t MPU_Control) {
  MPU->CTRL = MPU_Control | MPU_CTRL_ENABLE_Msk;
#ifdef SCB_SHCSR_MEMFAULTENA_Msk
  SCB->SHCSR |= SCB_SHCSR_MEMFAULTENA_Msk;
#endif
  __DSB();
  __ISB();
}

// Disable the MPU.
__STATIC_INLINE void ARM_MPU_Disable(void) {
  __DMB();
#ifdef SCB_SHCSR_MEMFAULTENA_Msk
  SCB->SHCSR &= ~SCB_SHCSR_MEMFAULTENA_Msk;
#endif
  MPU->CTRL  &= ~MPU_CTRL_ENABLE_Msk;
}

// Clear and disable the given MPU region.
__STATIC_INLINE void ARM_MPU_ClrRegion(uint32_t rnr) {
  MPU->RNR = rnr;
  MPU->RASR = 0U;
}

// Configure an MPU region. 
__STATIC_INLINE void ARM_MPU_SetRegion(uint32_t rbar, uint32_t rasr) {
  MPU->RBAR = rbar;
  MPU->RASR = rasr;
}

// Configure the given MPU region. 
__STATIC_INLINE void ARM_MPU_SetRegionEx(uint32_t rnr, uint32_t rbar, uint32_t rasr) {
  MPU->RNR = rnr;
  MPU->RBAR = rbar;
  MPU->RASR = rasr;
}

// Memcopy with strictly ordered memory access, e.g. for register targets.
__STATIC_INLINE void ARM_MPU_OrderedMemcpy(volatile uint32_t* dst, const uint32_t* __RESTRICT src, uint32_t len) {
  uint32_t i;
  for (i = 0U; i < len; ++i) dst[i] = src[i];
}

// Load the given number of MPU regions from a table.
__STATIC_INLINE void ARM_MPU_Load(ARM_MPU_Region_t const* table, uint32_t cnt)  {
  const uint32_t rowWordSize = sizeof(ARM_MPU_Region_t)/4U;
  while (cnt > MPU_TYPE_RALIASES) {
    ARM_MPU_OrderedMemcpy(&(MPU->RBAR), &(table->RBAR), MPU_TYPE_RALIASES*rowWordSize);
    table += MPU_TYPE_RALIASES;
    cnt -= MPU_TYPE_RALIASES;
  }
  ARM_MPU_OrderedMemcpy(&(MPU->RBAR), &(table->RBAR), cnt*rowWordSize);
}

#endif // MPU section

#ifdef __cplusplus
};
#endif
