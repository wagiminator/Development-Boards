// ===================================================================================
// Basic System Functions for CH32X035/X034/X033                              * v1.0 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are 
// set up automatically on system start.
//
// System clock functions available:
// ---------------------------------
// CLK_init()               init system clock according to F_CPU
// CLK_reset()              reset system clock to default state
//
// HSI_enable()             enable internal 48 MHz high-speed clock (HSI)
// HSI_disable()            disable HSI
// HSI_ready()              check if HSI is stable
//
// MCO_init()               init clock output to pin PB9
// MCO_setSYS()             output SYS_CLK on pin PB9
// MCO_setHSI()             output internal oscillator on pin PB9
// MCO_stop()               stop clock output
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
// RST_wasOPA()             check if last reset was caused by OPA
//
// Independent Watchdog Timer (IWDG) functions available:
// ------------------------------------------------------
// IWDG_start(n)            start independent watchdog timer, n milliseconds, n<=5591
// IWDG_reload(n)           reload watchdog counter with n milliseconds, n<=5591
// IWDG_feed()              feed the dog (reload last time)
//
// Automatic Wake-up Timer (AWU) functions available:
// --------------------------------------------------
// AWU_start(n)             start AWU with n milliseconds period and event trigger
// AWU_stop()               stop AWU and event trigger
// AWU_set(n)               set AWU period to n milliseconds
//
// AWU_enable()             enable AWU
// AWU_disable()            disable AWU
// AWU_RT_enable()          enable AWU rising edge trigger
// AWU_RT_disable()         disable AWU rising edge trigger
// AWU_EV_enable()          enable AWU event
// AWU_EV_disable()         disable AWU event
// AWU_INT_enable()         enable AWU interrupt (without NVIC)
// AWU_INT_disable()        disable AWU interrupt (without NVIC)
//
// Sleep functions available:
// --------------------------
// SLEEP_WFI_now()          put device into SLEEP, wake up by interrupt
// SLEEP_WFE_now()          put device into SLEEP, wake up by event
// STOP_WFI_now()           put device into STOP, wake up by interrupt
// STOP_WFE_now()           put device into STOP, wake up by event
// STDBY_WFI_now()          put device into STANDBY (deep sleep), wake by interrupt
// STDBY_WFE_now()          put device into STANDBY (deep sleep), wake by event
//
// SLEEP_ms(n)              put device into SLEEP for n milliseconds (uses AWU)
// STOP_ms(n)               put device into STOP for n milliseconds (uses AWU)
// STDBY_ms(n)              put device into STANDBY for n milliseconds (uses AWU)
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

#include "ch32x035.h"

// ===================================================================================
// System Options (set "1" to activate)
// ===================================================================================
#define SYS_CLK_INIT      1         // 1: init system clock on startup
#define SYS_TICK_INIT     1         // 1: init and start SYSTICK on startup
#define SYS_GPIO_EN       1         // 1: enable GPIO ports on startup
#define SYS_CLEAR_BSS     0         // 1: clear uninitialized variables
#define SYS_USE_VECTORS   0         // 1: create interrupt vector table

// ===================================================================================
// Sytem Clock Defines
// ===================================================================================
// Set system clock frequency
#ifndef F_CPU
  #define F_CPU           8000000   // 8 Mhz if not otherwise defined
#endif

// Calculate system clock settings
#if   F_CPU ==  48000000
  #define SYS_CLK_DIV     RCC_HPRE_DIV1
#elif F_CPU ==  24000000
  #define SYS_CLK_DIV     RCC_HPRE_DIV2
#elif F_CPU ==  16000000
  #define SYS_CLK_DIV     RCC_HPRE_DIV3
#elif F_CPU ==  12000000
  #define SYS_CLK_DIV     RCC_HPRE_DIV4
#elif F_CPU ==   9600000
  #define SYS_CLK_DIV     RCC_HPRE_DIV5
#elif F_CPU ==   8000000
  #define SYS_CLK_DIV     RCC_HPRE_DIV6
#elif F_CPU ==   6000000
  #define SYS_CLK_DIV     RCC_HPRE_DIV8
#elif F_CPU ==   3000000
  #define SYS_CLK_DIV     RCC_HPRE_DIV16
#elif F_CPU ==   1500000
  #define SYS_CLK_DIV     RCC_HPRE_DIV32
#elif F_CPU ==    750000
  #define SYS_CLK_DIV     RCC_HPRE_DIV64
#elif F_CPU ==    375000
  #define SYS_CLK_DIV     RCC_HPRE_DIV128
