// ===================================================================================
// Basic GPIO, PWM and ADC Functions for STC8H1K08 Series Microcontrollers    * v1.0 *
// ===================================================================================
//
// Pins must be defined as P10, P11, P12, etc. - e.g.:
// #define PIN_LED P33      // LED on pin P3.3
//
// Functions available:
// --------------------
// PIN_input(PIN)           set PIN as INPUT (high impedance, no pullup) (*)
// PIN_input_PU(PIN)        set PIN as INPUT with internal PULLUP
// PIN_output(PIN)          set PIN as OUTPUT (push-pull)
// PIN_output_OD(PIN)       set PIN as OUTPUT (open-drain, no pullup)
// PIN_output_OD_PU(PIN)    set PIN as OUTPUT (open-drain, internal pullup)
// PIN_bidirect(PIN)        set PIN as quasi-bidirectional I/O (weak pull-up)
//
// PIN_pullup(PIN)          enable internal 4k1 PULLUP on PIN
// PIN_pulloff(PIN)         disable internal 4k1 PULLUP on PIN (*)
// PIN_fast(PIN)            set fast level shifting on PIN
// PIN_slow(PIN)            set slow level shifting on PIN (*)
// PIN_enable(PIN)          enable digital input on PIN (*)
// PIN_disable(PIN)         disable digital input on PIN
// PIN_strong(PIN)          set enhanced drive current ability on PIN
// PIN_weak(PIN)            set general drive current ability on PIN (*)
// PIN_ST_enable(PIN)       enable schmitt trigger function on PIN (*)
// PIN_ST_disable(PIN)      disable schmitt trigger function on PIN
//
// PIN_low(PIN)             set PIN output value to LOW
// PIN_high(PIN)            set PIN output value to HIGH (*)
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            read PIN input value
// PIN_write(PIN, val)      write PIN output value (0 = LOW / 1 = HIGH)
//
// PIN_asm(PIN)             convert PIN for inline assembly: setb PIN_asm(PIN_LED)
// PIN_set(PIN)             convert PIN for direct manipulation: PIN_set(PIN_LED) = 1;
//
// PWM_init()               init PWMA timer
// PWM_start(PIN)           start PWM output on PIN (P10 - P27, P33, P34 only)
// PWM_stop(PIN)            stop PWM output on PIN
// PWM_write(PIN, val)      set PWM output active level duty cycle on PIN (0 - 255)
//
// ADC_init()               init and enable ADC
// ADC_enable()             enable ADC
// ADC_disable()            disable ADC
// ADC_fast()               set ADC fast mode (less accurate) (*)
// ADC_slow()               set ADC slow mode (more accurate)
// ADC_input(PIN)           set ADC input pin (P10 - P11, P30 - P36 only)
// ADC_read()               sample and read ADC value (0 - 1023)
// ADC_read_VDD()           sample and read VREF, calculate and return VDD in mV
//
// CMP_enable()             enable comparator
// CMP_disable()            disable comparator
// CMP_pos_pin(PIN)         set CMP non-inverting input pin (P37 and ADC input pins only)
// CMP_neg_pin(PIN)         set CMP inverting input pin (P36 only)
// CMP_neg_VREF()           set VREF (1.19V) as inverting input
// CMP_out_pin(PIN)         set CMP output pin (P34, P41 only)
// CMP_out_enable()         enable CMP output on pin
// CMP_out_disable()        disable CMP output on pin
// CMP_out_invert()         invert CMP output
// CMP_out_noninvert()      normal CMP output (*)
// CMP_read()               read CMP output (0: pos < neg, 1: pos > neg, if not inverted)
//
// Notes:
// ------
// (*) default reset state.
// Pins used for PWM should be set as OUTPUT beforehand.
// Not all PWM pins work independently of each other. Refer to the datasheet for more info.
// Pins used for ADC or CMP input must have been set as INPUT (high impedance) beforehand.
// If ADC input pins are used for the CMP, ADC and CMP must be enabled.
// Access to extended registers must be granted (P_SW2 |= 0x80;).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include "stc8h.h"

// ===================================================================================
// Enumerate pin designators (use these designators to define pins)
// ===================================================================================

