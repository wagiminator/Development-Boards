// ===================================================================================
// GPIO and PWM Functions for CH551, CH552 and CH554
// ===================================================================================
//
// Pins must be defined as P10, P11, P12, etc. - e.g.:
// #define PIN_LED P33      // LED on pin P3.3
//
// Functions available:
// --------------------
// PIN_input(PIN)           Set PIN as INPUT (high impedance, no pullup)
// PIN_input_PU(PIN)        Set PIN as INPUT with internal PULLUP
// PIN_output(PIN)          Set PIN as OUTPUT (push-pull)
// PIN_output_OD(PIN)       Set PIN as OUTPUT (open-drain)
//
// PIN_low(PIN)             Set PIN output value to LOW
// PIN_high(PIN)            Set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            Read PIN input value
// PIN_write(PIN, val)      Write PIN output value (0 = LOW / 1 = HIGH)
//
// PWM_start(PIN)           Start PWM output on PIN (must be a PWM-capable pin!!!)
// PWM_stop(PIN)            Stop PWM output on PIN
// PWM_write(PIN, val)      Set PWM output active level duty cycle on PIN
//
// PWM_pol_normal(PIN)      Set normal PWM polarity on PIN (default low and active high)
// PWM_pol_reverse(PIN)     Set reverse PWM polarity on PIN (default high and active low)
// PWM_set_freq(freq)       Set global PWM frequency (in Hertz)

#pragma once
#include "ch554.h"

// ===================================================================================
// Pin definitions
// ===================================================================================

// Enumerate pin designators (use these designators to define pins)
enum{P10, P11, P12, P13, P14, P15, P16, P17, P30, P31, P32, P33, P34, P35, P36, P37};

// Define pins for direct bit manipulation (these are for internal use only)
SBIT(P1_0, 0x90, 0);
SBIT(P1_1, 0x90, 1);
SBIT(P1_2, 0x90, 2);
SBIT(P1_3, 0x90, 3);
SBIT(P1_4, 0x90, 4);
SBIT(P1_5, 0x90, 5);
SBIT(P1_6, 0x90, 6);
SBIT(P1_7, 0x90, 7);
SBIT(P3_0, 0xB0, 0);
SBIT(P3_1, 0xB0, 1);
SBIT(P3_2, 0xB0, 2);
SBIT(P3_3, 0xB0, 3);
SBIT(P3_4, 0xB0, 4);
SBIT(P3_5, 0xB0, 5);
SBIT(P3_6, 0xB0, 6);
SBIT(P3_7, 0xB0, 7);

// For pin error
inline void WRONGPIN(void) {
}

// ===================================================================================
// Set pin as INPUT (high impedance, no pullup)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=P10)&&(PIN<=P17) ? (P1_DIR_PU &= ~(1<<(PIN&7)), P1_MOD_OC &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3_DIR_PU &= ~(1<<(PIN&7)), P3_MOD_OC &= ~(1<<(PIN&7))) : \
(WRONGPIN())))

// ===================================================================================
// Set pin as INPUT with internal PULLUP resistor (also open-drain output,
// when output changes from LOW to HIGH, it will drive HIGH for 2 clock cycles)
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=P10)&&(PIN<=P17) ? (P1_MOD_OC |= (1<<(PIN&7)), P1_DIR_PU |= (1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3_MOD_OC |= (1<<(PIN&7)), P3_DIR_PU |= (1<<(PIN&7))) : \
(WRONGPIN())))

// ===================================================================================
// Set pin as OUTPUT (push-pull)
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=P10)&&(PIN<=P17) ? (P1_MOD_OC &= ~(1<<(PIN&7)), P1_DIR_PU |= (1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3_MOD_OC &= ~(1<<(PIN&7)), P3_DIR_PU |= (1<<(PIN&7))) : \
(WRONGPIN())))

// ===================================================================================
// Set pin as OPEN-DRAIN OUTPUT (also high-impedance input, no pullup)
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=P10)&&(PIN<=P17) ? (P1_MOD_OC |= (1<<(PIN&7)), P1_DIR_PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3_MOD_OC |= (1<<(PIN&7)), P3_DIR_PU &= ~(1<<(PIN&7))) : \
(WRONGPIN())))

// ===================================================================================
// Set pin to LOW
// ===================================================================================
#define PIN_low(PIN) \
  (PIN == P10 ? (P1_0 = 0) : \
  (PIN == P11 ? (P1_1 = 0) : \
  (PIN == P12 ? (P1_2 = 0) : \
  (PIN == P13 ? (P1_3 = 0) : \
  (PIN == P14 ? (P1_4 = 0) : \
  (PIN == P15 ? (P1_5 = 0) : \
  (PIN == P16 ? (P1_6 = 0) : \
  (PIN == P17 ? (P1_7 = 0) : \
  (PIN == P30 ? (P3_0 = 0) : \
  (PIN == P31 ? (P3_1 = 0) : \
  (PIN == P32 ? (P3_2 = 0) : \
  (PIN == P33 ? (P3_3 = 0) : \
  (PIN == P34 ? (P3_4 = 0) : \
  (PIN == P35 ? (P3_5 = 0) : \
  (PIN == P36 ? (P3_6 = 0) : \
  (PIN == P37 ? (P3_7 = 0) : \
(WRONGPIN())))))))))))))))))

