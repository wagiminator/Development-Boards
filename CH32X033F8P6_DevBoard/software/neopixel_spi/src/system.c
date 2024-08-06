// ===================================================================================
// Basic System Functions for CH32X035/X034/X033                              * v1.2 *
// ===================================================================================
//
// This file must be included!!!!
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "system.h"

// ===================================================================================
// Setup Microcontroller (this function is called automatically at startup)
// ===================================================================================
void SYS_init(void) {

  // Init system clock
  #if SYS_CLK_INIT > 0
    #if F_CPU <= 12000000
      //FLASH->ACTLR = FLASH_ACTLR_LATENCY_0;
    #elif F_CPU <= 24000000
      FLASH->ACTLR = FLASH_ACTLR_LATENCY_1;
    #else
      FLASH->ACTLR = FLASH_ACTLR_LATENCY_2;
    #endif
    RCC->CFGR0 = SYS_CLK_DIV;
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
    STK_init();
  #endif

  // Enable GPIO
  #if SYS_GPIO_EN > 0
    RCC->APB2PCENR = RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN;
  #endif
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Reset system clock to default state
void CLK_reset(void) {
  RCC->CTLR   |= (uint32_t)0x00000001;
  RCC->CFGR0   = (uint32_t)0x00000050;
  FLASH->ACTLR = (uint32_t)0x00000000;
}

// Setup pin PB9 for MCO (output, push-pull, alternate)
void MCO_init(void) {
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN;
  GPIOB->CFGHR    = (GPIOB->CFGHR & ~((uint32_t)0b1111<<(1<<2))) | ((uint32_t)0b1011<<(1<<2));
  RCC->CFGR0     |= RCC_CFGR0_MCO_SYSCLK;
}

// ===================================================================================
// Delay Functions
// ===================================================================================

// Wait n counts of SysTick
void DLY_ticks(uint32_t n) {
  uint32_t end = STK->CNTL + n;
  while(((int32_t)(STK->CNTL - end)) < 0);
}

// ===================================================================================
// Bootloader (BOOT) Functions
// ===================================================================================

// Perform software reset and jump to bootloader
void BOOT_now(void) {
  FLASH->KEYR = FLASH_KEY1;
  FLASH->KEYR = FLASH_KEY2;
  FLASH->BOOT_MODEKEYR = FLASH_KEY1;
  FLASH->BOOT_MODEKEYR = FLASH_KEY2;      // unlock flash
  FLASH->STATR = FLASH_STATR_BOOT_MODE;   // start bootloader after software reset
  FLASH->CTLR  = FLASH_CTLR_LOCK;         // lock flash
  RCC->RSTSCKR = RCC_RMVF;                // clear reset flags
  PFIC->SCTLR  = PFIC_SYSRESET;           // perform software reset
}

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================

// Start independent watchdog timer (IWDG) with given amount of WDG clock cycles
// (ticks). The IWDG clock is HSI/1024/prescaler. One tick is 64*1024*1000/48000000 ms
// long, max ticks is 4095 = 5591ms.
// Once the IWDG has been started, it cannot be disabled, only reloaded (feed).
void IWDG_start_t(uint16_t ticks) {
  IWDG->CTLR = 0x5555;                    // allow register modification
  while(IWDG->STATR & IWDG_PVU);          // wait for clock register to be ready
  IWDG->PSCR = 0b100;                     // set clock prescaler 64
  while(IWDG->STATR & IWDG_RVU);          // wait for reload register to be ready
  IWDG->RLDR = ticks;                     // set watchdog counter reload value
  IWDG->CTLR = 0xAAAA;                    // load reload value into watchdog counter
  IWDG->CTLR = 0xCCCC;                    // enable IWDG
}

// Reload watchdog counter with n ticks, n<=4095
void IWDG_reload_t(uint16_t ticks) {
  IWDG->CTLR = 0x5555;                    // allow register modification
  while(IWDG->STATR & IWDG_RVU);          // wait for reload register to be ready
  IWDG->RLDR = ticks;                     // set watchdog counter reload value
  IWDG->CTLR = 0xAAAA;                    // load reload value into watchdog counter
}

// ===================================================================================
// Automatic Wake-up Timer (AWU) Functions
// ===================================================================================

// Init and start automatic wake-up timer
void AWU_init(void) {
  AWU->CSR = 0x02;                        // enable automatic wake-up timer
  EXTI->EVENR |= ((uint32_t)1<<27);       // enable AWU event
  EXTI->RTENR |= ((uint32_t)1<<27);       // enable AWU rising edge triggering
}

// Stop automatic wake-up timer
void AWU_stop(void) {
  AWU->CSR = 0x00;                        // disable automatic wake-up timer
  EXTI->EVENR &= ~((uint32_t)1<<27);      // disable AWU event
  EXTI->RTENR &= ~((uint32_t)1<<27);      // disable AWU rising edge triggering
}

// ===================================================================================
// Sleep Functions
// ===================================================================================

// Put device into sleep, wake up by interrupt
void SLEEP_WFI_now(void) {
  PFIC->SCTLR &= ~PFIC_SLEEPDEEP;         // set power-down mode to SLEEP
  __WFI();                                // wait for interrupt
}

// Put device into sleep, wake up by event
void SLEEP_WFE_now(void) {
  PFIC->SCTLR &= ~PFIC_SLEEPDEEP;         // set power-down mode to SLEEP
  __WFE();                                // wait for event
}

// Put device into stop, wake up interrupt
void STOP_WFI_now(void) {
  PFIC->SCTLR |= PFIC_SLEEPDEEP;          // set power-down mode to STOP
  __WFI();                                // wait for interrupt
}

// Put device into stop, wake up event
void STOP_WFE_now(void) {
  PFIC->SCTLR |= PFIC_SLEEPDEEP;          // set power-down mode to STOP
  __WFE();                                // wait for event
}

// Put device into standby (deep sleep), wake up interrupt
void STDBY_WFI_now(void) {
  RCC->APB1PCENR |= RCC_PWREN;            // enable power module
  PWR->CTLR      |= PWR_CTLR_PDDS;        // set power-down mode to STANDBY
  PFIC->SCTLR    |= PFIC_SLEEPDEEP;
  __WFI();                                // wait for interrupt
  PWR->CTLR      &= ~PWR_CTLR_PDDS;       // reset power-down mode
}

// Put device into standby (deep sleep), wake up event
void STDBY_WFE_now(void) {
  RCC->APB1PCENR |= RCC_PWREN;            // enable power module
  PWR->CTLR      |= PWR_CTLR_PDDS;        // set power-down mode to STANDBY
  PFIC->SCTLR    |= PFIC_SLEEPDEEP;
  __WFE();                                // wait for event
  PWR->CTLR      &= ~PWR_CTLR_PDDS;       // reset power-down mode
}

// ===================================================================================
// C++ Support
// ===================================================================================
#ifdef __cplusplus
extern void __cxa_pure_virtual() { while (1); }
extern void (*__preinit_array_start[]) (void) __attribute__((weak));
extern void (*__preinit_array_end[]) (void) __attribute__((weak));
extern void (*__init_array_start[]) (void) __attribute__((weak));
extern void (*__init_array_end[]) (void) __attribute__((weak));

void __libc_init_array(void) {
  uint32_t count, i;
  count = __preinit_array_end - __preinit_array_start;
  for (i = 0; i < count; i++) __preinit_array_start[i]();
  count = __init_array_end - __init_array_start;
  for (i = 0; i < count; i++) __init_array_start[i]();
}
#endif

// ===================================================================================
// C version of CH32X035 Startup .s file from WCH
// ===================================================================================
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _data_lma;
extern uint32_t _data_vma;
extern uint32_t _edata;

// Prototypes
int main(void)                __attribute__((section(".text.main"), used));
void jump_reset(void)         __attribute__((section(".init.jump"), naked, used));
void (*const vectors[])(void) __attribute__((section(".vector"), used));
void reset_handler(void)      __attribute__((section(".text.reset_handler"), naked, used));

#if SYS_USE_VECTORS > 0
// Unless a specific handler is overridden, it just spins forever
void default_handler(void)    __attribute__((section(".text.vector_handler"), naked, used));
void default_handler(void)    { while(1); }

// All interrupt handlers are aliased to default_handler unless overridden individually
#define DUMMY_HANDLER __attribute__((section(".text.vector_handler"), weak, alias("default_handler"), used))
DUMMY_HANDLER void NMI_Handler(void);
DUMMY_HANDLER void HardFault_Handler(void);
DUMMY_HANDLER void Ecall_M_Mode_Handler(void);
DUMMY_HANDLER void Ecall_U_Mode_Handler(void);
DUMMY_HANDLER void Break_Point_Handler(void);
DUMMY_HANDLER void SysTick_Handler(void);
DUMMY_HANDLER void SW_Handler(void);
DUMMY_HANDLER void WWDG_IRQHandler(void);
DUMMY_HANDLER void PVD_IRQHandler(void);
DUMMY_HANDLER void FLASH_IRQHandler(void);
DUMMY_HANDLER void EXTI7_0_IRQHandler(void);
DUMMY_HANDLER void AWU_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel1_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel2_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel3_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel4_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel5_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel6_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel7_IRQHandler(void);
DUMMY_HANDLER void ADC1_IRQHandler(void);
DUMMY_HANDLER void I2C1_EV_IRQHandler(void);
DUMMY_HANDLER void I2C1_ER_IRQHandler(void);
DUMMY_HANDLER void USART1_IRQHandler(void);
DUMMY_HANDLER void SPI1_IRQHandler(void);
DUMMY_HANDLER void TIM1_BRK_IRQHandler(void);
DUMMY_HANDLER void TIM1_UP_IRQHandler(void);
DUMMY_HANDLER void TIM1_TRG_COM_IRQHandler(void);
DUMMY_HANDLER void TIM1_CC_IRQHandler(void);
DUMMY_HANDLER void TIM2_UP_IRQHandler(void);
DUMMY_HANDLER void USART2_IRQHandler(void);
DUMMY_HANDLER void EXTI15_8_IRQHandler(void);
DUMMY_HANDLER void EXTI25_16_IRQHandler(void);
DUMMY_HANDLER void USART3_IRQHandler(void);
DUMMY_HANDLER void USART4_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel8_IRQHandler(void);
DUMMY_HANDLER void USBFS_IRQHandler(void);
DUMMY_HANDLER void USBFSWakeUp_IRQHandler(void);
DUMMY_HANDLER void PIOC_IRQHandler(void);
DUMMY_HANDLER void OPA_IRQHandler(void);
DUMMY_HANDLER void USBPD_IRQHandler(void);
DUMMY_HANDLER void USBPDWakeUp_IRQHandler(void);
DUMMY_HANDLER void TIM2_CC_IRQHandler(void);
DUMMY_HANDLER void TIM2_TRG_COM_IRQHandler(void);
DUMMY_HANDLER void TIM2_BRK_IRQHandler(void);
DUMMY_HANDLER void TIM3_IRQHandler(void);
#endif  // SYS_USE_VECTORS > 0

// FLASH starts with a jump to the reset handler
void jump_reset(void) { asm volatile("j reset_handler"); }

// Interrupt vector table
void (* const vectors[])(void) = {
  // RISC-V handlers
  jump_reset,                       //  0 - Reset
  #if SYS_USE_VECTORS > 0
  0,                                //  1 - Reserved
  NMI_Handler,                      //  2 - NMI Handler
  HardFault_Handler,                //  3 - Hard Fault Handler
  0,                                //  4 - Reserved
  Ecall_M_Mode_Handler,             //  5 - Ecall M Mode Handler
  0,                                //  6 - Reserved
  0,                                //  7 - Reserved
  Ecall_U_Mode_Handler,             //  8 - Ecall U Mode Handler
  Break_Point_Handler,              //  9 - Break Point Handler
  0,                                // 10 - Reserved
  0,                                // 11 - Reserved
  SysTick_Handler,                  // 12 - SysTick Handler
  0,                                // 13 - Reserved
  SW_Handler,                       // 14 - SW Handler
  0,                                // 15 - Reserved
  
  // Peripheral handlers
  WWDG_IRQHandler,                  // 16 - Window Watchdog
  PVD_IRQHandler,                   // 17 - PVD through EXTI Line detect
  FLASH_IRQHandler,                 // 18 - Flash
  0,                                // 19 - Reserved
  EXTI7_0_IRQHandler,               // 20 - EXTI Line 7..0
  AWU_IRQHandler,                   // 21 - Auto Wake-up
  DMA1_Channel1_IRQHandler,         // 22 - DMA1 Channel 1
  DMA1_Channel2_IRQHandler,         // 23 - DMA1 Channel 2
  DMA1_Channel3_IRQHandler,         // 24 - DMA1 Channel 3
  DMA1_Channel4_IRQHandler,         // 25 - DMA1 Channel 4
  DMA1_Channel5_IRQHandler,         // 26 - DMA1 Channel 5
  DMA1_Channel6_IRQHandler,         // 27 - DMA1 Channel 6
  DMA1_Channel7_IRQHandler,         // 28 - DMA1 Channel 7
  ADC1_IRQHandler,                  // 29 - ADC1
  I2C1_EV_IRQHandler,               // 30 - I2C1 Event
  I2C1_ER_IRQHandler,               // 31 - I2C1 Error
  USART1_IRQHandler,                // 32 - USART1
  SPI1_IRQHandler,                  // 33 - SPI1
  TIM1_BRK_IRQHandler,              // 34 - TIM1 Break
  TIM1_UP_IRQHandler,               // 35 - TIM1 Update
  TIM1_TRG_COM_IRQHandler,          // 36 - TIM1 Trigger and Commutation
  TIM1_CC_IRQHandler,               // 37 - TIM1 Capture Compare
  TIM2_UP_IRQHandler,               // 38 - TIM2 Update
  USART2_IRQHandler,                // 39 - USART2
  EXTI15_8_IRQHandler,              // 40 - EXTI Line 15..8
  EXTI25_16_IRQHandler,             // 41 - EXTI Line 25..16
  USART3_IRQHandler,                // 42 - USART3
  USART4_IRQHandler,                // 43 - USART4
  DMA1_Channel8_IRQHandler,         // 44 - DMA1 Channel8
  USBFS_IRQHandler,                 // 45 - USBFS Break
  USBFSWakeUp_IRQHandler,           // 46 - USBFS Wake up from suspend
  PIOC_IRQHandler,                  // 47 - PIOC
  OPA_IRQHandler,                   // 48 - OPA
  USBPD_IRQHandler,                 // 49 - USBPD
  USBPDWakeUp_IRQHandler,           // 50 - USBPD Wake-up
  TIM2_CC_IRQHandler,               // 51 - TIM2 Capture Compare
  TIM2_TRG_COM_IRQHandler,          // 52 - TIM2 Trigger and Commutation
  TIM2_BRK_IRQHandler,              // 53 - TIM2 Break
  TIM3_IRQHandler                   // 54 - TIM3
  #endif  // SYS_USE_VECTORS > 0
};

// Reset handler
void reset_handler(void) {
  uint32_t *src, *dst;
  
  // Set pointers, vectors, processor status, and interrupts
  asm volatile(
  " .option push              \n\
    .option norelax           \n\
    la gp, __global_pointer$  \n\
    .option pop               \n\
    la sp, _eusrstack         \n"
    #if __GNUC__ > 10
    ".option arch, +zicsr     \n"
    #endif
  " li a0, 0x1f               \n\
    csrw 0xbc0, a0            \n\
    li a0, 0x88               \n\
    csrs mstatus, a0          \n\
    li a1, 0x3                \n\
    csrw 0x804, a1            \n\
    la a0, vectors            \n\
    or a0, a0, a1             \n\
    csrw mtvec, a0            \n\
    csrw mepc, %[main]        \n"
    : : [main] "r" (main) : "a0", "a1" , "memory"
  );

  // Copy data from FLASH to RAM
  src = &_data_lma;
  dst = &_data_vma;
  while(dst < &_edata) *dst++ = *src++;

  // Clear uninitialized variables
  #if SYS_CLEAR_BSS > 0
  dst = &_sbss;
  while(dst < &_ebss) *dst++ = 0;
  #endif

  // C++ Support
  #ifdef __cplusplus
  __libc_init_array();
  #endif

  // Init system
  SYS_init();

  // Return
  asm volatile("mret");
}