enum{ P00, P01, P02, P03, P04, P05, P06, P07, P10, P11, P12, P13, P14, P15, P16, P17,
      P20, P21, P22, P23, P24, P25, P26, P27, P30, P31, P32, P33, P34, P35, P36, P37,
      P40, P41, P42, P43, P44, P45, P46, P47, P50, P51, P52, P53, P54, P55, P56, P57,
      P60, P61, P62, P63, P64, P65, P66, P67, P70, P71, P72, P73, P74, P75, P76, P77 };

// ===================================================================================
// Helper Defines (these are for internal use only)
// ===================================================================================

// 2nd-stage glue defines
#define PIN_h_a(PIN) _P##PIN
#define PIN_h_s(PIN) P##PIN

#define PORT_h_bi(PORT, PIN)      (P##PORT##M1 &= ~(1<<PIN), P##PORT##M0 &= ~(1<<PIN))
#define PORT_h_pp(PORT, PIN)      (P##PORT##M1 &= ~(1<<PIN), P##PORT##M0 |=  (1<<PIN))
#define PORT_h_in(PORT, PIN)      (P##PORT##M1 |=  (1<<PIN), P##PORT##M0 &= ~(1<<PIN))
#define PORT_h_od(PORT, PIN)      (P##PORT##M1 |=  (1<<PIN), P##PORT##M0 |=  (1<<PIN))

#define PORT_h_pe(PORT, PIN)      (P##PORT##PU  |=  (1<<<PIN))    // pullup enable
#define PORT_h_pd(PORT, PIN)      (P##PORT##PU  &= ~(1<<<PIN))    // pullup disable
#define PORT_h_sd(PORT, PIN)      (P##PORT##NCS |=  (1<<<PIN))    // schmitt trigger disable
#define PORT_h_se(PORT, PIN)      (P##PORT##NCS &= ~(1<<<PIN))    // schmitt trigger enable
#define PORT_h_ss(PORT, PIN)      (P##PORT##SR  |=  (1<<<PIN))    // slow speed
#define PORT_h_sf(PORT, PIN)      (P##PORT##SR  &= ~(1<<<PIN))    // fast speed
#define PORT_h_dg(PORT, PIN)      (P##PORT##DR  |=  (1<<<PIN))    // general drive current
#define PORT_h_de(PORT, PIN)      (P##PORT##DR  &= ~(1<<<PIN))    // enhanced drive current
#define PORT_h_ie(PORT, PIN)      (P##PORT##IE  |=  (1<<<PIN))    // digital input enable
#define PORT_h_id(PORT, PIN)      (P##PORT##IE  &= ~(1<<<PIN))    // digital input disable

#define PORT_h_l(PORT, PIN)       PP##PORT##PIN = 0
#define PORT_h_h(PORT, PIN)       PP##PORT##PIN = 1
#define PORT_h_t(PORT, PIN)       PP##PORT##PIN = !PP##PORT##PIN
#define PORT_h_r(PORT, PIN)       (PP##PORT##PIN)
#define PORT_h_w(PORT,PIN,val)    PP##PORT##PIN = val

// ===================================================================================
// Set pin as INPUT (high impedance, no pullup)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0M1 |=  (1<<(PIN&7)), P0M0 &= ~(1<<(PIN&7)), P0PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1M1 |=  (1<<(PIN&7)), P1M0 &= ~(1<<(PIN&7)), P1PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2M1 |=  (1<<(PIN&7)), P2M0 &= ~(1<<(PIN&7)), P2PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3M1 |=  (1<<(PIN&7)), P3M0 &= ~(1<<(PIN&7)), P3PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4M1 |=  (1<<(PIN&7)), P4M0 &= ~(1<<(PIN&7)), P4PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5M1 |=  (1<<(PIN&7)), P5M0 &= ~(1<<(PIN&7)), P5PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6M1 |=  (1<<(PIN&7)), P6M0 &= ~(1<<(PIN&7)), P6PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7M1 |=  (1<<(PIN&7)), P7M0 &= ~(1<<(PIN&7)), P7PU &= ~(1<<(PIN&7))) : \
(0)))))))))

#define PIN_input_HI    PIN_input
#define PIN_input_FL    PIN_input
#define PIN_input_AN    PIN_input