// ===================================================================================
// Set pin to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  (PIN == P10 ? (P1_0 = 1) : \
  (PIN == P11 ? (P1_1 = 1) : \
  (PIN == P12 ? (P1_2 = 1) : \
  (PIN == P13 ? (P1_3 = 1) : \
  (PIN == P14 ? (P1_4 = 1) : \
  (PIN == P15 ? (P1_5 = 1) : \
  (PIN == P16 ? (P1_6 = 1) : \
  (PIN == P17 ? (P1_7 = 1) : \
  (PIN == P30 ? (P3_0 = 1) : \
  (PIN == P31 ? (P3_1 = 1) : \
  (PIN == P32 ? (P3_2 = 1) : \
  (PIN == P33 ? (P3_3 = 1) : \
  (PIN == P34 ? (P3_4 = 1) : \
  (PIN == P35 ? (P3_5 = 1) : \
  (PIN == P36 ? (P3_6 = 1) : \
  (PIN == P37 ? (P3_7 = 1) : \
(WRONGPIN())))))))))))))))))

// ===================================================================================
// TOGGLE pin
// ===================================================================================
#define PIN_toggle(PIN) \
  (PIN == P10 ? (P1_0 = !P1_0) : \
  (PIN == P11 ? (P1_1 = !P1_1) : \
  (PIN == P12 ? (P1_2 = !P1_2) : \
  (PIN == P13 ? (P1_3 = !P1_3) : \
  (PIN == P14 ? (P1_4 = !P1_4) : \
  (PIN == P15 ? (P1_5 = !P1_5) : \
  (PIN == P16 ? (P1_6 = !P1_6) : \
  (PIN == P17 ? (P1_7 = !P1_7) : \
  (PIN == P30 ? (P3_0 = !P3_0) : \
  (PIN == P31 ? (P3_1 = !P3_1) : \
  (PIN == P32 ? (P3_2 = !P3_2) : \
  (PIN == P33 ? (P3_3 = !P3_3) : \
  (PIN == P34 ? (P3_4 = !P3_4) : \
  (PIN == P35 ? (P3_5 = !P3_5) : \
  (PIN == P36 ? (P3_6 = !P3_6) : \
  (PIN == P37 ? (P3_7 = !P3_7) : \
(WRONGPIN())))))))))))))))))

// ===================================================================================
// READ pin
// ===================================================================================
#define PIN_read(PIN) \
  (PIN == P10 ? (P1_0) : \
  (PIN == P11 ? (P1_1) : \
  (PIN == P12 ? (P1_2) : \
  (PIN == P13 ? (P1_3) : \
  (PIN == P14 ? (P1_4) : \
  (PIN == P15 ? (P1_5) : \
  (PIN == P16 ? (P1_6) : \
  (PIN == P17 ? (P1_7) : \
  (PIN == P30 ? (P3_0) : \
  (PIN == P31 ? (P3_1) : \
  (PIN == P32 ? (P3_2) : \
  (PIN == P33 ? (P3_3) : \
  (PIN == P34 ? (P3_4) : \
  (PIN == P35 ? (P3_5) : \
  (PIN == P36 ? (P3_6) : \
  (PIN == P37 ? (P3_7) : \
(WRONGPIN())))))))))))))))))

// ===================================================================================
// WRITE pin
// ===================================================================================
#define PIN_write(PIN, val) \
  (PIN == P10 ? (P1_0 = val) : \
  (PIN == P11 ? (P1_1 = val) : \
  (PIN == P12 ? (P1_2 = val) : \
  (PIN == P13 ? (P1_3 = val) : \
  (PIN == P14 ? (P1_4 = val) : \
  (PIN == P15 ? (P1_5 = val) : \
  (PIN == P16 ? (P1_6 = val) : \
  (PIN == P17 ? (P1_7 = val) : \
  (PIN == P30 ? (P3_0 = val) : \
  (PIN == P31 ? (P3_1 = val) : \
  (PIN == P32 ? (P3_2 = val) : \
  (PIN == P33 ? (P3_3 = val) : \
  (PIN == P34 ? (P3_4 = val) : \
  (PIN == P35 ? (P3_5 = val) : \
  (PIN == P36 ? (P3_6 = val) : \
  (PIN == P37 ? (P3_7 = val) : \
(WRONGPIN())))))))))))))))))

