// ===================================================================================
// Basic System Functions for ATSAMD21E, ATSAMD21G, ATSAMD21J                 * v1.0 *
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
  CLK_init();
  #endif

  // Init SYSTICK
  #if SYS_TICK_INIT > 0
  SysTick->CTRL = SysTick_CTRL_ENABLE | SysTick_CTRL_CLKSOURCE;
  #endif

  // Enable GPIO
  #if SYS_GPIO_EN > 0
  PM->APBBMASK.reg |= PM_APBBMASK_PORT;
  #endif
}

// ===================================================================================
// Generic Clock (GCLK) Functions
// ===================================================================================

// Set generic clock generator (gen) to source (src) with divider (div) and flags
static void GCLK_setGENx(uint8_t gen, uint8_t src, uint16_t div, uint32_t flags) {
  GCLK->GENDIV.reg  = GCLK_GENDIV_ID(gen) | GCLK_GENDIV_DIV(div);
  GCLK_sync();
  GCLK->GENCTRL.reg = GCLK_GENCTRL_ID(gen) | GCLK_GENCTRL_SRC(src)
                    | GCLK_GENCTRL_GENEN   | GCLK_GENCTRL_IDC | flags;
  GCLK_sync();
}

// Set generic clock generator (gen: 0..8) to source (src) with divider (div)
void GCLK_setGEN(uint8_t gen, uint8_t src, uint16_t div) {
  GCLK_setGENx(gen, src, div, 0);
}

// Connect generic clock channel (ch) to generator (gen)
void GCLK_setCH(uint8_t ch, uint8_t gen) {
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(ch) | GCLK_CLKCTRL_GEN(gen) | GCLK_CLKCTRL_CLKEN;
  GCLK_sync();
}

// Disconnect generic clock channel (ch)
void GCLK_stopCH(uint8_t ch) {
  GCLK->CLKCTRL.reg = GCLK_CLKCTRL_ID(ch);
  GCLK_sync();
}

// Read-modify-write generator control register (for enabling/disabling output)
static void GCLK_modGEN(uint8_t gen, uint32_t clr, uint32_t set) {
  uint32_t ctrl;
  *((__IO uint8_t*)&GCLK->GENCTRL.reg) = gen;   // select generator for reading
  GCLK_sync();
  ctrl = GCLK->GENCTRL.reg;
  GCLK->GENCTRL.reg = (ctrl & ~clr) | set;
  GCLK_sync();
}

// Enable output of system clock (GCLK0) on MCO_PIN (GCLK_IO0, PMUX function H)
void MCO_setSYS(void) {
  PortGroup* port = &PORT->Group[MCO_PIN >> 5];
  GCLK_modGEN(GCLK_SYS, 0, GCLK_GENCTRL_OE);
  if(MCO_PIN & 1)
    port->PMUX[(MCO_PIN & 31) >> 1].reg = (port->PMUX[(MCO_PIN & 31) >> 1].reg
                                        & ~PORT_PMUX_PMUXO_Msk) | PORT_PMUX_PMUXO(7);
  else
    port->PMUX[(MCO_PIN & 31) >> 1].reg = (port->PMUX[(MCO_PIN & 31) >> 1].reg
                                        & ~PORT_PMUX_PMUXE_Msk) | PORT_PMUX_PMUXE(7);
  port->PINCFG[MCO_PIN & 31].reg |= PORT_PINCFG_PMUXEN;
}

// Disable clock output
void MCO_stop(void) {
  PORT->Group[MCO_PIN >> 5].PINCFG[MCO_PIN & 31].reg &= ~PORT_PINCFG_PMUXEN;
  GCLK_modGEN(GCLK_SYS, GCLK_GENCTRL_OE, 0);
}

// ===================================================================================
// System Clock Functions
// ===================================================================================

// Enable internal 32.768kHz RC oscillator (OSC32K) with factory calibration
void OSC32K_enable(void) {
  uint32_t cal = ((*(uint32_t*)FUSES_OSC32K_CAL_ADDR) & FUSES_OSC32K_CAL_Msk) >> FUSES_OSC32K_CAL_Pos;
  SYSCTRL->OSC32K.reg = SYSCTRL_OSC32K_CALIB(cal) | SYSCTRL_OSC32K_STARTUP(2)
                      | SYSCTRL_OSC32K_EN32K | SYSCTRL_OSC32K_RUNSTDBY;
  SYSCTRL->OSC32K.reg |= SYSCTRL_OSC32K_ENABLE;
  while(!OSC32K_ready());
}