#elif F_CPU ==    187500
  #define SYS_CLK_DIV     RCC_HPRE_DIV256
#else
  #warning Unsupported system clock frequency, using internal 8 MHz
  #define SYS_CLK_DIV     RCC_HPRE_DIV1
  #undef  F_CPU
  #define F_CPU           8000000
#endif

// ===================================================================================
// System Clock Functions
// ===================================================================================
void CLK_reset(void);     // reset system clock to default state
void MCO_init(void);      // init clock output to pin PB9

// Internal 48MHz high-speed clock (HSI) functions
#define CLK_init()        RCC->CFGR0 = SYS_CLK_DIV      // init system clock
#define HSI_enable()      RCC->CTLR |=  RCC_HSION       // enable HSI
#define HSI_disable()     RCC->CTLR &= ~RCC_HSION       // disable HSI
#define HSI_ready()       (RCC->CTLR &  RCC_HSIRDY)     // check if HSI is stable

// Clock output functions (pin PA8)
#define MCO_setSYS()      RCC->CFGR0 = (RCC->CFGR0 & ~RCC_CFGR0_MCO) | RCC_CFGR0_MCO_SYSCLK
#define MCO_setHSI()      RCC->CFGR0 = (RCC->CFGR0 & ~RCC_CFGR0_MCO) | RCC_CFGR0_MCO_HSI
#define MCO_stop()        RCC->CFGR0 &= ~RCC_CFGR0_MCO  // stop clock output to pin PB9

// ===================================================================================
// Delay (DLY) Functions
// ===================================================================================
void DLY_ticks(uint32_t n);                             // delay n system ticks

#define STK_init()        STK->CTLR = STK_CTLR_STE | STK_CTLR_STCLK // init SYSTICK @ F_CPU
#define DLY_US_TIME       (F_CPU / 1000000)             // system ticks per us
#define DLY_MS_TIME       (F_CPU / 1000)                // system ticks per ms
#define DLY_us(n)         DLY_ticks((n) * DLY_US_TIME)  // delay n microseconds
#define DLY_ms(n)         DLY_ticks((n) * DLY_MS_TIME)  // delay n milliseconds
#define DLY_cycles        DLY_ticks                     // alias

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
#define RST_wasOPA()      (RCC->RSTSCKR & RCC_OPARSTF)

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================
void IWDG_start_t(uint16_t ticks);                        // start IWDG with n ticks
void IWDG_reload_t(uint16_t ticks);                       // reload IWDG with n ticks

#define IWDG_start(n)     IWDG_start_t((HSI_VALUE/1000)*(n)/65536)
#define IWDG_reload(n)    IWDG_reload_t((HSI_VALUE/1000)*(n)/65536)
#define IWDG_feed()       IWDG->CTLR = 0xAAAA
#define IWDG_reset()      IWDG->CTLR = 0xAAAA             // alias

// ===================================================================================
// Automatic Wake-up Timer (AWU) Functions
// ===================================================================================
void AWU_init(void);
void AWU_stop(void);

#define AWU_start(n)          {AWU_init(); AWU_set(n);}
#define AWU_enable()          AWU->CSR = 0x02
#define AWU_disable()         AWU->CSR = 0x00
#define AWU_RT_enable()       EXTI->RTENR  |=  ((uint32_t)1 << 27)
#define AWU_RT_disable()      EXTI->RTENR  &= ~((uint32_t)1 << 27)
#define AWU_EV_enable()       EXTI->EVENR  |=  ((uint32_t)1 << 27)
#define AWU_EV_disable()      EXTI->EVENR  &= ~((uint32_t)1 << 27)
#define AWU_INT_enable()      EXTI->INTENR |=  ((uint32_t)1 << 27)
#define AWU_INT_disable()     EXTI->INTENR &= ~((uint32_t)1 << 27)

#define AWU_WRVAL(n)          ((HSI_VALUE/1000)*(n)/1024)
#define AWU_LIMIT(n)          (63*(n)*1024/(HSI_VALUE/1000))

