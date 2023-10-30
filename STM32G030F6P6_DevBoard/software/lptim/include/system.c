// ===================================================================================
// Basic System Functions for STM32G030, STM32G031, and STM32G041             * v0.1 *
// ===================================================================================
//
// This file must be included!!! The system configuration and the system clock are 
// set up automatically on system start.

#include "system.h"

// ===================================================================================
// Setup Microcontroller (this function is called automatically at startup)
// ===================================================================================
void SYS_init(void) {
  // Init system clock
  #if SYS_CLK_INIT > 0
  #if   F_CPU > 48000000
  FLASH->ACR |= 0b010;                          // FLASH latency: two wait states
  #elif F_CPU > 24000000
  FLASH->ACR |= 0b001;                          // FLASH latency: one wait state
  #endif
  CLK_init();
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
  SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE;
  #endif

  // Enable GPIO
  #if SYS_GPIO_EN > 0
  RCC->IOPENR = RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN
              | RCC_IOPENR_GPIODEN | RCC_IOPENR_GPIOFEN;
  #endif
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Init internal oscillator (non PLL) as system clock source
void CLK_init_HSI(void) {
  #if CLK_DIV > 0
  RCC->CR = CLK_DIV | RCC_CR_HSION;                       // set HSI divisor
  #endif
}

// Init internal oscillator with PLL as system clock source
void CLK_init_HSI_PLL(void) {
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSI                   // set HSI as source for PLL
               | RCC_PLLCFGR_PLLREN                       // enable PLLR clock output
               | (0b0001000 << 8)                         // PLL multiplication factor = 8
               | (0b000 << 4)                             // PLL input divider = 1
               | CLK_PLL_DIV;                             // PLLR output divider
  RCC->CR |= RCC_CR_PLLON;                                // Enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));                      // Wait till PLL is ready      
  RCC->CFGR = 0b010;                                      // PLLR as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b010<<3));    // Wait for PLLR
}

// Init external crystal (non PLL) as system clock source
void CLK_init_HSE(void) {
  RCC->CR |= RCC_CR_HSEON;                                // Turn on HSE
  while(!(RCC->CR & RCC_CR_HSERDY));                      // wait till HSE is ready
  RCC->CFGR = 0b001;                                      // HSE as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b001<<3));    // Wait for HSE
}

// Init external crystal with PLL as system clock source
void CLK_init_HSE_PLL(void) {
  RCC->CR |= RCC_CR_HSEON;                                // Turn on HSE
  while(!(RCC->CR & RCC_CR_HSERDY));                      // wait till HSE is ready
  RCC->PLLCFGR = RCC_PLLCFGR_PLLSRC_HSE                   // set HSE as source for PLL
               | RCC_PLLCFGR_PLLREN                       // enable PLLR clock output
               | (0b0001000 << 8)                         // PLL multiplication factor = 8
               | (0b000 << 4)                             // PLL input divider = 1
               | CLK_PLL_DIV;                             // PLLR output divider
  RCC->CR |= RCC_CR_PLLON;                                // Enable PLL
  while(!(RCC->CR & RCC_CR_PLLRDY));                      // Wait till PLL is ready      
  RCC->CFGR = 0b010;                                      // PLLR as system clock source
  while((RCC->CFGR & RCC_CFGR_SWS_Msk) != (0b010<<3));    // Wait for PLLR
}

// Reset system clock to default state
void CLK_reset(void) {
  RCC->CR |= RCC_CR_HSION;                                // enable HSI, no divider
  while(!(RCC->CR & RCC_CR_HSIRDY));                      // wait for HSI ready
  RCC->CFGR = 0x00000000;                                 // HSI as system clock source
  while(RCC->CFGR & RCC_CFGR_SWS_Msk);                    // wait until switched to HSI
  RCC->ICSCR = (RCC->ICSCR & ~RCC_ICSCR_HSITRIM) | 0x4000;// reset ICSCR register
  RCC->CR      = 0x00000500;                              // keep only HSI on
  RCC->PLLCFGR = 0x00001000;                              // reset PLLCFGR register
  RCC->CIER    = 0x00000000;                              // disable all interrupts
  RCC->CICR    = 0xffffffff;                              // clear all interrupt flags
  FLASH->ACR  &= ~(uint32_t)0b111;                        // reset flash latency
}

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================

// Init RTC with LSI as clock source and 1s clock period
void RTC_init(void) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  RCC->APBENR1 |= RCC_APBENR1_PWREN     // enable low power control block clock
                | RCC_APBENR1_RTCAPBEN; // enable RTC module clock
  PWR->CR1     |= PWR_CR1_DBP;          // enable access to RTC domain
  RCC->BDCR     = (0b10<<8)             // set LSI as clock source
                | RCC_BDCR_RTCEN;       // enable RTC
  RTC->WPR = 0xCA;                      // remove RTC register write protection
  RTC->WPR = 0x53;
}