// Enable external 32.768kHz crystal oscillator (XOSC32K) on PA00/PA01
void XOSC32K_enable(void) {
  SYSCTRL->XOSC32K.reg = SYSCTRL_XOSC32K_STARTUP(6) | SYSCTRL_XOSC32K_XTALEN
                       | SYSCTRL_XOSC32K_EN32K      | SYSCTRL_XOSC32K_RUNSTDBY;
  SYSCTRL->XOSC32K.reg |= SYSCTRL_XOSC32K_ENABLE; // must be written separately
  while(!XOSC32K_ready());
}

// Set flash wait states according to system clock (VDD >= 2.7V)
static void CLK_setWaitStates(uint32_t freq) {
  NVMCTRL->CTRLB.reg = (NVMCTRL->CTRLB.reg & ~NVMCTRL_CTRLB_RWS_Msk)
                     | NVMCTRL_CTRLB_RWS(freq > 24000000 ? 1 : 0);
}

// Init DFLL48M (divided) as system clock source
void CLK_init_DFLL(void) {
  #ifdef CLK_DIV
  uint32_t coarse;

  // Set flash wait states for maximum frequency
  CLK_setWaitStates(DFLL48M_VALUE);

  // Errata: write DFLLCTRL with ONDEMAND = 0 before configuring the DFLL
  SYSCTRL->DFLLCTRL.reg = 0;
  while(!DFLL_ready());

  // Load factory coarse calibration value, fine value: mid-range
  coarse = ((*(uint32_t*)FUSES_DFLL48M_COARSE_CAL_ADDR) & FUSES_DFLL48M_COARSE_CAL_Msk)
         >> FUSES_DFLL48M_COARSE_CAL_Pos;
  if(coarse == 0x3f) coarse = 0x1f;             // not calibrated -> use mid-range
  SYSCTRL->DFLLVAL.reg = SYSCTRL_DFLLVAL_COARSE(coarse) | SYSCTRL_DFLLVAL_FINE(0x1ff);
  while(!DFLL_ready());

  #if SYS_USE_XOSC32K > 0
  // Closed loop mode with external 32.768kHz crystal as reference
  XOSC32K_enable();
  GCLK_setGEN(GCLK_REF, GCLK_SOURCE_XOSC32K, 1);
  GCLK_setCH(SYSCTRL_GCLK_ID_DFLL48, GCLK_REF);
  SYSCTRL->DFLLMUL.reg  = SYSCTRL_DFLLMUL_CSTEP(7) | SYSCTRL_DFLLMUL_FSTEP(63)
                        | SYSCTRL_DFLLMUL_MUL((DFLL48M_VALUE + XOSC32K_VALUE / 2) / XOSC32K_VALUE);
  while(!DFLL_ready());
  SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_MODE | SYSCTRL_DFLLCTRL_WAITLOCK
                        | SYSCTRL_DFLLCTRL_ENABLE;
  while(!DFLL_locked());                        // wait for coarse and fine lock
  #elif SYS_USB_CRM > 0
  // USB clock recovery mode (closed loop with USB start-of-frame as reference)
  SYSCTRL->DFLLMUL.reg  = SYSCTRL_DFLLMUL_CSTEP(7) | SYSCTRL_DFLLMUL_FSTEP(63)
                        | SYSCTRL_DFLLMUL_MUL(DFLL48M_VALUE / 1000);
  while(!DFLL_ready());
  SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_MODE | SYSCTRL_DFLLCTRL_USBCRM
                        | SYSCTRL_DFLLCTRL_CCDIS | SYSCTRL_DFLLCTRL_ENABLE;
  #else
  // Open loop mode with factory calibration
  SYSCTRL->DFLLCTRL.reg = SYSCTRL_DFLLCTRL_ENABLE;
  #endif
  while(!DFLL_ready());

  // Set DFLL48M (divided) as main clock (GCLK0)
  GCLK_setGEN(GCLK_SYS, GCLK_SOURCE_DFLL48M, CLK_DIV);
  CLK_setWaitStates(F_CPU);
  #endif
}