#define AWU_set(n) \
  (n <= AWU_LIMIT(   32) ? ({AWU->PSC = 0b0110; AWU->WR = AWU_WRVAL(n) /    32;}) : \
  (n <= AWU_LIMIT(   64) ? ({AWU->PSC = 0b0111; AWU->WR = AWU_WRVAL(n) /    64;}) : \
  (n <= AWU_LIMIT(  128) ? ({AWU->PSC = 0b1000; AWU->WR = AWU_WRVAL(n) /   128;}) : \
  (n <= AWU_LIMIT(  256) ? ({AWU->PSC = 0b1001; AWU->WR = AWU_WRVAL(n) /   256;}) : \
  (n <= AWU_LIMIT(  512) ? ({AWU->PSC = 0b1010; AWU->WR = AWU_WRVAL(n) /   512;}) : \
  (n <= AWU_LIMIT( 1024) ? ({AWU->PSC = 0b1011; AWU->WR = AWU_WRVAL(n) /  1024;}) : \
  (n <= AWU_LIMIT( 2048) ? ({AWU->PSC = 0b1100; AWU->WR = AWU_WRVAL(n) /  2048;}) : \
  (n <= AWU_LIMIT( 4096) ? ({AWU->PSC = 0b1101; AWU->WR = AWU_WRVAL(n) /  4096;}) : \
  (n <= AWU_LIMIT(10240) ? ({AWU->PSC = 0b1110; AWU->WR = AWU_WRVAL(n) / 10240;}) : \
  (n <= AWU_LIMIT(61440) ? ({AWU->PSC = 0b1111; AWU->WR = AWU_WRVAL(n) / 61440;}) : \
  (0)))))))))))

#define AWU_SLEEP(n)          {AWU_set(n); SLEEP_WFE_now();}
#define AWU_STOP(n)           {AWU_set(n); STOP_WFE_now();}
#define AWU_STDBY(n)          {AWU_set(n); STDBY_WFE_now();}

// ===================================================================================
// Sleep Functions
// ===================================================================================
void SLEEP_WFI_now(void);   // put device into sleep, wake up by interrupt
void SLEEP_WFE_now(void);   // put device into sleep, wake up by event
void STOP_WFI_now(void);    // put device into stop, wake up by interrupt
void STOP_WFE_now(void);    // put device into stop, wake up by event
void STDBY_WFI_now(void);   // put device into standby (deep sleep), wake up interrupt
void STDBY_WFE_now(void);   // put device into standby (deep sleep), wake up event

#define SLEEP_ms(n)           {AWU_start(n); SLEEP_WFE_now(); AWU_stop();}
#define STOP_ms(n)            {AWU_start(n); STOP_WFE_now();  AWU_stop();}
#define STDBY_ms(n)           {AWU_start(n); STDBY_WFE_now(); AWU_stop();}

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
  __asm volatile("csrr %0, 0x800" : "=r" (result));
  __asm volatile("csrw 0x800, %0" :: "r" (0x6000));
  return result;
}

// Restore interrupt status
static inline void __iRestore(const uint32_t *__s) {
  __asm volatile("csrw 0x800, %0" :: "r" (*__s));
}

// ===================================================================================
// Device Electronic Signature (ESIG)
// ===================================================================================
#define ESIG_FLASHSIZE      (*(__I uint16_t*)(0x1FFFF7E0))
#define ESIG_UID1           (*(__I uint32_t*)(0x1FFFF7E8))
#define ESIG_UID2           (*(__I uint32_t*)(0x1FFFF7EC))
#define ESIG_UID3           (*(__I uint32_t*)(0x1FFFF7F0))

// ===================================================================================
// Imported System Functions
// ===================================================================================
// Enable Global Interrupt
__attribute__((always_inline)) static inline void __enable_irq(void) {
  __asm volatile("csrw 0x800, %0" : : "r" (0x6088));
}

// Disable Global Interrupt
__attribute__((always_inline)) static inline void __disable_irq(void) {
  __asm volatile("csrw 0x800, %0" : : "r" (0x6000));
}

// Return the Machine Status Register
__attribute__((always_inline)) static inline uint32_t __get_MSTATUS(void) {
  uint32_t result;
  __asm volatile("csrr %0, mstatus" : "=r"(result));
  return (result);
}

// Set the Machine Status Register
__attribute__((always_inline)) static inline void __set_MSTATUS(uint32_t value) {
  __asm volatile("csrw mstatus, %0" : : "r"(value));
}

// Return the Machine ISA Register
__attribute__((always_inline)) static inline uint32_t __get_MISA(void) {
  uint32_t result;
  __asm volatile("csrr %0, misa" : "=r"(result));
  return (result);
}

// Set the Machine ISA Register
__attribute__((always_inline)) static inline void __set_MISA(uint32_t value) {
  __asm volatile("csrw misa, %0" : : "r"(value));
}

// Return the Machine Trap-Vector Base-Address Register
__attribute__((always_inline)) static inline uint32_t __get_MTVEC(void) {
  uint32_t result;
  __asm volatile("csrr %0, mtvec": "=r"(result));
  return (result);
}