// Init RTC with LSE as clock source and 1s clock period
void RTC_init_LSE(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN     // enable low power control block clock
                | RCC_APBENR1_RTCAPBEN; // enable RTC module clock
  PWR->CR1     |= PWR_CR1_DBP;          // enable access to RTC domain
  RCC->BDCR     = RCC_BDCR_LSEON        // turn on LSE
                | (0b01<<8)             // set LSE as clock source
                | RCC_BDCR_RTCEN;       // enable RTC
  RTC->WPR = 0xCA;                      // remove RTC register write protection
  RTC->WPR = 0x53;
}

// Set RTC prescaler (default 127, 255)
void RTC_setPrescaler(uint8_t async, uint16_t sync) {
  RTC->ICSR |= RTC_ICSR_INIT;           // enter initialization mode
  while(!(RTC->ICSR & RTC_ICSR_INITF)); // wait until init mode is entered
  RTC->PRER = ((uint32_t)async << 16)
            | sync;
  RTC->ICSR &= ~RTC_ICSR_INIT;          // exit initialization mode
}

// Set time in BC format
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC->ICSR |= RTC_ICSR_INIT;           // enter initialization mode
  while(!(RTC->ICSR & RTC_ICSR_INITF)); // wait until init mode is entered
  RTC->TR = (uint32_t)hours   << 16     // set hours in BCD format
          | (uint32_t)minutes <<  8     // set minutes in BCD format
          | (uint32_t)seconds <<  0;    // set seconds in BCD format
  RTC->CR |= RTC_CR_FMT;                // set time format to 24 hours
  RTC->ICSR &= ~RTC_ICSR_INIT;          // exit initialization mode
}

// Set Date in BCD format
void RTC_setDate(uint8_t year, uint8_t weekday, uint8_t month, uint8_t day) {
  RTC->ICSR |= RTC_ICSR_INIT;           // enter initialization mode
  while(!(RTC->ICSR & RTC_ICSR_INITF)); // wait until init mode is entered
  RTC->DR = (uint32_t)year    << 16     // set year in BCD format
          | (uint32_t)weekday << 13     // set day of the week (1:Monday - 7:Sunday)
          | (uint32_t)month   <<  8     // set month in BCD format
          | (uint32_t)day     <<  0;    // set day in BCD format
  RTC->ICSR &= ~RTC_ICSR_INIT;          // exit initialization mode
}

// Read time register
uint32_t RTC_readTime(void) {
  while(!(RTC->ICSR & RTC_ICSR_RSF));   // wait for registers synchronized
  RTC->ICSR &= ~RTC_ICSR_RSF;           // clear synchronization flag
  return RTC->TR;
}

// Read date register
uint32_t RTC_readDate(void) {
  while(!(RTC->ICSR & RTC_ICSR_RSF));   // wait for registers synchronized
  RTC->ICSR &= ~RTC_ICSR_RSF;           // clear synchronization flag
  return RTC->DR;
}

// Set Alarm A
void RTC_setAlarmA(uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC->CR &= ~RTC_CR_ALRAE;             // disable Alarm A
  while(!(RTC->ICSR & RTC_ICSR_ALRAWF));// wait for write access
  RTC->ALRMASSR = 0;                    // no sub second comparison
  RTC->ALRMAR = (RTC->ALRMAR & 0xc0808080)  // keep alarm flags
              | (uint32_t)weekday << 24 // set day of the week (1:Monday - 7:Sunday)
              | (uint32_t)hours   << 16 // set hours in BCD format
              | (uint32_t)minutes <<  8 // set minutes in BCD format
              | (uint32_t)seconds <<  0 // set seconds in BCD format
              | RTC_ALRMAR_WDSEL;       // select week day mode
  RTC->SCR = RTC_SCR_CALRAF;            // clear alarm flag
  RTC->CR |= RTC_CR_ALRAE;              // enable Alarm A
}