// Init internal 8MHz oscillator (divided) as system clock source
void CLK_init_OSC8M(void) {
  #ifdef CLK_PRESC
  SYSCTRL->OSC8M.reg = (SYSCTRL->OSC8M.reg & ~SYSCTRL_OSC8M_PRESC_Msk)
                     | SYSCTRL_OSC8M_PRESC(CLK_PRESC) | SYSCTRL_OSC8M_ENABLE;
  while(!OSC8M_ready());
  GCLK_setGEN(GCLK_SYS, GCLK_SOURCE_OSC8M, 1);
  CLK_setWaitStates(F_CPU);
  #endif
}

// Reset system clock to default state (OSC8M / 8 = 1MHz)
void CLK_reset(void) {
  SYSCTRL->OSC8M.reg = (SYSCTRL->OSC8M.reg & ~SYSCTRL_OSC8M_PRESC_Msk)
                     | SYSCTRL_OSC8M_PRESC(3) | SYSCTRL_OSC8M_ENABLE;
  while(!OSC8M_ready());
  GCLK_setGEN(GCLK_SYS, GCLK_SOURCE_OSC8M, 1);
  SYSCTRL->DFLLCTRL.reg = 0;                    // disable DFLL48M
  CLK_setWaitStates(0);
}

// ===================================================================================
// Real-Time Clock (RTC) Functions
// ===================================================================================

// Setup RTC clock: generator GCLK_RTC = source / 32 = 1024Hz, also in standby
static void RTC_setClock(uint8_t src) {
  PM->APBAMASK.reg |= PM_APBAMASK_RTC;          // enable RTC module clock
  GCLK_setGENx(GCLK_RTC, src, 4, GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_RUNSTDBY);
  GCLK_setCH(RTC_GCLK_ID, GCLK_RTC);            // GCLK_RTC -> RTC
  RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_SWRST;   // reset RTC
  while(RTC->MODE0.CTRL.reg & RTC_MODE0_CTRL_SWRST);
}

// Init RTC in calendar mode with 1s clock period
static void RTC_initCalendar(uint8_t src) {
  RTC_setClock(src);
  RTC->MODE2.CTRL.reg = RTC_MODE2_CTRL_MODE_CLOCK         // calendar mode, 24h
                      | RTC_MODE2_CTRL_PRESCALER_DIV1024; // 1024Hz / 1024 = 1Hz
  RTC_sync();
  RTC->MODE2.READREQ.reg = RTC_READREQ_RREQ | RTC_READREQ_RCONT
                         | RTC_READREQ_ADDR(0x10);        // continuous read sync
  RTC->MODE2.CTRL.reg |= RTC_MODE2_CTRL_ENABLE;           // enable RTC
  RTC_sync();
}

// Init RTC with internal 32.768kHz oscillator (OSC32K)
void RTC_init(void) {
  OSC32K_enable();
  RTC_initCalendar(GCLK_SOURCE_OSC32K);
}

// Init RTC with external 32.768kHz crystal (XOSC32K)
void RTC_init_XOSC32K(void) {
  XOSC32K_enable();
  RTC_initCalendar(GCLK_SOURCE_XOSC32K);
}

// Read clock register
uint32_t RTC_read(void) {
  RTC_sync();
  return RTC->MODE2.CLOCK.reg;
}

// Set time (hours, minutes, seconds), decimal values
void RTC_setTime(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  uint32_t clk = RTC_read() & ~(RTC_MODE2_CLOCK_HOUR_Msk | RTC_MODE2_CLOCK_MINUTE_Msk
                                                        | RTC_MODE2_CLOCK_SECOND_Msk);
  RTC->MODE2.CLOCK.reg = clk | RTC_MODE2_CLOCK_HOUR(hours) | RTC_MODE2_CLOCK_MINUTE(minutes)
                             | RTC_MODE2_CLOCK_SECOND(seconds);
  RTC_sync();
}

