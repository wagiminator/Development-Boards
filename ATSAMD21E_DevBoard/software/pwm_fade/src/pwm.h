// ===================================================================================
// Basic PWM Functions using Timer/Counter for Control (TCC0..2) for SAMD21   * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PWM_init()               init TCC0..2 for PWM with frequency and resolution set below
// PWM_start(PIN)           start PWM output on PIN (see below)
// PWM_stop(PIN)            stop PWM output on PIN (PIN becomes LOW output)
// PWM_write(PIN, val)      set PWM duty cycle of PIN (0 .. PWM_TOP+1 = 0% .. 100%)
// PWM_setTOP(n)            set PWM TOP value of all TCCs (period = n + 1 clock cycles)
// PWM_setPRESC(n)          set PWM clock prescaler of all TCCs (see below)
// PWM_enable()             enable TCC0..2 (start counters)
// PWM_disable()            disable TCC0..2 (stop counters)
//
// PWM pins and channels:
// ----------------------
// Timer Channel  Pins
// TCC0  CH0    PA4, PA8, PA14, PA22, PB10 (G/J), PB16 (J), PB30 (J)
// TCC0  CH1    PA5, PA9, PA15, PA23, PB11 (G/J), PB17 (J), PB31 (J)
// TCC0  CH2    PA10, PA18, PA20 (G/J), PB12 (J)
// TCC0  CH3    PA11, PA19, PA21 (G/J), PB13 (J)
// TCC1  CH0    PA6, PA24, PA30
// TCC1  CH1    PA7, PA25, PA31
// TCC2  CH0    PA0, PA12 (G/J), PA16
// TCC2  CH1    PA1, PA13 (G/J), PA17
//
// Notes:
// ------
// - TCC0 has 4 compare channels (24-bit), TCC1 and TCC2 have 2 compare channels each
//   (TCC1: 24-bit, TCC2: 16-bit). Pins of the same channel output the same PWM signal
//   (same duty cycle).
// - PWM frequency = F_CPU / PRESCALER / (PWM_TOP + 1), e.g. 48MHz / 1 / 256 = 187.5kHz
//   Prescaler values: 0: DIV1, 1: DIV2, 2: DIV4, 3: DIV8, 4: DIV16, 5: DIV64,
//   6: DIV256, 7: DIV1024.
// - PA24/PA25 are the USB pins, PA30/PA31 are the SWD pins (SWCLK/SWDIO).
// - Pins marked with (G/J) or (J) are only available on SAMD21G/J or SAMD21J.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"
#include "gpio.h"

// PWM parameters
#define PWM_PRESC       0         // prescaler (0..7, see above)
#define PWM_TOP         255       // TOP value (resolution), max. 0xffff (TCC2: 16-bit)

#if PWM_TOP > 0xffff
  #error PWM_TOP must be <= 0xffff
#endif

// PWM timer (0..2), channel and peripheral function (4 = E, 5 = F) of PIN
#define PWM_TCC(PIN) \
  ( ((PIN) == PA4) || ((PIN) == PA5) || ((PIN) == PA8) || ((PIN) == PA9) || ((PIN) == PA10) \
   || ((PIN) == PA11) || ((PIN) == PA14) || ((PIN) == PA15) || ((PIN) == PA18) || ((PIN) == PA19) \
   || ((PIN) == PA20) || ((PIN) == PA21) || ((PIN) == PA22) || ((PIN) == PA23) || ((PIN) == PB10) \
   || ((PIN) == PB11) || ((PIN) == PB12) || ((PIN) == PB13) || ((PIN) == PB16) || ((PIN) == PB17) \
   || ((PIN) == PB30) || ((PIN) == PB31) ) ? 0 : \
  ( ((PIN) == PA0) || ((PIN) == PA1) || ((PIN) == PA12) || ((PIN) == PA13) || ((PIN) == PA16) \
   || ((PIN) == PA17) ) ? 2 : \
  1

#define PWM_CH(PIN) \
  ( ((PIN) == PA0) || ((PIN) == PA4) || ((PIN) == PA6) || ((PIN) == PA8) || ((PIN) == PA12) \
   || ((PIN) == PA14) || ((PIN) == PA16) || ((PIN) == PA22) || ((PIN) == PA24) || ((PIN) == PA30) \
   || ((PIN) == PB10) || ((PIN) == PB16) || ((PIN) == PB30) ) ? 0 : \
  ( ((PIN) == PA1) || ((PIN) == PA5) || ((PIN) == PA7) || ((PIN) == PA9) || ((PIN) == PA13) \
   || ((PIN) == PA15) || ((PIN) == PA17) || ((PIN) == PA23) || ((PIN) == PA25) || ((PIN) == PA31) \
   || ((PIN) == PB11) || ((PIN) == PB17) || ((PIN) == PB31) ) ? 1 : \
  ( ((PIN) == PA10) || ((PIN) == PA18) || ((PIN) == PA20) || ((PIN) == PB12) ) ? 2 : \
  3

#define PWM_MUX(PIN) \
  ( ((PIN) == PA10) || ((PIN) == PA11) || ((PIN) == PA14) || ((PIN) == PA15) || ((PIN) == PA18) \
   || ((PIN) == PA19) || ((PIN) == PA20) || ((PIN) == PA21) || ((PIN) == PA22) || ((PIN) == PA23) \
   || ((PIN) == PA24) || ((PIN) == PA25) || ((PIN) == PB10) || ((PIN) == PB11) || ((PIN) == PB12) \
   || ((PIN) == PB13) || ((PIN) == PB16) || ((PIN) == PB17) ) ? 5 : \
  4

// PWM functions and macros
#define PWM_start(PIN)    (PIN_low(PIN), PIN_output(PIN), PIN_alternate(PIN, PWM_MUX(PIN)))
#define PWM_stop(PIN)     (PIN_low(PIN), PIN_alternate_off(PIN))
#define PWM_write(PIN, v) PWM_setCC(PWM_TCC(PIN), PWM_CH(PIN), v)

void PWM_init(void);
void PWM_enable(void);
void PWM_disable(void);
void PWM_setCC(uint8_t tcc, uint8_t ch, uint32_t val);
void PWM_setTOP(uint32_t top);
void PWM_setPRESC(uint8_t presc);

#ifdef __cplusplus
};
#endif