// Set Alarm B
void RTC_setAlarmB(uint8_t weekday, uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC->CR &= ~RTC_CR_ALRBE;             // disable Alarm B
  while(!(RTC->ICSR & RTC_ICSR_ALRBWF));// wait for write access
  RTC->ALRMBSSR = 0;                    // no sub second comparison
  RTC->ALRMBR = (RTC->ALRMBR & 0xc0808080)  // keep alarm flags
              | (uint32_t)weekday << 24 // set day of the week (1:Monday - 7:Sunday)
              | (uint32_t)hours   << 16 // set hours in BCD format
              | (uint32_t)minutes <<  8 // set minutes in BCD format
              | (uint32_t)seconds <<  0 // set seconds in BCD format
              | RTC_ALRMBR_WDSEL;       // select week day mode
  RTC->SCR = RTC_SCR_CALRBF;            // clear alarm flag
  RTC->CR |= RTC_CR_ALRBE;              // enable Alarm B
}

// Start wakeup timer
void RTC_startWakeupTimer(uint16_t ms) {
  RTC->CR &= ~RTC_CR_WUTE;              // disable wakeup timer
  while(!(RTC->ICSR & RTC_ICSR_WUTWF)); // wait to gain access
  RTC->WUTR = ms << 1;                  // set timer (base 32kHz / 16 = 2kHz)
  RTC->SCR = RTC_SCR_CWUTF;             // clear wakup timer flag
  RTC->CR |= RTC_CR_WUTE;               // enable wakeup timer
}

// ===================================================================================
// Delay Functions
// ===================================================================================

// Wait n+1 counts of SysTick
void DLY_ticks(uint32_t n) {
  SysTick->LOAD = n;
  SysTick->VAL  = 0;
  while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
}

// ===================================================================================
// Independent Watchdog Timer (IWDG) Functions
// ===================================================================================

// Start independent watchdog timer (IWDG) with given time in milliseconds (max 8191).
// Once the IWDG has been started, it cannot be disabled, only reloaded (feed).
// It can be stopped by disabling the internal low-speed clock (LSI).
void IWDG_start(uint16_t ms) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  IWDG->KR   = 0x5555;                  // allow register modification
  while(IWDG->SR & IWDG_SR_PVU);        // wait for prescaler register to be ready
  IWDG->PR   = 0b100;                   // set LSI clock prescaler 64 (~0.5kHz)
  while(IWDG->SR & IWDG_SR_RVU);        // wait for reload register to be ready
  IWDG->RLR  = ms >> 1;                 // set watchdog counter reload value
  IWDG->KR   = 0xAAAA;                  // load reload value into watchdog counter
  IWDG->KR   = 0xCCCC;                  // enable IWDG
}

// Reload watchdog counter with n milliseconds, n<=8191
void IWDG_reload(uint16_t ms) {
  IWDG->KR   = 0x5555;                  // allow register modification
  while(IWDG->SR & IWDG_SR_RVU);        // wait for reload register to be ready
  IWDG->RLR = ms >> 1;                  // set watchdog counter reload value
  IWDG->KR   = 0xAAAA;                  // load reload value into watchdog counter
}

// ===================================================================================
// Low Power Timer (LPT) Functions
// ===================================================================================

// Init and enable low-power timer
void LPT_init(void) {
  LSI_enable();                         // enable internal low-speed clock (LSI)
  RCC->APBENR1 |= RCC_APBENR1_LPTIM1EN; // enable LPT module clock
  RCC->CCIPR   |= (uint32_t)0b01<<18;   // set LSI as LPT clock source
  LPTIM1->CFGR  = (uint32_t)0b101<<9;   // set prescaler to 32 (~1kHz)
  LPTIM1->CR    = LPTIM_CR_ENABLE;      // enable timer
}

// Start low-power timer single shot with period in ms
void LPT_shot(uint16_t ms) {
  DLY_ms(2);                            // wait two LPT clock cycles
  LPTIM1->ARR = ms;                     // set interval
  LPTIM1->CR |= LPTIM_CR_SNGSTRT;       // start timer in single mode
}

// Start low-power timer in continuous mode with period in ms
void LPT_start(uint16_t ms) {
  DLY_ms(2);                            // wait two LPT clock cycles
  LPTIM1->ARR = ms;                     // set interval
  LPTIM1->CR |= LPTIM_CR_CNTSTRT;       // start timer in single mode
}