// Set date (year since RTC_YEAR_REF, month, day), decimal values
void RTC_setDate(uint8_t year, uint8_t month, uint8_t day) {
  uint32_t clk = RTC_read() & ~(RTC_MODE2_CLOCK_YEAR_Msk | RTC_MODE2_CLOCK_MONTH_Msk
                                                        | RTC_MODE2_CLOCK_DAY_Msk);
  RTC->MODE2.CLOCK.reg = clk | RTC_MODE2_CLOCK_YEAR(year) | RTC_MODE2_CLOCK_MONTH(month)
                             | RTC_MODE2_CLOCK_DAY(day);
  RTC_sync();
}

// Set daily alarm (hours, minutes, seconds)
void RTC_setAlarm(uint8_t hours, uint8_t minutes, uint8_t seconds) {
  RTC->MODE2.Mode2Alarm[0].ALARM.reg = RTC_MODE2_ALARM_HOUR(hours)
                                     | RTC_MODE2_ALARM_MINUTE(minutes)
                                     | RTC_MODE2_ALARM_SECOND(seconds);
  RTC_sync();
  RTC->MODE2.Mode2Alarm[0].MASK.reg  = RTC_MODE2_MASK_SEL_HHMMSS;
  RTC_sync();
  RTC_clearAlarmFlag();
}

// ===================================================================================
// Low-Power Timer (LPT) Functions (RTC in 32-bit counter mode)
// ===================================================================================

// Init and enable low-power timer (1024Hz from OSCULP32K, also in standby)
void LPT_init(void) {
  RTC_setClock(GCLK_SOURCE_OSCULP32K);
  RTC->MODE0.CTRL.reg = RTC_MODE0_CTRL_MODE_COUNT32       // 32-bit counter mode
                      | RTC_MODE0_CTRL_MATCHCLR           // clear on compare match
                      | RTC_MODE0_CTRL_PRESCALER_DIV1;    // 1024Hz
  RTC_sync();
  RTC->MODE0.COMP[0].reg = 0xffffffff;
  RTC_sync();
  RTC->MODE0.CTRL.reg |= RTC_MODE0_CTRL_ENABLE;           // enable timer
  RTC_sync();
}

// Start low-power timer in continuous mode with period in ms
void LPT_start(uint32_t ms) {
  uint32_t ticks = ms + (ms * 3) / 125;                   // 1024 ticks per 1000ms
  RTC->MODE0.COMP[0].reg = ticks ? ticks - 1 : 0;         // set period
  RTC_sync();
  RTC->MODE0.COUNT.reg   = 0;                             // restart counter
  RTC_sync();
}

// Restart low-power timer with period in ms (flag is set after the period)
void LPT_shot(uint32_t ms) {
  LPT_start(ms);
  LPT_clearReloadFlag();
}

// Put device into STANDBY and wake-up after LPT period in ms
void LPT_sleep(uint32_t ms) {
  LPT_shot(ms);                                 // start low-power timer (LPT)
  RTC->MODE0.INTENSET.reg = RTC_MODE0_INTENSET_CMP0;  // enable interrupt (for wake-up)
  NVIC_ClearPendingIRQ(RTC_IRQn);               // clear pending RTC interrupt
  SCB->SCR |= SCB_SCR_SEVONPEND_Msk;            // pending interrupt generates event
  while(!LPT_readReloadFlag()) STDBY_WFE_now(); // put device into standby, wait for LPT
  SCB->SCR &= ~SCB_SCR_SEVONPEND_Msk;
  RTC->MODE0.INTENCLR.reg = RTC_MODE0_INTENCLR_CMP0;  // disable interrupt
  LPT_clearReloadFlag();                        // clear compare match flag
  NVIC_ClearPendingIRQ(RTC_IRQn);               // clear pending RTC interrupt
}

// ===================================================================================
// Delay Functions
// ===================================================================================

// Wait n+1 counts of SysTick
void DLY_ticks(uint32_t n) {
  while(n > 0xFFFFFF) {                 // SysTick has only 24 bits
    SysTick->LOAD = 0xFFFFFF;
    SysTick->VAL  = 0;
    while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
    n -= 0x1000000;
  }
  SysTick->LOAD = n;
  SysTick->VAL  = 0;
  while(!(SysTick->CTRL & SysTick_CTRL_COUNTFLAG));
}