// ===================================================================================
// Set pin as INPUT with internal 4k1 PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0M1 |=  (1<<(PIN&7)), P0M0 &= ~(1<<(PIN&7)), P0PU |=  (1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1M1 |=  (1<<(PIN&7)), P1M0 &= ~(1<<(PIN&7)), P1PU |=  (1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2M1 |=  (1<<(PIN&7)), P2M0 &= ~(1<<(PIN&7)), P2PU |=  (1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3M1 |=  (1<<(PIN&7)), P3M0 &= ~(1<<(PIN&7)), P3PU |=  (1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4M1 |=  (1<<(PIN&7)), P4M0 &= ~(1<<(PIN&7)), P4PU |=  (1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5M1 |=  (1<<(PIN&7)), P5M0 &= ~(1<<(PIN&7)), P5PU |=  (1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6M1 |=  (1<<(PIN&7)), P6M0 &= ~(1<<(PIN&7)), P6PU |=  (1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7M1 |=  (1<<(PIN&7)), P7M0 &= ~(1<<(PIN&7)), P7PU |=  (1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Set pin as OUTPUT (push-pull)
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0M1 &= ~(1<<(PIN&7)), P0M0 |=  (1<<(PIN&7)), P0PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1M1 &= ~(1<<(PIN&7)), P1M0 |=  (1<<(PIN&7)), P1PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2M1 &= ~(1<<(PIN&7)), P2M0 |=  (1<<(PIN&7)), P2PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3M1 &= ~(1<<(PIN&7)), P3M0 |=  (1<<(PIN&7)), P3PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4M1 &= ~(1<<(PIN&7)), P4M0 |=  (1<<(PIN&7)), P4PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5M1 &= ~(1<<(PIN&7)), P5M0 |=  (1<<(PIN&7)), P5PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6M1 &= ~(1<<(PIN&7)), P6M0 |=  (1<<(PIN&7)), P6PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7M1 &= ~(1<<(PIN&7)), P7M0 |=  (1<<(PIN&7)), P7PU &= ~(1<<(PIN&7))) : \
(0)))))))))

#define PIN_output_PP   PIN_output