// Put device in to SLEEP and wake-up after LPT period in ms
void LPT_sleep(uint16_t ms) {
  LPT_shot(ms);                        // start low-power timer (LPT) single shot
  LPTIM1->IER  =  LPTIM_IER_ARRMIE;    // enable auto reload interrupt
  EXTI->EMR1  |=  (uint32_t)1 << 29;   // enable wake up with event
  SLEEP_WFE_now();                     // put device into sleep, wake up by LPT event
  LPTIM1->ICR  =  LPTIM_ICR_ARRMCF;    // clear auto reload interrupt flag
  LPTIM1->IER &= ~LPTIM_IER_ARRMIE;    // disable auto reload interrupt
  EXTI->EMR1  &= ~((uint32_t)1 << 29); // disable wake up with event
}

// ===================================================================================
// Sleep Functions
// ===================================================================================

// Put device into SLEEP, wake up by interrupt
void SLEEP_WFI_now(void) {
  __WFI();                              // wait for interrupt
}

// Put device into SLEEP, wake up by event
void SLEEP_WFE_now(void) {
  __WFE();                              // wait for event
}

// Put device into STOP 0, wake up interrupt
void STOP0_WFI_now(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->RPR1 = 0xffffffff;              // clear all EXTI rising edge pending flags
  EXTI->FPR1 = 0xffffffff;              // clear all EXTI falling edge pending flags
  PWR->CR1 &= ~PWR_CR1_LPMS;            // set low-power mode to stop 0
  __WFI();                              // wait for interrupt
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
}

// Put device into STOP 0, wake up event
void STOP0_WFE_now(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->RPR1 = 0xffffffff;              // clear all EXTI rising edge pending flags
  EXTI->FPR1 = 0xffffffff;              // clear all EXTI falling edge pending flags
  PWR->CR1 &= ~PWR_CR1_LPMS;            // set low-power mode to stop 0
  __WFE();                              // wait for event
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
}

// Put device into STOP 1, wake up interrupt
void STOP1_WFI_now(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->RPR1 = 0xffffffff;              // clear all EXTI rising edge pending flags
  EXTI->FPR1 = 0xffffffff;              // clear all EXTI falling edge pending flags
  PWR->CR1 = (PWR->CR1 & ~PWR_CR1_LPMS)
           | (0b001<<PWR_CR1_LPMS_Pos); // set low-power mode to stop 1
  __WFI();                              // wait for interrupt
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
}

// Put device into STOP 1, wake up event
void STOP1_WFE_now(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->RPR1 = 0xffffffff;              // clear all EXTI rising edge pending flags
  EXTI->FPR1 = 0xffffffff;              // clear all EXTI falling edge pending flags
  PWR->CR1 = (PWR->CR1 & ~PWR_CR1_LPMS)
           | (0b001<<PWR_CR1_LPMS_Pos); // set low-power mode to stop 1
  __WFE();                              // wait for event
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
}

// Put device into STANDBY, wake up interrupt
void STDBY_WFI_now(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  PWR->CR1 = (PWR->CR1 & ~PWR_CR1_LPMS)
           | (0b011<<PWR_CR1_LPMS_Pos); // set low-power mode to standby
  PWR->SCR = 0b111111;                  // clear WUFx bits
  RTC->SCR = 0b111111;                  // clear RTC interrupt flags
  __WFI();                              // wait for interrupt
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
}

// Put device into STANDBY, wake up event
void STDBY_WFE_now(void) {
  RCC->APBENR1 |= RCC_APBENR1_PWREN;    // enable low power control block clock
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;    // set deep sleep mode
  NVIC->ICPR[0] = 0xffffffff;           // clear pending interrupts
  EXTI->RPR1 = 0xffffffff;              // clear all EXTI rising edge pending flags
  EXTI->FPR1 = 0xffffffff;              // clear all EXTI falling edge pending flags
  PWR->CR1 = (PWR->CR1 & ~PWR_CR1_LPMS)
           | (0b011<<PWR_CR1_LPMS_Pos); // set low-power mode to standby
  PWR->SCR = 0b111111;                  // clear WUFx bits
  RTC->SCR = 0b111111;                  // clear RTC interrupt flags
  __WFE();                              // wait for event
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;   // unset deep sleep mode
}