// ===================================================================================
// (PORT, PIN) manipulation macros
// ===================================================================================
#define PORT_input(PORT, PIN)     (P##PORT##_DIR_PU &= ~(1<<PIN), P##PORT##_MOD_OC &= ~(1<<PIN))
#define PORT_input_PU(PORT, PIN)  (P##PORT##_MOD_OC |=  (1<<PIN), P##PORT##_DIR_PU |=  (1<<PIN))
#define PORT_output(PORT, PIN)    (P##PORT##_MOD_OC &= ~(1<<PIN), P##PORT##_DIR_PU |=  (1<<PIN))
#define PORT_output_OD(PORT, PIN) (P##PORT##_MOD_OC |=  (1<<PIN), P##PORT##_DIR_PU &= ~(1<<PIN))

#define PORT_low(PORT, PIN)       P##PORT##_##PIN = 0
#define PORT_high(PORT, PIN)      P##PORT##_##PIN = 1
#define PORT_toggle(PORT, PIN)    P##PORT##_##PIN = !P##PORT##_##PIN
#define PORT_read(PORT, PIN)      (P##PORT##_##PIN)
#define PORT_write(PORT,PIN,val)  P##PORT##_##PIN = val

// ===================================================================================
// Start PWM on pin (must be a PWM-capable pin!!!)
// ===================================================================================
#define PWM_start(PIN) \
  ((PIN == P15) ? (PIN_FUNC &= ~bPWM1_PIN_X, PWM_CTRL |= bPWM1_OUT_EN) : \
  ((PIN == P34) ? (PIN_FUNC &= ~bPWM2_PIN_X, PWM_CTRL |= bPWM2_OUT_EN) : \
  ((PIN == P30) ? (PIN_FUNC |=  bPWM1_PIN_X, PWM_CTRL |= bPWM1_OUT_EN) : \
  ((PIN == P31) ? (PIN_FUNC |=  bPWM2_PIN_X, PWM_CTRL |= bPWM2_OUT_EN) : \
(WRONGPIN())))))

// ===================================================================================
// Set PWM output active level duty cycle on pin
// ===================================================================================
#define PWM_write(PIN, val) \
  ((PIN == P15) ? (PWM_DATA1 = val) : \
  ((PIN == P34) ? (PWM_DATA2 = val) : \
  ((PIN == P30) ? (PWM_DATA1 = val) : \
  ((PIN == P31) ? (PWM_DATA2 = val) : \
(WRONGPIN())))))

// ===================================================================================
// Stop PWM on pin
// ===================================================================================
#define PWM_stop(PIN) \
  ((PIN == P15) ? (PWM_CTRL &= ~bPWM1_OUT_EN) : \
  ((PIN == P34) ? (PWM_CTRL &= ~bPWM2_OUT_EN) : \
  ((PIN == P30) ? (PWM_CTRL &= ~bPWM1_OUT_EN) : \
  ((PIN == P31) ? (PWM_CTRL &= ~bPWM2_OUT_EN) : \
(WRONGPIN())))))

// ===================================================================================
// Set normal PWM polarity on pin (default low and active high)
// ===================================================================================
#define PWM_pol_normal(PIN) \
  ((PIN == P15) ? (PWM_CTRL &= ~bPWM1_POLAR) : \
  ((PIN == P34) ? (PWM_CTRL &= ~bPWM2_POLAR) : \
  ((PIN == P30) ? (PWM_CTRL &= ~bPWM1_POLAR) : \
  ((PIN == P31) ? (PWM_CTRL &= ~bPWM2_POLAR) : \
(WRONGPIN())))))

// ===================================================================================
// Set reverse PWM polarity on pin (default high and active low)
// ===================================================================================
#define PWM_pol_reverse(PIN) \
  ((PIN == P15) ? (PWM_CTRL |= bPWM1_POLAR) : \
  ((PIN == P34) ? (PWM_CTRL |= bPWM2_POLAR) : \
  ((PIN == P30) ? (PWM_CTRL |= bPWM1_POLAR) : \
  ((PIN == P31) ? (PWM_CTRL |= bPWM2_POLAR) : \
(WRONGPIN())))))

// ===================================================================================
// Set global PWM frequency (in Hertz, range: FREQ_SYS/65536 - FREQ_SYS/256)
// ===================================================================================
#define PWM_set_freq(freq) \
  (((freq) >= FREQ_SYS / 256) ? (PWM_CK_SE = 0)             : \
  (((FREQ_SYS / 256 / (frq) - 1) > 255) ? (PWM_CK_SE = 255) : \
  (PWM_CK_SE = (uint8_t)(FREQ_SYS / 256 / (frq) - 1))         \
))