// ===================================================================================
// Watchdog Timer (IWDG/WDT) Functions
// ===================================================================================
#define WDT_sync()  while(WDT->STATUS.reg & WDT_STATUS_SYNCBUSY)

// Reload watchdog counter with n milliseconds (period rounded up to 8..16384 cycles)
void IWDG_reload(uint16_t ms) {
  uint32_t ticks = ms + (ms * 3) / 125;         // 1024 ticks per 1000ms
  uint8_t  per   = 0;
  while((per < 11) && (((uint32_t)8 << per) < ticks)) per++;
  WDT->CTRL.reg   = 0;                          // disable WDT (to change period)
  WDT_sync();
  WDT->CONFIG.reg = WDT_CONFIG_PER(per);        // set period
  WDT_sync();
  WDT->CTRL.reg   = WDT_CTRL_ENABLE;            // enable WDT
  WDT_sync();
  IWDG_feed();                                  // restart counter
}

// Start watchdog timer with given time in milliseconds (max 16000).
// The WDT can be stopped unless the ALWAYSON bit is set (NVM user row).
void IWDG_start(uint16_t ms) {
  PM->APBAMASK.reg |= PM_APBAMASK_WDT;          // enable WDT module clock
  GCLK_setGENx(GCLK_WDT, GCLK_SOURCE_OSCULP32K, 4, GCLK_GENCTRL_DIVSEL | GCLK_GENCTRL_RUNSTDBY);
  GCLK_setCH(WDT_GCLK_ID, GCLK_WDT);            // 32768Hz / 32 = 1024Hz -> WDT
  IWDG_reload(ms);                              // set period and start WDT
}

// Stop watchdog timer
void IWDG_stop(void) {
  WDT->CTRL.reg = 0;                            // disable WDT
  WDT_sync();
}

// ===================================================================================
// Sleep Functions
// ===================================================================================

// Put device into IDLE 0 (CPU clock stopped), wake up by interrupt
void SLEEP_WFI_now(void) {
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;           // no deep sleep
  PM->SLEEP.reg = PM_SLEEP_IDLE(0);             // IDLE 0
  __DSB();
  __WFI();                                      // wait for interrupt
}

// Put device into IDLE 0 (CPU clock stopped), wake up by event
void SLEEP_WFE_now(void) {
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;           // no deep sleep
  PM->SLEEP.reg = PM_SLEEP_IDLE(0);             // IDLE 0
  __DSB();
  __WFE();                                      // wait for event
}

// Keep NVM powered in sleep modes (recommended workaround for wake-up issues)
#define SLEEP_NVM_on()  (NVMCTRL->CTRLB.reg = (NVMCTRL->CTRLB.reg & ~NVMCTRL_CTRLB_SLEEPPRM_Msk) \
                                            | NVMCTRL_CTRLB_SLEEPPRM_DISABLED)

// Put device into IDLE 2 (CPU, AHB and APB clocks stopped), wake up by interrupt
void IDLE2_WFI_now(void) {
  SLEEP_NVM_on();
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;           // no deep sleep
  PM->SLEEP.reg = PM_SLEEP_IDLE(2);             // IDLE 2
  __DSB();
  __WFI();                                      // wait for interrupt
}

// Put device into IDLE 2 (CPU, AHB and APB clocks stopped), wake up by event
void IDLE2_WFE_now(void) {
  SLEEP_NVM_on();
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;           // no deep sleep
  PM->SLEEP.reg = PM_SLEEP_IDLE(2);             // IDLE 2
  __DSB();
  __WFE();                                      // wait for event
}

// Put device into STANDBY, wake up by interrupt
void STDBY_WFI_now(void) {
  SLEEP_NVM_on();
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;            // set deep sleep mode
  __DSB();
  __WFI();                                      // wait for interrupt
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;           // unset deep sleep mode
}

// Put device into STANDBY, wake up by event
void STDBY_WFE_now(void) {
  SLEEP_NVM_on();
  SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;            // set deep sleep mode
  __DSB();
  __WFE();                                      // wait for event
  SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;           // unset deep sleep mode
}

// ===================================================================================
// Bootloader Functions
// ===================================================================================