// ===================================================================================
// Bootloader Functions
// ===================================================================================
void BOOT_now(void) {
  void (*SysMemBootJump)(void);
  __disable_irq();                      // disable all interrupts
  SysTick->CTRL = 0;                    // disable SysTick Timer
  CLK_reset();                          // reset system clock settings
  NVIC->ICER[0]=0xffffffff;             // clear interrupt enable register
  NVIC->ICPR[0]=0xffffffff;             // clear interrupt pending register
  __enable_irq();                       // re-enable all interrupts
  SysMemBootJump = (void (*)(void)) (*((uint32_t *) ((BOOT_ADDR + 4))));
  __set_MSP(*(uint32_t *)BOOT_ADDR);    // set main stack pointer for bootloader
  SysMemBootJump();                     // jump to bootloader
  while(1);                             // just be sure
}

// ===================================================================================
// True Random Number Generator (RNG) Functions
// ===================================================================================

// Init true random number generator (RNG)
void RNG_init(void) {
  RCC->AHBENR |= RCC_AHBENR_RNGEN;      // enable RNG clock
  RCC->CCIPR   = (RCC->CCIPR & ~(uint32_t)0b1111 << 26)
               | (uint32_t)0b10 << 26   // set SYSCLK as RNG clock
               | (uint32_t)0b00 << 28;  // set clock divider to 1
  RNG->CR  =  RNG_CR_RNGEN;             // enable RNG
}