// Set the Machine Trap-Vector Base-Address Register
__attribute__((always_inline)) static inline void __set_MTVEC(uint32_t value) {
  __asm volatile("csrw mtvec, %0" : : "r"(value));
}

// Return the Machine Seratch Register
__attribute__((always_inline)) static inline uint32_t __get_MSCRATCH(void) {
  uint32_t result;
  __asm volatile("csrr %0, mscratch" : "=r"(result));
  return (result);
}

// Set the Machine Seratch Register
__attribute__((always_inline)) static inline void __set_MSCRATCH(uint32_t value) {
  __asm volatile("csrw mscratch, %0" : : "r"(value));
}

// Return the Machine Exception Program Register
__attribute__((always_inline)) static inline uint32_t __get_MEPC(void) {
  uint32_t result;
  __asm volatile("csrr %0, mepc" : "=r"(result));
  return (result);
}

// Set the Machine Exception Program Register
__attribute__((always_inline)) static inline void __set_MEPC(uint32_t value) {
  __asm volatile("csrw mepc, %0" : : "r"(value));
}

// Return the Machine Cause Register
__attribute__((always_inline)) static inline uint32_t __get_MCAUSE(void) {
  uint32_t result;
  __asm volatile("csrr %0, mcause": "=r"(result));
  return (result);
}

// Set the Machine Cause Register
__attribute__((always_inline)) static inline void __set_MCAUSE(uint32_t value) {
  __asm volatile("csrw mcause, %0" : : "r"(value));
}

// Return Vendor ID Register
__attribute__((always_inline)) static inline uint32_t __get_MVENDORID(void) {
  uint32_t result;
  __asm volatile("csrr %0, mvendorid" : "=r"(result));
  return (result);
}

// Return Machine Architecture ID Register
__attribute__((always_inline)) static inline uint32_t __get_MARCHID(void) {
  uint32_t result;
  __asm volatile("csrr %0, marchid" : "=r"(result));
  return (result);
}

// Return Machine Implementation ID Register
__attribute__((always_inline)) static inline uint32_t __get_MIMPID(void) {
  uint32_t result;
  __asm volatile("csrr %0, mimpid" : "=r"(result));
  return (result);
}

// Return Hart ID Register
__attribute__((always_inline)) static inline uint32_t __get_MHARTID(void) {
  uint32_t result;
  __asm volatile("csrr %0, mhartid" : "=r"(result));
  return (result);
}

// Return SP Register
__attribute__((always_inline)) static inline uint32_t __get_SP(void) {
  uint32_t result;
  __asm volatile("mv %0, sp" : "=r"(result):);
  return (result);
}

// No OPeration
__attribute__((always_inline)) static inline void __NOP(void) {
  __asm volatile("nop");
}