// ===================================================================================
// Set pin as OPEN-DRAIN OUTPUT (no pullup)
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0M1 |=  (1<<(PIN&7)), P0M0 |=  (1<<(PIN&7)), P0PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1M1 |=  (1<<(PIN&7)), P1M0 |=  (1<<(PIN&7)), P1PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2M1 |=  (1<<(PIN&7)), P2M0 |=  (1<<(PIN&7)), P2PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3M1 |=  (1<<(PIN&7)), P3M0 |=  (1<<(PIN&7)), P3PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4M1 |=  (1<<(PIN&7)), P4M0 |=  (1<<(PIN&7)), P4PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5M1 |=  (1<<(PIN&7)), P5M0 |=  (1<<(PIN&7)), P5PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6M1 |=  (1<<(PIN&7)), P6M0 |=  (1<<(PIN&7)), P6PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7M1 |=  (1<<(PIN&7)), P7M0 |=  (1<<(PIN&7)), P7PU &= ~(1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Set pin as OPEN-DRAIN OUTPUT with internal 4k1 PULLUP resistor
// ===================================================================================
#define PIN_output_OD_PU(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0M1 |=  (1<<(PIN&7)), P0M0 |=  (1<<(PIN&7)), P0PU |=  (1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1M1 |=  (1<<(PIN&7)), P1M0 |=  (1<<(PIN&7)), P1PU |=  (1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2M1 |=  (1<<(PIN&7)), P2M0 |=  (1<<(PIN&7)), P2PU |=  (1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3M1 |=  (1<<(PIN&7)), P3M0 |=  (1<<(PIN&7)), P3PU |=  (1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4M1 |=  (1<<(PIN&7)), P4M0 |=  (1<<(PIN&7)), P4PU |=  (1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5M1 |=  (1<<(PIN&7)), P5M0 |=  (1<<(PIN&7)), P5PU |=  (1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6M1 |=  (1<<(PIN&7)), P6M0 |=  (1<<(PIN&7)), P6PU |=  (1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7M1 |=  (1<<(PIN&7)), P7M0 |=  (1<<(PIN&7)), P7PU |=  (1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Set PIN as quasi-bidirectional I/O (weak pull-up)
// ===================================================================================
#define PIN_bidirect(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0M1 &= ~(1<<(PIN&7)), P0M0 &= ~(1<<(PIN&7)), P0PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1M1 &= ~(1<<(PIN&7)), P1M0 &= ~(1<<(PIN&7)), P1PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2M1 &= ~(1<<(PIN&7)), P2M0 &= ~(1<<(PIN&7)), P2PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3M1 &= ~(1<<(PIN&7)), P3M0 &= ~(1<<(PIN&7)), P3PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4M1 &= ~(1<<(PIN&7)), P4M0 &= ~(1<<(PIN&7)), P4PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5M1 &= ~(1<<(PIN&7)), P5M0 &= ~(1<<(PIN&7)), P5PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6M1 &= ~(1<<(PIN&7)), P6M0 &= ~(1<<(PIN&7)), P6PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7M1 &= ~(1<<(PIN&7)), P7M0 &= ~(1<<(PIN&7)), P7PU &= ~(1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Enable internal 4k1 PULLUP on PIN
// ===================================================================================
#define PIN_pullup(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0PU |=  (1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1PU |=  (1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2PU |=  (1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3PU |=  (1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4PU |=  (1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5PU |=  (1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6PU |=  (1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7PU |=  (1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Disable internal 4k1 PULLUP on PIN
// ===================================================================================
#define PIN_pulloff(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6PU &= ~(1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7PU &= ~(1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Set fast level shifting on PIN
// ===================================================================================
#define PIN_fast(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0SR &= ~(1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1SR &= ~(1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2SR &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3SR &= ~(1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4SR &= ~(1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5SR &= ~(1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6SR &= ~(1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7SR &= ~(1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Set slow level shifting on PIN
// ===================================================================================
#define PIN_slow(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0SR |=  (1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1SR |=  (1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2SR |=  (1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3SR |=  (1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4SR |=  (1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5SR |=  (1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6SR |=  (1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7SR |=  (1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Enable digital input on PIN
// ===================================================================================
#define PIN_enable(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0IE |=  (1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1IE |=  (1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2IE |=  (1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3IE |=  (1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4IE |=  (1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5IE |=  (1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6IE |=  (1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7IE |=  (1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Disable digital input on PIN
// ===================================================================================
#define PIN_disable(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0IE &= ~(1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1IE &= ~(1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2IE &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3IE &= ~(1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4IE &= ~(1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5IE &= ~(1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6IE &= ~(1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7IE &= ~(1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Set enhanced drive current ability on PIN
// ===================================================================================
#define PIN_strong(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0DR &= ~(1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1DR &= ~(1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2DR &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3DR &= ~(1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4DR &= ~(1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5DR &= ~(1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6DR &= ~(1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7DR &= ~(1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Set general drive current ability on PIN
// ===================================================================================
#define PIN_weak(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0DR |=  (1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1DR |=  (1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2DR |=  (1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3DR |=  (1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4DR |=  (1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5DR |=  (1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6DR |=  (1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7DR |=  (1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Enable schmitt trigger function on PIN
// ===================================================================================
#define PIN_ST_enable(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0NCS &= ~(1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1NCS &= ~(1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2NCS &= ~(1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3NCS &= ~(1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4NCS &= ~(1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5NCS &= ~(1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6NCS &= ~(1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7NCS &= ~(1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Disable schmitt trigger function on PIN
// ===================================================================================
#define PIN_ST_disable(PIN) \
  ((PIN>=P00)&&(PIN<=P07) ? (P0NCS |=  (1<<(PIN&7))) : \
  ((PIN>=P10)&&(PIN<=P17) ? (P1NCS |=  (1<<(PIN&7))) : \
  ((PIN>=P20)&&(PIN<=P27) ? (P2NCS |=  (1<<(PIN&7))) : \
  ((PIN>=P30)&&(PIN<=P37) ? (P3NCS |=  (1<<(PIN&7))) : \
  ((PIN>=P40)&&(PIN<=P47) ? (P4NCS |=  (1<<(PIN&7))) : \
  ((PIN>=P50)&&(PIN<=P57) ? (P5NCS |=  (1<<(PIN&7))) : \
  ((PIN>=P60)&&(PIN<=P67) ? (P6NCS |=  (1<<(PIN&7))) : \
  ((PIN>=P70)&&(PIN<=P77) ? (P7NCS |=  (1<<(PIN&7))) : \
(0)))))))))

// ===================================================================================
// Pin manipulation macros
// ===================================================================================
#define PIN_low(PIN)          PIN_h_s(PIN) = 0              // set pin to LOW
#define PIN_high(PIN)         PIN_h_s(PIN) = 1              // set pin to HIGH
#define PIN_toggle(PIN)       PIN_h_s(PIN) = !PIN_h_s(PIN)  // TOGGLE pin
#define PIN_read(PIN)         (PIN_h_s(PIN))                // READ pin
#define PIN_write(PIN, val)   PIN_h_s(PIN) = val            // WRITE pin value

// ===================================================================================
// Convert pin for inline assembly and direct manipulation
// ===================================================================================
#define PIN_asm(PIN)          PIN_h_a(PIN)
#define PIN_set(PIN)          PIN_h_s(PIN)

// ===================================================================================
// (PORT, PIN) manipulation macros
// ===================================================================================
#define PORT_input(PORT, PIN)     PORT_h_i(PORT, PIN)       // set pin as INPUT
#define PORT_input_PU(PORT, PIN)  PORT_h_iP(PORT, PIN)      // set pin as INPUT PULLUP
#define PORT_output(PORT, PIN)    PORT_h_o(PORT, PIN)       // set pin as OUTPUT
#define PORT_output_OD(PORT, PIN) PORT_h_oO(PORT, PIN)      // set pin as OUTPUT OPEN-DRAIN

#define PORT_low(PORT, PIN)       PORT_h_l(PORT, PIN)       // set pin to LOW
#define PORT_high(PORT, PIN)      PORT_h_h(PORT, PIN)       // set pin to HIGH
#define PORT_toggle(PORT, PIN)    PORT_h_t(PORT, PIN)       // TOGGLE pin
#define PORT_read(PORT, PIN)      PORT_h_r(PORT, PIN)       // READ pin
#define PORT_write(PORT,PIN,val)  PORT_h_w(PORT, PIN, val)  // WRITE pin value

// ===================================================================================
// Basic PWM functions
// ===================================================================================

inline void PWM_init() {
  PWMA_CCER1 = 0x00;        // close channels first
  PWMA_CCER2 = 0x00;
  PWMA_CCMR1 = 0x60;        // set PWMA output mode
  PWMA_CCMR2 = 0x60;
  PWMA_CCMR3 = 0x60;
  PWMA_CCMR4 = 0x60;
  PWMA_CCER1 = 0x55;        // re-enable channels
  PWMA_CCER2 = 0x55;
  PWMA_ARRH  = 0x00;        // set period
  PWMA_ARRL  = 0xff;
  PWMA_CCR1H = 0x00;        // clear duty cycle high bytes
  PWMA_CCR2H = 0x00;
  PWMA_CCR3H = 0x00;
  PWMA_CCR4H = 0x00;
  PWMA_BKR   = 0x80;        // enable main output
  PWMA_CR1   = 0x01;        // enable counter
}

#define PWM_start(PIN) \
  ((PIN == P10) ? (PWMA_PS &= 0xfc,                  PWMA_ENO |= 0x01) : \
  ((PIN == P11) ? (PWMA_PS &= 0xfc,                  PWMA_ENO |= 0x02) : \
  ((PIN == P12) ? (PWMA_PS &= 0xf3,                  PWMA_ENO |= 0x04) : \
  ((PIN == P13) ? (PWMA_PS &= 0xf3,                  PWMA_ENO |= 0x08) : \
  ((PIN == P14) ? (PWMA_PS &= 0xcf,                  PWMA_ENO |= 0x10) : \
  ((PIN == P15) ? (PWMA_PS &= 0xcf,                  PWMA_ENO |= 0x20) : \
  ((PIN == P16) ? (PWMA_PS &= 0x3f,                  PWMA_ENO |= 0x40) : \
  ((PIN == P17) ? (PWMA_PS &= 0x3f,                  PWMA_ENO |= 0x80) : \
  ((PIN == P20) ? (PWMA_PS &= 0xfc, PWMA_PS |= 0x01, PWMA_ENO |= 0x01) : \
  ((PIN == P21) ? (PWMA_PS &= 0xfc, PWMA_PS |= 0x01, PWMA_ENO |= 0x02) : \
  ((PIN == P22) ? (PWMA_PS &= 0xf3, PWMA_PS |= 0x04, PWMA_ENO |= 0x04) : \
  ((PIN == P23) ? (PWMA_PS &= 0xf3, PWMA_PS |= 0x04, PWMA_ENO |= 0x08) : \
  ((PIN == P24) ? (PWMA_PS &= 0xcf, PWMA_PS |= 0x10, PWMA_ENO |= 0x10) : \
  ((PIN == P25) ? (PWMA_PS &= 0xcf, PWMA_PS |= 0x10, PWMA_ENO |= 0x20) : \
  ((PIN == P26) ? (PWMA_PS &= 0x3f, PWMA_PS |= 0x40, PWMA_ENO |= 0x40) : \
  ((PIN == P27) ? (PWMA_PS &= 0x3f, PWMA_PS |= 0x40, PWMA_ENO |= 0x80) : \
  ((PIN == P34) ? (                 PWMA_PS |= 0xc0, PWMA_ENO |= 0x40) : \
  ((PIN == P33) ? (                 PWMA_PS |= 0xc0, PWMA_ENO |= 0x80) : \
(0)))))))))))))))))))

#define PWM_stop(PIN) \
  ((PIN == P10) ? (PWMA_ENO &= ~0x01) : \
  ((PIN == P11) ? (PWMA_ENO &= ~0x02) : \
  ((PIN == P12) ? (PWMA_ENO &= ~0x04) : \
  ((PIN == P13) ? (PWMA_ENO &= ~0x08) : \
  ((PIN == P14) ? (PWMA_ENO &= ~0x10) : \
  ((PIN == P15) ? (PWMA_ENO &= ~0x20) : \
  ((PIN == P16) ? (PWMA_ENO &= ~0x40) : \
  ((PIN == P17) ? (PWMA_ENO &= ~0x80) : \
  ((PIN == P20) ? (PWMA_ENO &= ~0x01) : \
  ((PIN == P21) ? (PWMA_ENO &= ~0x02) : \
  ((PIN == P22) ? (PWMA_ENO &= ~0x04) : \
  ((PIN == P23) ? (PWMA_ENO &= ~0x08) : \
  ((PIN == P24) ? (PWMA_ENO &= ~0x10) : \
  ((PIN == P25) ? (PWMA_ENO &= ~0x20) : \
  ((PIN == P26) ? (PWMA_ENO &= ~0x40) : \
  ((PIN == P27) ? (PWMA_ENO &= ~0x80) : \
  ((PIN == P34) ? (PWMA_ENO &= ~0x40) : \
  ((PIN == P33) ? (PWMA_ENO &= ~0x80) : \
(0)))))))))))))))))))

#define PWM_write(PIN, val) \
  ((PIN == P10) ? (PWMA_CCR1L =  (val)) : \
  ((PIN == P11) ? (PWMA_CCR1L = ~(val)) : \
  ((PIN == P12) ? (PWMA_CCR2L =  (val)) : \
  ((PIN == P13) ? (PWMA_CCR2L = ~(val)) : \
  ((PIN == P14) ? (PWMA_CCR3L =  (val)) : \
  ((PIN == P15) ? (PWMA_CCR3L = ~(val)) : \
  ((PIN == P16) ? (PWMA_CCR4L =  (val)) : \
  ((PIN == P17) ? (PWMA_CCR4L = ~(val)) : \
  ((PIN == P20) ? (PWMA_CCR1L =  (val)) : \
  ((PIN == P21) ? (PWMA_CCR1L = ~(val)) : \
  ((PIN == P22) ? (PWMA_CCR2L =  (val)) : \
  ((PIN == P23) ? (PWMA_CCR2L = ~(val)) : \
  ((PIN == P24) ? (PWMA_CCR3L =  (val)) : \
  ((PIN == P25) ? (PWMA_CCR3L = ~(val)) : \
  ((PIN == P26) ? (PWMA_CCR4L =  (val)) : \
  ((PIN == P27) ? (PWMA_CCR4L = ~(val)) : \
  ((PIN == P34) ? (PWMA_CCR4L =  (val)) : \
  ((PIN == P33) ? (PWMA_CCR4L = ~(val)) : \
(0)))))))))))))))))))

// ===================================================================================
// ADC functions (P10 - P17, P30 - P36, VREF only)
// ===================================================================================
#define ADC_enable()        ADC_CONTR |=  0x80
#define ADC_disable()       ADC_CONTR &= ~0x80
#define ADC_fast()          ADCTIM = 0b00101010
#define ADC_slow()          ADCTIM = 0b11111111
#define ADC_clockdiv(n)     ADCCFG = (ADCCFG & 0xf0) | ((n) & 0x0f)

#define ADC_input(PIN) \
  ((PIN == P10) ? (ADC_CONTR &= 0xf0) : \
  ((PIN == P11) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x01) : \
  ((PIN == P12) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x02) : \
  ((PIN == P13) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x03) : \
  ((PIN == P14) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x04) : \
  ((PIN == P15) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x05) : \
  ((PIN == P16) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x06) : \
  ((PIN == P17) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x07) : \
  ((PIN == P30) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x08) : \
  ((PIN == P31) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x09) : \
  ((PIN == P32) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0a) : \
  ((PIN == P33) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0b) : \
  ((PIN == P34) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0c) : \
  ((PIN == P35) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0d) : \
  ((PIN == P36) ? (ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0e) : \
(0))))))))))