// Read true random word
uint32_t RNG_read(void) {
  //while(!(RNG->SR & RNG_SR_DRDY));      // wait for data ready
  return RNG->DR;                       // read and return random value
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
// C version of STM32G041.s Startup File
// ===================================================================================
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t _data_lma;
extern uint32_t _data_vma;
extern uint32_t _edata;
extern void _estack(void);

// Prototypes
int main(void)                __attribute__((section(".text.main"), used));
void (*const vectors[])(void) __attribute__((section(".vectors"), used));
void Reset_Handler(void)      __attribute__((section(".text.irq_handler"), naked, used));

#if SYS_USE_VECTORS > 0
// Unless a specific handler is overridden, it just spins forever
void Default_Handler(void)    __attribute__((section(".text.irq_handler"), naked, used));
void Default_Handler(void)    { while(1); }

// All interrupt handlers are aliased to default_handler unless overridden individually
#define DUMMY_HANDLER __attribute__((section(".text.irq_handler"), weak, alias("Default_Handler"), used))
DUMMY_HANDLER void NMI_Handler(void);
DUMMY_HANDLER void HardFault_Handler(void);
DUMMY_HANDLER void SVC_Handler(void);
DUMMY_HANDLER void PendSV_Handler(void);
DUMMY_HANDLER void SysTick_Handler(void);
DUMMY_HANDLER void WWDG_IRQHandler(void);
DUMMY_HANDLER void PVD_IRQHandler(void);
DUMMY_HANDLER void RTC_TAMP_IRQHandler(void);
DUMMY_HANDLER void FLASH_IRQHandler(void);
DUMMY_HANDLER void RCC_IRQHandler(void);
DUMMY_HANDLER void EXTI0_1_IRQHandler(void);
DUMMY_HANDLER void EXTI2_3_IRQHandler(void);
DUMMY_HANDLER void EXTI4_15_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel1_IRQHandler(void);
DUMMY_HANDLER void DMA1_Channel2_3_IRQHandler(void);
DUMMY_HANDLER void DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler(void);
DUMMY_HANDLER void ADC1_IRQHandler(void);
DUMMY_HANDLER void TIM1_BRK_UP_TRG_COM_IRQHandler(void);
DUMMY_HANDLER void TIM1_CC_IRQHandler(void);
DUMMY_HANDLER void TIM2_IRQHandler(void);
DUMMY_HANDLER void TIM3_IRQHandler(void);
DUMMY_HANDLER void LPTIM1_IRQHandler(void);
DUMMY_HANDLER void LPTIM2_IRQHandler(void);
DUMMY_HANDLER void TIM14_IRQHandler(void);
DUMMY_HANDLER void TIM16_IRQHandler(void);
DUMMY_HANDLER void TIM17_IRQHandler(void);
DUMMY_HANDLER void I2C1_IRQHandler(void);
DUMMY_HANDLER void I2C2_IRQHandler(void);
DUMMY_HANDLER void SPI1_IRQHandler(void);
DUMMY_HANDLER void SPI2_IRQHandler(void);
DUMMY_HANDLER void USART1_IRQHandler(void);
DUMMY_HANDLER void USART2_IRQHandler(void);
DUMMY_HANDLER void LPUART1_IRQHandler(void);
DUMMY_HANDLER void AES_RNG_IRQHandler(void);
#endif  // SYS_USE_VECTORS > 0

// Interrupt vector table
void (*const vectors[])(void) = {
  &_estack,                       //  0 - Initial Stack Pointer Value

  // Cortex-M0+ handlers
  Reset_Handler,                  //  1 - Reset
  #if SYS_USE_VECTORS > 0
  NMI_Handler,                    //  2 - NMI
  HardFault_Handler,              //  3 - Hard Fault
  0,                              //  4 - Reserved
  0,                              //  5 - Reserved
  0,                              //  6 - Reserved
  0,                              //  7 - Reserved
  0,                              //  8 - Reserved
  0,                              //  9 - Reserved
  0,                              // 10 - Reserved
  SVC_Handler,                    // 11 - SVCall
  0,                              // 12 - Reserved
  0,                              // 13 - Reserved
  PendSV_Handler,                 // 14 - PendSV
  SysTick_Handler,                // 15 - SysTick

  // Peripheral handlers
  WWDG_IRQHandler,                //  0 - Window Watchdog
  PVD_IRQHandler,                 //  1 - PVD through EXTI Line detect
  RTC_TAMP_IRQHandler,            //  2 - RTC through EXTI Line
  FLASH_IRQHandler,               //  3 - FLASH
  RCC_IRQHandler,                 //  4 - RCC
  EXTI0_1_IRQHandler,             //  5 - EXTI Line 0 and 1
  EXTI2_3_IRQHandler,             //  6 - EXTI Line 2 and 3
  EXTI4_15_IRQHandler,            //  7 - EXTI Line 4 to 15
  0,                              //  8 - Reserved
  DMA1_Channel1_IRQHandler,       //  9 - DMA1 Channel 1
  DMA1_Channel2_3_IRQHandler,     // 10 - DMA1 Channel 2 and 3
  DMA1_Ch4_5_DMAMUX1_OVR_IRQHandler,  // 11 - DMA1 Channel 4 to Channel 5, DMAMUX1 overrun
  ADC1_IRQHandler,                // 12 - ADC1
  TIM1_BRK_UP_TRG_COM_IRQHandler, // 13 - TIM1 Break, Update, Trigger and Commutation
  TIM1_CC_IRQHandler,             // 14 - TIM1 Capture Compare
  TIM2_IRQHandler,                // 15 - TIM2
  TIM3_IRQHandler,                // 16 - TIM3
  LPTIM1_IRQHandler,              // 17 - LPTIM1
  LPTIM2_IRQHandler,              // 18 - LPTIM2
  TIM14_IRQHandler,               // 19 - TIM14
  0,                              // 20 - Reserved
  TIM16_IRQHandler,               // 21 - TIM16
  TIM17_IRQHandler,               // 22 - TIM17
  I2C1_IRQHandler,                // 23 - I2C1
  I2C2_IRQHandler,                // 24 - I2C2
  SPI1_IRQHandler,                // 25 - SPI1
  SPI2_IRQHandler,                // 26 - SPI2
  USART1_IRQHandler,              // 27 - USART1
  USART2_IRQHandler,              // 28 - USART2
  LPUART1_IRQHandler,             // 29 - LPUART1
  0,                              // 30 - Reserved
  AES_RNG_IRQHandler,             // 31 - AES and RNG
  #endif  // SYS_USE_VECTORS > 0
};

// Reset handler
void Reset_Handler(void) {
  uint32_t *src, *dst;

  // Set stack pointer
  asm volatile(
  " ldr r0, =_estack  \n\
    mov sp, r0        \n"
    : : : "r0", "memory"
  );

  // Configure vector table location
  SCB->VTOR = (uint32_t)vectors;

  // Copy data from FLASH to RAM
  src = &_data_lma;
  dst = &_data_vma;
  while(dst < &_edata) *dst++ = *src++;

  // Clear uninitialized variables
  #if SYS_CLEAR_BSS > 0
  dst = &_sbss;
  while(dst < &_ebss) *dst++ = 0;
  #endif

  // Init system
  SYS_init();

  // C++ Support
  #ifdef __cplusplus
  __libc_init_array();
  #endif

  // Start main function
  main();
  while(1);
}