// Enable NVIC interrupt (interrupt numbers)
__attribute__((always_inline)) static inline void NVIC_EnableIRQ(IRQn_Type IRQn) {
  NVIC->IENR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Disable NVIC interrupt (interrupt numbers)
__attribute__((always_inline)) static inline void NVIC_DisableIRQ(IRQn_Type IRQn) {
  NVIC->IRER[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Get Interrupt Enable State
__attribute__((always_inline)) static inline uint32_t NVIC_GetStatusIRQ(IRQn_Type IRQn) {
  return((uint32_t) ((NVIC->ISR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Get Interrupt Pending State
__attribute__((always_inline)) static inline uint32_t NVIC_GetPendingIRQ(IRQn_Type IRQn) {
  return((uint32_t) ((NVIC->IPR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Set Interrupt Pending
__attribute__((always_inline)) static inline void NVIC_SetPendingIRQ(IRQn_Type IRQn) {
  NVIC->IPSR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Clear Interrupt Pending
__attribute__((always_inline)) static inline void NVIC_ClearPendingIRQ(IRQn_Type IRQn) {
  NVIC->IPRR[((uint32_t)(IRQn) >> 5)] = (1 << ((uint32_t)(IRQn) & 0x1F));
}

// Get Interrupt Active State
__attribute__((always_inline)) static inline uint32_t NVIC_GetActive(IRQn_Type IRQn) {
  return((uint32_t)((NVIC->IACTR[(uint32_t)(IRQn) >> 5] & (1 << ((uint32_t)(IRQn) & 0x1F)))?1:0));
}

// Set Interrupt Priority
__attribute__((always_inline)) static inline void NVIC_SetPriority(IRQn_Type IRQn, uint8_t priority) {
  NVIC->IPRIOR[(uint32_t)(IRQn)] = priority;
}

// Wait for Interrupt
__attribute__((always_inline))  static inline void __WFI(void) {
  NVIC->SCTLR &= ~(1<<3);   // wfi
  asm volatile ("wfi");
}

// Set Event
__attribute__((always_inline)) static inline void _SEV(void) {
  uint32_t t;
  t = NVIC->SCTLR;
  NVIC->SCTLR |= (1<<3)|(1<<5);
  NVIC->SCTLR = (NVIC->SCTLR & ~(1<<5)) | ( t & (1<<5));
}

// Wait for Events
__attribute__((always_inline)) static inline void _WFE(void) {
  NVIC->SCTLR |= (1<<3);
  __asm volatile ("wfi");
}

// Wait for Events
__attribute__((always_inline)) static inline void __WFE(void) {
  _SEV();
  _WFE();
  _WFE();
}

// Set VTF Interrupt
__attribute__((always_inline)) static inline void SetVTFIRQ(uint32_t addr, IRQn_Type IRQn, uint8_t num, FunctionalState NewState) {
  if(num > 3)  return;
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
__attribute__((always_inline)) static inline void NVIC_SystemReset(void) {
  NVIC->CFGR = NVIC_KEY3|(1<<7);
}

// Atomic Add with 32bit value
__attribute__((always_inline)) static inline int32_t __AMOADD_W(volatile int32_t *addr, int32_t value) {
  int32_t result;
  __asm volatile ("amoadd.w %0, %2, %1" : "=r"(result), "+A"(*addr) : "r"(value) : "memory");
  return *addr;
}

// Atomic And with 32bit value
__attribute__((always_inline)) static inline int32_t __AMOAND_W(volatile int32_t *addr, int32_t value) {
  int32_t result;
  __asm volatile ("amoand.w %0, %2, %1" : "=r"(result), "+A"(*addr) : "r"(value) : "memory");
  return *addr;
}

// Atomic signed MAX with 32bit value
__attribute__((always_inline)) static inline int32_t __AMOMAX_W(volatile int32_t *addr, int32_t value) {
  int32_t result;
  __asm volatile ("amomax.w %0, %2, %1" : "=r"(result), "+A"(*addr) : "r"(value) : "memory");
  return *addr;
}

// Atomic unsigned MAX with 32bit value
__attribute__((always_inline)) static inline uint32_t __AMOMAXU_W(volatile uint32_t *addr, uint32_t value) {
  uint32_t result;
  __asm volatile ("amomaxu.w %0, %2, %1" : "=r"(result), "+A"(*addr) : "r"(value) : "memory");
  return *addr;
}

// Atomic signed MIN with 32bit value
__attribute__((always_inline)) static inline int32_t __AMOMIN_W(volatile int32_t *addr, int32_t value) {
  int32_t result;
  __asm volatile ("amomin.w %0, %2, %1" : "=r"(result), "+A"(*addr) : "r"(value) : "memory");
  return *addr;
}

// Atomic unsigned MIN with 32bit value
__attribute__((always_inline)) static inline uint32_t __AMOMINU_W(volatile uint32_t *addr, uint32_t value) {
  uint32_t result;
  __asm volatile ("amominu.w %0, %2, %1" : "=r"(result), "+A"(*addr) : "r"(value) : "memory");
  return *addr;
}

// Atomic OR with 32bit value
__attribute__((always_inline)) static inline int32_t __AMOOR_W(volatile int32_t *addr, int32_t value) {
  int32_t result;
  __asm volatile ("amoor.w %0, %2, %1" : "=r"(result), "+A"(*addr) : "r"(value) : "memory");
  return *addr;
}

// Atomically swap new 32bit value into memory using amoswap.d
__attribute__((always_inline)) static inline uint32_t __AMOSWAP_W(volatile uint32_t *addr, uint32_t newval) {
  uint32_t result;
  __asm volatile ("amoswap.w %0, %2, %1" : "=r"(result), "+A"(*addr) : "r"(newval) : "memory");
  return result;
}

// Atomic XOR with 32bit value
__attribute__((always_inline)) static inline int32_t __AMOXOR_W(volatile int32_t *addr, int32_t value) {
  int32_t result;
  __asm volatile ("amoxor.w %0, %2, %1" : "=r"(result), "+A"(*addr) : "r"(value) : "memory");
  return *addr;
}

#ifdef __cplusplus
};
#endif