#define ADC_input_VREF()    ADC_CONTR |= 0x0f

inline void ADC_init(void) {
  ADC_CONTR = 0x80;
  ADCCFG    = 0x20 | (F_CPU / 3000000);
  ADCTIM    = 0b00101010;
}

inline uint16_t ADC_read(void) {
  ADC_CONTR |= 0x40;
  while(ADC_CONTR & 0x40);
  return (((uint16_t)ADC_RES << 8) | ADC_RESL);
}

inline uint16_t ADC_read_VDD(void) {
  ADC_input_VREF();                             // set VREF as ADC input
  return((uint32_t)1190 * 1023 / ADC_read());   // return VDD in mV
}

// ===================================================================================
// Comparator Functions (POS: P37, ADC-pins / NEG: P36, VREF / OUT: P34, P41)
// ===================================================================================
#define CMP_enable()        CMPCR1 |=  0x80
#define CMP_disable()       CMPCR1 &= ~0x80
#define CMP_read()          (CMPCR1 & 0x01)

#define CMP_pos_pin(PIN) \
  ((PIN == P37) ? (CMPCR1 &= ~0x08) : \
  ((PIN == P10) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0) : \
  ((PIN == P11) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x01) : \
  ((PIN == P12) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x02) : \
  ((PIN == P13) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x03) : \
  ((PIN == P14) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x04) : \
  ((PIN == P15) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x05) : \
  ((PIN == P16) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x06) : \
  ((PIN == P17) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x07) : \
  ((PIN == P30) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x08) : \
  ((PIN == P31) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x09) : \
  ((PIN == P32) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0a) : \
  ((PIN == P33) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0b) : \
  ((PIN == P34) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0c) : \
  ((PIN == P35) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0d) : \
  ((PIN == P36) ? (CMPCR1 |=  0x08, ADC_CONTR &= 0xf0, ADC_CONTR |= 0x0e) : \
(0)))))))))))

#define CMP_neg_pin(PIN)    ((PIN == P36) ? (CMPCR1 |= 0x04) : (CMPCR1 &= ~0x04))
#define CMP_neg_VREF()      CMPCR1 &= ~0x04

#define CMP_out_pin(PIN)    ((PIN == P34) ? (P_SW2 &= f7) : ((PIN == P41) ? (P_SW2 |= 0x08) : (0)))
#define CMP_out_enable()    CMPCR1 |=  0x02
#define CMP_out_disable()   CMPCR1 &= ~0x02
#define CMP_out_invert()    CMPCR2 |=  0x80
#define CMP_out_noninvert() CMPCR2 &= ~0x80