// Jump to bootloader (SAM-BA/UF2 bootloader with double-tap support, e.g. Arduino)
void BOOT_now(void) {
  *(__IO uint32_t*)BOOT_MAGIC_ADDR = BOOT_MAGIC_VALUE;  // set double-tap magic value
  NVIC_SystemReset();                                   // conduct reset
}

// ===================================================================================
// Cyclic Redundancy Check (CRC) Functions
// ===================================================================================

// Calculate CRC-32 (IEEE 802.3) of len bytes (word aligned) at address addr using the
// device service unit (DSU). Start value: init. The result is returned without final
// XOR (standard CRC-32 = ~CRC_calc(addr, len, 0xffffffff), see CRC_calc32()).
uint32_t CRC_calc(const void* addr, uint32_t len, uint32_t init) {
  uint32_t result;
  PAC1->WPCLR.reg   = (uint32_t)1 << (ID_DSU & 31);   // remove DSU write protection
  DSU->STATUSA.reg  = DSU_STATUSA_DONE | DSU_STATUSA_BERR;
  DSU->ADDR.reg     = (uint32_t)addr & ~(uint32_t)3;  // start address (word aligned)
  DSU->LENGTH.reg   = len & ~(uint32_t)3;             // length in bytes (word aligned)
  DSU->DATA.reg     = init;                           // start value
  DSU->CTRL.reg     = DSU_CTRL_CRC;                   // start calculation
  while(!(DSU->STATUSA.reg & DSU_STATUSA_DONE));      // wait until finished
  result = DSU->DATA.reg;                             // read result
  PAC1->WPSET.reg   = (uint32_t)1 << (ID_DSU & 31);   // restore write protection
  return result;
}

// ===================================================================================
// Event System (EVSYS) Functions
// ===================================================================================

// Trigger software event on channel ch
void EVSYS_trigger(uint8_t ch) {
  uint32_t cfg;
  *((__IO uint8_t*)&EVSYS->CHANNEL.reg) = ch;         // select channel for reading
  cfg = EVSYS->CHANNEL.reg & ~EVSYS_CHANNEL_CHANNEL_Msk;
  EVSYS->CHANNEL.reg = cfg | EVSYS_CHANNEL_CHANNEL(ch) | EVSYS_CHANNEL_SWEVT;
}

// ===================================================================================
// Direct Memory Access Controller (DMA) Functions
// ===================================================================================
DmacDescriptor DMA_descriptor[DMAC_CH_NUM] __attribute__((aligned(16)));
DmacDescriptor DMA_writeback[DMAC_CH_NUM]  __attribute__((aligned(16)));

