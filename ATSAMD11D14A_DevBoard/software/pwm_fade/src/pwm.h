// ===================================================================================
// Basic PWM Functions using Timer/Counter for Control (TCC0) for SAMD11      * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PWM_init()               init TCC0 for PWM with frequency and resolution set below
// PWM_start(PIN)           start PWM output on PIN (see below)
// PWM_stop(PIN)            stop PWM output on PIN (PIN becomes LOW output)
// PWM_write(PIN, val)      set PWM duty cycle of PIN (0 .. PWM_TOP+1 = 0% .. 100%)
// PWM_setTOP(n)            set PWM TOP value (period = n + 1 clock cycles)
// PWM_setPRESC(n)          set PWM clock prescaler (see below)
// PWM_enable()             enable TCC0 (start counter)
// PWM_disable()            disable TCC0 (stop counter)
//
// PWM pins and channels:
// ----------------------
// Channel   0     1     2     3
// SAMD11C  PA4   PA5   PA8   PA9
//          PA14  PA15  PA24  PA25
//                      PA30  PA31
// SAMD11D  PA22  PA23  PA6   PA7
//                      PA10  PA11
//                      PA16  PA17
//
// Notes:
// ------
// - TCC0 has 4 compare channels with 8 waveform outputs. Pins of the same channel
//   output the same PWM signal (same duty cycle).
// - PWM frequency = F_CPU / PRESCALER / (PWM_TOP + 1), e.g. 48MHz / 1 / 256 = 187.5kHz
//   Prescaler values: 0: DIV1, 1: DIV2, 2: DIV4, 3: DIV8, 4: DIV16, 5: DIV64,
//   6: DIV256, 7: DIV1024.
// - PA24/PA25 are the USB pins, PA30/PA31 are the SWD pins (SWCLK/SWDIO).
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
#define PWM_TOP         255       // TOP value (resolution), max. 0xffffff (24-bit)

// PWM channel and peripheral function (E or F) of PIN
#define PWM_CH(PIN) \
  ( ((PIN) == PA4)  || ((PIN) == PA14) || ((PIN) == PA22) ? 0 :                     \
    ((PIN) == PA5)  || ((PIN) == PA15) || ((PIN) == PA23) ? 1 :                     \
    ((PIN) == PA6)  || ((PIN) == PA8)  || ((PIN) == PA10) || ((PIN) == PA16)        \
                    || ((PIN) == PA24) || ((PIN) == PA30) ? 2 : 3 )
#define PWM_MUX(PIN) \
  ( ((PIN) == PA8)  || ((PIN) == PA9)  || ((PIN) == PA24) || ((PIN) == PA25) ? 4 : 5 )

// PWM functions and macros
#define PWM_sync()        while(TCC0->SYNCBUSY.reg)
#define PWM_start(PIN)    (PIN_low(PIN), PIN_output(PIN), PIN_alternate(PIN, PWM_MUX(PIN)))
#define PWM_stop(PIN)     (PIN_low(PIN), PIN_alternate_off(PIN))
#define PWM_write(PIN, v) PWM_setCC(PWM_CH(PIN), v)

void PWM_init(void);
void PWM_enable(void);
void PWM_disable(void);
void PWM_setCC(uint8_t ch, uint32_t val);
void PWM_setTOP(uint32_t top);
void PWM_setPRESC(uint8_t presc);

#ifdef __cplusplus
};
#endif