// Init DMAC (only once, can be called by several libraries)
void DMA_init(void) {
  if(DMAC->CTRL.reg & DMAC_CTRL_DMAENABLE) return;    // already initialized
  PM->AHBMASK.reg  |= PM_AHBMASK_DMAC;                // enable DMAC clocks
  PM->APBBMASK.reg |= PM_APBBMASK_DMAC;
  DMAC->CTRL.reg    = DMAC_CTRL_SWRST;                // reset DMAC
  while(DMAC->CTRL.reg & DMAC_CTRL_SWRST);
  DMAC->BASEADDR.reg = (uint32_t)DMA_descriptor;      // set descriptor memory
  DMAC->WRBADDR.reg  = (uint32_t)DMA_writeback;       // set write-back memory
  DMAC->CTRL.reg    = DMAC_CTRL_DMAENABLE | DMAC_CTRL_LVLEN(0xf);
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
// C version of the SAMD21 Startup File
// ===================================================================================
extern uint32_t _sbss, _ebss, _sdata, _edata, _sidata;
extern void _estack(void);

// Prototypes
int main(void)                __attribute__((section(".text.main"), used));
void (*const vectors[])(void) __attribute__((section(".isr_vector"), used));
void Reset_Handler(void)      __attribute__((section(".text.irq_handler"), used, noreturn));

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
DUMMY_HANDLER void PM_IRQHandler(void);
DUMMY_HANDLER void SYSCTRL_IRQHandler(void);
DUMMY_HANDLER void WDT_IRQHandler(void);
DUMMY_HANDLER void RTC_IRQHandler(void);
DUMMY_HANDLER void EIC_IRQHandler(void);
DUMMY_HANDLER void NVMCTRL_IRQHandler(void);
DUMMY_HANDLER void DMAC_IRQHandler(void);
DUMMY_HANDLER void USB_IRQHandler(void);
DUMMY_HANDLER void EVSYS_IRQHandler(void);
DUMMY_HANDLER void SERCOM0_IRQHandler(void);
DUMMY_HANDLER void SERCOM1_IRQHandler(void);
DUMMY_HANDLER void SERCOM2_IRQHandler(void);
DUMMY_HANDLER void SERCOM3_IRQHandler(void);
DUMMY_HANDLER void SERCOM4_IRQHandler(void);
DUMMY_HANDLER void SERCOM5_IRQHandler(void);
DUMMY_HANDLER void TCC0_IRQHandler(void);
DUMMY_HANDLER void TCC1_IRQHandler(void);
DUMMY_HANDLER void TCC2_IRQHandler(void);
DUMMY_HANDLER void TC3_IRQHandler(void);
DUMMY_HANDLER void TC4_IRQHandler(void);
DUMMY_HANDLER void TC5_IRQHandler(void);
DUMMY_HANDLER void TC6_IRQHandler(void);
DUMMY_HANDLER void TC7_IRQHandler(void);
DUMMY_HANDLER void ADC_IRQHandler(void);
DUMMY_HANDLER void AC_IRQHandler(void);
DUMMY_HANDLER void DAC_IRQHandler(void);
DUMMY_HANDLER void PTC_IRQHandler(void);
DUMMY_HANDLER void I2S_IRQHandler(void);
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
  PM_IRQHandler,                  //  0 - Power Manager
  SYSCTRL_IRQHandler,             //  1 - System Control
  WDT_IRQHandler,                 //  2 - Watchdog Timer
  RTC_IRQHandler,                 //  3 - Real-Time Counter
  EIC_IRQHandler,                 //  4 - External Interrupt Controller
  NVMCTRL_IRQHandler,             //  5 - Non-Volatile Memory Controller
  DMAC_IRQHandler,                //  6 - Direct Memory Access Controller
  USB_IRQHandler,                 //  7 - Universal Serial Bus
  EVSYS_IRQHandler,               //  8 - Event System Interface
  SERCOM0_IRQHandler,             //  9 - Serial Communication Interface 0
  SERCOM1_IRQHandler,             // 10 - Serial Communication Interface 1
  SERCOM2_IRQHandler,             // 11 - Serial Communication Interface 2
  SERCOM3_IRQHandler,             // 12 - Serial Communication Interface 3
  SERCOM4_IRQHandler,             // 13 - Serial Communication Interface 4 (SAMD21G/J only)
  SERCOM5_IRQHandler,             // 14 - Serial Communication Interface 5 (SAMD21G/J only)
  TCC0_IRQHandler,                // 15 - Timer Counter for Control 0
  TCC1_IRQHandler,                // 16 - Timer Counter for Control 1
  TCC2_IRQHandler,                // 17 - Timer Counter for Control 2
  TC3_IRQHandler,                 // 18 - Timer Counter 3
  TC4_IRQHandler,                 // 19 - Timer Counter 4
  TC5_IRQHandler,                 // 20 - Timer Counter 5
  TC6_IRQHandler,                 // 21 - Timer Counter 6 (SAMD21J only)
  TC7_IRQHandler,                 // 22 - Timer Counter 7 (SAMD21J only)
  ADC_IRQHandler,                 // 23 - Analog-to-Digital Converter
  AC_IRQHandler,                  // 24 - Analog Comparators
  DAC_IRQHandler,                 // 25 - Digital-to-Analog Converter
  PTC_IRQHandler,                 // 26 - Peripheral Touch Controller
  I2S_IRQHandler,                 // 27 - Inter-IC Sound Interface
  #endif  // SYS_USE_VECTORS > 0
};

// Reset handler
void Reset_Handler(void) {
  uint32_t *src, *dst;

  // Set stack pointer (no literal pool, works with large programs)
  asm volatile("mov sp, %0" : : "r" (&_estack) : "memory");

  // Configure vector table location (necessary if a bootloader is used)
  SCB->VTOR = (uint32_t)vectors;

  // Copy data from FLASH to RAM
  src = &_sidata;
  dst = &_sdata;
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
