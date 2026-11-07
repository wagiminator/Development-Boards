// ===================================================================================
// Basic GPIO Functions for megaAVR 0-Series                                  * v1.0 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PF6 - e.g.:
// #define PIN_LED PA7      // LED on pin PA7
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           set PIN as INPUT (*)
// PIN_output(PIN)          set PIN as OUTPUT
// PIN_pullup(PIN)          enable internal PULLUP resistor on PIN
// PIN_pulloff(PIN)         disable internal PULLUP resistor on PIN (*)
// PIN_invert(PIN)          invert input and output values on PIN
// PIN_invertoff(PIN)       do not invert input and output values on PIN (*)
//
// PIN_low(PIN)             set PIN output value to LOW (*)
// PIN_high(PIN)            set PIN output value to HIGH
// PIN_toggle(PIN)          toggle PIN output value
// PIN_read(PIN)            read PIN input value
// PIN_write(PIN, val)      write PIN output value (0 = LOW / 1 = HIGH)
//
// PIN_enable(PIN)          enable digital input and disable interrupt on PIN (*)
// PIN_disable(PIN)         disable digital input and disable interrupt on PIN
// PIN_INT_falling(PIN)     enable digital input and interrupt on falling edge
// PIN_INT_rising(PIN)      enable digital input and interrupt on rising edge
// PIN_INT_change(PIN)      enable digital input and interrupt on both edges (pin change)
// PIN_INT_level(PIN)       enable digital input and interrupt on low level
// PIN_all_disable()        disable digital input and disable interrupt on all pins
//
// PIN_FLAG_read(PIN)       read interrupt flag of PIN
// PIN_FLAG_clear(PIN)      clear interrupt flag of PIN
//
// PORT functions available:
// -------------------------
// PORTA_input()            set complete PORT as input
// PORTA_output()           set complete PORT as output
// PORTA_low()              set complete PORT output value to LOW
// PORTA_high()             set complete PORT output value to HIGH
// PORTA_write(val)         write PORT output value
// PORTA_toggle()           toggle PORT output value
// PORTA_read()             read PORT input value
//
// PORTB_input()            set complete PORT as input
// PORTB_output()           set complete PORT as output
// PORTB_low()              set complete PORT output value to LOW
// PORTB_high()             set complete PORT output value to HIGH
// PORTB_write(val)         write PORT output value
// PORTB_toggle()           toggle PORT output value
// PORTB_read()             read PORT input value
//
// PORTC_input()            set complete PORT as input
// PORTC_output()           set complete PORT as output
// PORTC_low()              set complete PORT output value to LOW
// PORTC_high()             set complete PORT output value to HIGH
// PORTC_write(val)         write PORT output value
// PORTC_toggle()           toggle PORT output value
// PORTC_read()             read PORT input value
//
// PORTD_input()            set complete PORT as input
// PORTD_output()           set complete PORT as output
// PORTD_low()              set complete PORT output value to LOW
// PORTD_high()             set complete PORT output value to HIGH
// PORTD_write(val)         write PORT output value
// PORTD_toggle()           toggle PORT output value
// PORTD_read()             read PORT input value
//
// PORTE_input()            set complete PORT as input
// PORTE_output()           set complete PORT as output
// PORTE_low()              set complete PORT output value to LOW
// PORTE_high()             set complete PORT output value to HIGH
// PORTE_write(val)         write PORT output value
// PORTE_toggle()           toggle PORT output value
// PORTE_read()             read PORT input value
//
// PORTF_input()            set complete PORT as input
// PORTF_output()           set complete PORT as output
// PORTF_low()              set complete PORT output value to LOW
// PORTF_high()             set complete PORT output value to HIGH
// PORTF_write(val)         write PORT output value
// PORTF_toggle()           toggle PORT output value
// PORTF_read()             read PORT input value
//
// ADC functions available:
// ------------------------
// ADC_init()               init and enable ADC, 10-bit, single-shot, fast, VDD as reference
// ADC_enable()             enable analog-to-digital converter (ADC)
// ADC_disable()            disable ADC
// ADC_STDBY_enable()       enable ADC running in STANDBY sleep mode
// ADC_STDBY_disable()      disable ADC running in STANDBY sleep mode (*)
// ADC_set8bit()            set ADC resolution to  8-bit
// ADC_set10bit()           set ADC resolution to 10-bit (*)
// ADC_freerun()            set ADC to free-run (continuous) mode
// ADC_singleshot()         set ADC to single-shot mode (*)
// ADC_slow()               set ADC for slow conversion speed (more accurate, higher impedance)
// ADC_medium()             set ADC for medium conversion speed
// ADC_fast()               set ADC for fast conversion speed (*)
// ADC_oversample(n)        set number of accumulated samples to (1<<n), n=0: no accumulation
//
// ADC_input(PIN)           select PIN as ADC input (see Notes for details)
// ADC_input_DACREF()       set DAC reference of analog comparator (AC0.DACREF) as ADC input
// ADC_input_TEMP()         set internal temperature sensor as ADC input
// ADC_input_GND()          set ground as ADC input
//
// ADC_read()               sample and read 10-bit ADC value  (for 10-bit single-shot mode)
// ADC_read_8bit()          sample and read  8-bit ADC value  (for  8-bit single-shot mode)
// ADC_read_VDD()           sample and read VDD in mV         (for 10-bit single-shot mode)
// ADC_read_TEMP()          sample and read temperature in °C (for 10-bit single-shot mode)
// ADC_read_freerun()       read next 10-bit ADC value in free-run mode (must be started first)
// ADC_start_freerun()      set ADC to free-run (continuous) mode and start conversion
// ADC_start()              start ADC conversion
//
// ADC_REF_VDD()            select VDD as ADC reference
// ADC_REF_VREF()           select internal voltage reference as ADC reference
// ADC_REF_EXT()            select external reference on pin PD7 (VREFA) as ADC reference
// ADC_VREF_0v55()          select 0.55V as internal reference (*)
// ADC_VREF_1V1()           select 1.1V  as internal reference
// ADC_VREF_1V5()           select 1.5V  as internal reference
// ADC_VREF_2V5()           select 2.5V  as internal reference
// ADC_VREF_4V34()          select 4.34V as internal reference
// ADC_VREF_enable()        force internal reference to stay always on
// ADC_VREF_disable()       internal reference is only on when needed (*)
//
// ADC_isRunning()          check if ADC is sampling
// ADC_isReady()            check if ADC result is ready to be read
// ADC_FLAG_read()          check if ADC result is ready to be read (alias)
// ADC_FLAG_clear()         clear result ready flag (also cleared when reading result)
// ADC_INT_enable()         enable ADC result ready interrupt
// ADC_INT_disable()        disable ADC result ready interrupt (*)
//
// CMP functions available:
// ------------------------
// CMP_enable()             enable analog comparator (CMP)
// CMP_disable()            disable CMP
// CMP_STDBY_enable()       enable CMP running in STANDBY sleep mode
// CMP_STDBY_disable()      disable CMP running in STANDBY sleep mode (*)
// CMP_LP_enable()          enable CMP low-power mode (slows down reaction time)
// CMP_LP_disable()         disable CMP low-power mode (*)
// CMP_HYS_set(n)           set hysteresis (0: off (*), 1: 10mV, 2: 25mV, 3: 50mV)
//
// CMP_NEG_PD3()            set pin PD3 as negative (inverting) CMP input (*)
// CMP_NEG_PD5()            set pin PD5 as negative (inverting) CMP input
// CMP_NEG_PD7()            set pin PD7 as negative (inverting) CMP input
// CMP_NEG_DACREF()         set DAC reference as negative (inverting) CMP input
//
// CMP_POS_PD2()            set pin PD2 as positive (non-inverting) CMP input (*)
// CMP_POS_PD4()            set pin PD4 as positive (non-inverting) CMP input
// CMP_POS_PD6()            set pin PD6 as positive (non-inverting) CMP input
// CMP_POS_PD1()            set pin PD1 as positive (non-inverting) CMP input
//
// CMP_OUT_enable()         enable CMP output on pin PA7
// CMP_OUT_disable()        disable CMP output
// CMP_OUT_invert()         invert CMP output
// CMP_OUT_invertoff()      do not invert CMP output
// CMP_OUT_read()           read CMP output (0: pos < neg, 1: pos > neg; if not inverted)
//
// CMP_INT_mode(n)          set interrupt mode (0: both edges (*), 2: neg edge, 3: pos edge)
// CMP_INT_enable()         enable CMP interrupt
// CMP_INT_disable()        disable CMP interrupt (*)
// CMP_FLAG_read()          read CMP interrupt flag
// CMP_FLAG_clear()         clear CMP interrupt flag
//
// DACREF functions available (8-bit reference DAC of the analog comparator):
// --------------------------------------------------------------------------
// DACREF_set(v)            set DACREF value (Vout = VREF * v / 256)
// DACREF_VREF_0v55()       select 0.55V as DACREF reference (*)
// DACREF_VREF_1V1()        select 1.1V  as DACREF reference
// DACREF_VREF_1V5()        select 1.5V  as DACREF reference
// DACREF_VREF_2V5()        select 2.5V  as DACREF reference
// DACREF_VREF_4V34()       select 4.34V as DACREF reference
// DACREF_VREF_VDD()        select VDD   as DACREF reference
// DACREF_VREF_enable()     force DACREF reference to stay always on
// DACREF_VREF_disable()    DACREF reference is only on when needed (*)
//
// Notes:
// ------
// - (*) reset state
// - Pins used for ADC and CMP must be set with PIN_input and PIN_disable beforehand.
// - ADC input pins: PD0..PD7 (AIN0..AIN7), PE0..PE3 (AIN8..AIN11, 48-pin devices),
//   PF2..PF5 (AIN12..AIN15, PF2..PF5 not on 28-pin devices).
// - CMP inverting input pins: PD3, PD5, PD7; non-inverting: PD2, PD4, PD6, PD1;
//   output: PA7.
// - The megaAVR 0-series has no DAC with output pin and no internal reference as ADC
//   input. ADC_read_VDD() measures the DACREF of the analog comparator instead, it
//   changes reference and input settings and the DACREF settings.
// - ADC_read_TEMP() changes the reference and input settings.
// - Not all pins and functions are available for every ATmega. Refer to the datasheet
//   for more details.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// Enumerate PIN Designators (use these designators to define pins)
// ===================================================================================
enum { PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7,
       PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7,
       PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7,
       PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7,
       PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7,
       PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7 };

// ===================================================================================
// PIN Functions
// ===================================================================================
#define PIN_input(x)        (&VPORTA.DIR)[((x)&56)>>1] &= ~(1<<((x)&7))
#define PIN_output(x)       (&VPORTA.DIR)[((x)&56)>>1] |=  (1<<((x)&7))
#define PIN_pullup(x)       (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] |=  PORT_PULLUPEN_bm
#define PIN_pulloff(x)      (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] &= ~PORT_PULLUPEN_bm
#define PIN_invert(x)       (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] |=  PORT_INVEN_bm
#define PIN_invertoff(x)    (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] &= ~PORT_INVEN_bm

#define PIN_low(x)          (&VPORTA.OUT)[((x)&56)>>1] &= ~(1<<((x)&7))
#define PIN_high(x)         (&VPORTA.OUT)[((x)&56)>>1] |=  (1<<((x)&7))
#define PIN_toggle(x)       (&VPORTA.IN )[((x)&56)>>1]  =  (1<<((x)&7))
#define PIN_read(x)         ((&VPORTA.IN)[((x)&56)>>1] &   (1<<((x)&7)))
#define PIN_write(x, v)     ((v) ? (PIN_high(x)) : (PIN_low(x)))

#define PIN_enable(x)       (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] &= ~PORT_ISC_gm
#define PIN_disable(x)      (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_INPUT_DISABLE_gc
#define PIN_INT_falling(x)  (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_FALLING_gc
#define PIN_INT_rising(x)   (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_RISING_gc
#define PIN_INT_level(x)    (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_LEVEL_gc
#define PIN_INT_change(x)   (&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&56)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_BOTHEDGES_gc

#define PIN_FLAG_read(x)    ((&VPORTA.INTFLAGS)[((x)&56)>>1] &  (1<<((x)&7)))
#define PIN_FLAG_clear(x)   ( &VPORTA.INTFLAGS)[((x)&56)>>1] =  (1<<((x)&7))

// Disable all pins to save power (except PF6 if used as RESET)
static inline void PIN_all_disable(void) {
  for(uint8_t pin=0; pin<8; pin++) {
    (&PORTA.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;
    (&PORTC.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;
    (&PORTD.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;
    (&PORTF.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;
    #ifdef TCB3                                 // 40/48-pin devices
    (&PORTB.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;
    (&PORTE.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;
    #endif
  }
}

// ===================================================================================
// PORT Functions
// ===================================================================================
#define PORTA_input()       PORTA.DIR = 0x00
#define PORTA_output()      PORTA.DIR = 0xff
#define PORTA_low()         PORTA.OUT = 0x00
#define PORTA_high()        PORTA.OUT = 0xff
#define PORTA_write(v)      PORTA.OUT = (v)
#define PORTA_toggle()      PORTA.OUTTGL = 0xff
#define PORTA_read()        (PORTA.IN)

#define PORTB_input()       PORTB.DIR = 0x00
#define PORTB_output()      PORTB.DIR = 0xff
#define PORTB_low()         PORTB.OUT = 0x00
#define PORTB_high()        PORTB.OUT = 0xff
#define PORTB_write(v)      PORTB.OUT = (v)
#define PORTB_toggle()      PORTB.OUTTGL = 0xff
#define PORTB_read()        (PORTB.IN)

#define PORTC_input()       PORTC.DIR = 0x00
#define PORTC_output()      PORTC.DIR = 0xff
#define PORTC_low()         PORTC.OUT = 0x00
#define PORTC_high()        PORTC.OUT = 0xff
#define PORTC_write(v)      PORTC.OUT = (v)
#define PORTC_toggle()      PORTC.OUTTGL = 0xff
#define PORTC_read()        (PORTC.IN)

#define PORTD_input()       PORTD.DIR = 0x00
#define PORTD_output()      PORTD.DIR = 0xff
#define PORTD_low()         PORTD.OUT = 0x00
#define PORTD_high()        PORTD.OUT = 0xff
#define PORTD_write(v)      PORTD.OUT = (v)
#define PORTD_toggle()      PORTD.OUTTGL = 0xff
#define PORTD_read()        (PORTD.IN)

#define PORTE_input()       PORTE.DIR = 0x00
#define PORTE_output()      PORTE.DIR = 0xff
#define PORTE_low()         PORTE.OUT = 0x00
#define PORTE_high()        PORTE.OUT = 0xff
#define PORTE_write(v)      PORTE.OUT = (v)
#define PORTE_toggle()      PORTE.OUTTGL = 0xff
#define PORTE_read()        (PORTE.IN)

#define PORTF_input()       PORTF.DIR = 0x00
#define PORTF_output()      PORTF.DIR = 0xff
#define PORTF_low()         PORTF.OUT = 0x00
#define PORTF_high()        PORTF.OUT = 0xff
#define PORTF_write(v)      PORTF.OUT = (v)
#define PORTF_toggle()      PORTF.OUTTGL = 0xff
#define PORTF_read()        (PORTF.IN)

// ===================================================================================
// ADC Functions
// ===================================================================================

// Define ADC prescaler according to F_CPU for CLK_ADC <= 1.25MHz
#if   F_CPU > 10000000
  #define ADC_PRESC   ADC_PRESC_DIV16_gc
#elif F_CPU >  5000000
  #define ADC_PRESC   ADC_PRESC_DIV8_gc
#elif F_CPU >  2500000
  #define ADC_PRESC   ADC_PRESC_DIV4_gc
#else
  #define ADC_PRESC   ADC_PRESC_DIV2_gc
#endif

#define ADC_enable()        ADC0.CTRLA |=  ADC_ENABLE_bm
#define ADC_disable()       ADC0.CTRLA &= ~ADC_ENABLE_bm
#define ADC_STDBY_enable()  ADC0.CTRLA |=  ADC_RUNSTBY_bm
#define ADC_STDBY_disable() ADC0.CTRLA &= ~ADC_RUNSTBY_bm
#define ADC_set8bit()       ADC0.CTRLA |=  ADC_RESSEL_bm
#define ADC_set10bit()      ADC0.CTRLA &= ~ADC_RESSEL_bm
#define ADC_freerun()       ADC0.CTRLA |=  ADC_FREERUN_bm
#define ADC_singleshot()    ADC0.CTRLA &= ~ADC_FREERUN_bm
#define ADC_start()         ADC0.COMMAND = ADC_STCONV_bm
#define ADC_start_freerun() do {ADC_freerun(); ADC_FLAG_clear(); ADC_start();} while(0)

#define ADC_oversample(n)   ADC0.CTRLB  =  (n) & 0x07

#define ADC_REF_VDD()       ADC0.CTRLC = (ADC0.CTRLC & ~ADC_REFSEL_gm) | ADC_REFSEL_VDDREF_gc
#define ADC_REF_VREF()      ADC0.CTRLC = (ADC0.CTRLC & ~ADC_REFSEL_gm) | ADC_REFSEL_INTREF_gc
#define ADC_REF_EXT()       ADC0.CTRLC = (ADC0.CTRLC & ~ADC_REFSEL_gm) | ADC_REFSEL_VREFA_gc

#define ADC_VREF_enable()   VREF.CTRLB |=  VREF_ADC0REFEN_bm
#define ADC_VREF_disable()  VREF.CTRLB &= ~VREF_ADC0REFEN_bm
#define ADC_VREF_0v55()     VREF.CTRLA &= ~VREF_ADC0REFSEL_gm
#define ADC_VREF_1V1()      VREF.CTRLA = (VREF.CTRLA & ~VREF_ADC0REFSEL_gm) | VREF_ADC0REFSEL_1V1_gc
#define ADC_VREF_1V5()      VREF.CTRLA = (VREF.CTRLA & ~VREF_ADC0REFSEL_gm) | VREF_ADC0REFSEL_1V5_gc
#define ADC_VREF_2V5()      VREF.CTRLA = (VREF.CTRLA & ~VREF_ADC0REFSEL_gm) | VREF_ADC0REFSEL_2V5_gc
#define ADC_VREF_4V34()     VREF.CTRLA = (VREF.CTRLA & ~VREF_ADC0REFSEL_gm) | VREF_ADC0REFSEL_4V34_gc

#define ADC_isRunning()     (ADC0.COMMAND  &  ADC_STCONV_bm)
#define ADC_isReady()       (ADC0.INTFLAGS &  ADC_RESRDY_bm)
#define ADC_FLAG_read()     (ADC0.INTFLAGS &  ADC_RESRDY_bm)
#define ADC_FLAG_clear()    ADC0.INTFLAGS  =  ADC_RESRDY_bm
#define ADC_INT_enable()    ADC0.INTCTRL  |=  ADC_RESRDY_bm
#define ADC_INT_disable()   ADC0.INTCTRL  &= ~ADC_RESRDY_bm

// Set GPIO pin as ADC input
#define ADC_input(PIN) \
  ((PIN>=PD0)&&(PIN<=PE3) ? ( ADC0.MUXPOS = (PIN) - PD0 ) : \
  ((PIN>=PF2)&&(PIN<=PF5) ? ( ADC0.MUXPOS = (PIN) - PF2 + 12 ) : \
  (0)))

#define ADC_input_DACREF()  ADC0.MUXPOS = ADC_MUXPOS_DACREF_gc
#define ADC_input_TEMP()    ADC0.MUXPOS = ADC_MUXPOS_TEMPSENSE_gc
#define ADC_input_GND()     ADC0.MUXPOS = ADC_MUXPOS_GND_gc

#define ADC_slow()          do {ADC0.CTRLC = (ADC0.CTRLC & ~ADC_PRESC_gm) | (ADC_PRESC + 4); \
                             ADC0.SAMPCTRL = 0x1f;} while(0)
#define ADC_medium()        do {ADC0.CTRLC = (ADC0.CTRLC & ~ADC_PRESC_gm) | (ADC_PRESC + 2); \
                             ADC0.SAMPCTRL = 0x07;} while(0)
#define ADC_fast()          do {ADC0.CTRLC = (ADC0.CTRLC & ~ADC_PRESC_gm) | (ADC_PRESC + 0); \
                             ADC0.SAMPCTRL = 0x00;} while(0)

// ADC init
static inline void ADC_init(void) {
  ADC0.CTRLC  = ADC_SAMPCAP_bm                // reduced size of sampling capacitance
              | ADC_REFSEL_VDDREF_gc          // set VCC as reference
              | ADC_PRESC;                    // set prescaler
  ADC0.CTRLD  = ADC_INITDLY_DLY64_gc;         // delay to settle internal reference
  ADC0.CTRLA  = ADC_ENABLE_bm;                // enable ADC, 10 bits, single shot
}

// ADC read (10-bit, for single shot only)
static inline uint16_t ADC_read(void) {
  ADC0.COMMAND = ADC_STCONV_bm;               // start sampling
  while(ADC0.COMMAND & ADC_STCONV_bm);        // wait until sampling complete
  return ADC0.RES;                            // return sample value
}

// ADC read (8-bit, for single shot only)
static inline uint8_t ADC_read_8bit(void) {
  ADC0.COMMAND = ADC_STCONV_bm;               // start sampling
  while(ADC0.COMMAND & ADC_STCONV_bm);        // wait until sampling complete
  return ADC0.RESL;                           // return sample value
}

// ADC read (10-bit, for freerun mode, must be started first)
static inline uint16_t ADC_read_freerun(void) {
  while(~ADC0.INTFLAGS & ADC_RESRDY_bm);      // wait until sampling complete
  return ADC0.RES;                            // clear flag and return sample value
}

// Sample and read supply voltage (VDD) in millivolts (mV)
static inline uint16_t ADC_read_VDD(void) {
  uint8_t ctrla = AC0.CTRLA;                  // save comparator settings
  VREF.CTRLA = (VREF.CTRLA & ~VREF_AC0REFSEL_gm) | VREF_AC0REFSEL_1V1_gc;
  AC0.DACREF = 0xff;                          // DACREF = 1.1V * 255 / 256
  AC0.CTRLA  = ctrla | AC_ENABLE_bm;          // AC must be enabled for DACREF
  ADC_REF_VDD();                              // select VDD as ADC reference
  ADC_input_DACREF();                         // set DACREF as ADC input
  DLY_us(32);                                 // give time to settle
  uint16_t raw = ADC_read();                  // sample DACREF
  AC0.CTRLA  = ctrla;                         // restore comparator settings
  if(!raw) return 0xffff;                     // prevent division by zero
  return((1100UL * 255 * 4) / raw);           // calculate and return VDD in mV
}

// Sample and read temperature sensor in °C
static inline int16_t ADC_read_TEMP(void) {
  uint8_t ctrlc    = ADC0.CTRLC;              // save ADC settings
  uint8_t sampctrl = ADC0.SAMPCTRL;
  ADC_VREF_1V1();                             // select 1.1V internal reference
  ADC0.CTRLC = ADC_SAMPCAP_bm                 // SAMPCAP = 1 (datasheet recommendation)
             | ADC_REFSEL_INTREF_gc           // internal reference as ADC reference
             | (ADC_PRESC + 1);               // CLK_ADC <= 625kHz -> SAMPLEN >= 32us
  ADC0.SAMPCTRL = 31;                         // max sample length
  ADC_input_TEMP();                           // set temp sensor as ADC input
  DLY_us(32);                                 // give time to settle
  int32_t temp = (int32_t)ADC_read() - (int8_t)SIGROW.TEMPSENSE1; // offset correction
  temp *= SIGROW.TEMPSENSE0;                  // gain correction
  ADC0.CTRLC    = ctrlc;                      // restore ADC settings
  ADC0.SAMPCTRL = sampctrl;
  return(((temp + 0x80) >> 8) - 273);         // calculate and return temperature in °C
}

// ===================================================================================
// CMP Functions
// ===================================================================================
#define CMP_enable()        AC0.CTRLA |=  AC_ENABLE_bm
#define CMP_disable()       AC0.CTRLA &= ~AC_ENABLE_bm
#define CMP_STDBY_enable()  AC0.CTRLA |=  AC_RUNSTDBY_bm
#define CMP_STDBY_disable() AC0.CTRLA &= ~AC_RUNSTDBY_bm
#define CMP_LP_enable()     AC0.CTRLA |=  AC_LPMODE_bm
#define CMP_LP_disable()    AC0.CTRLA &= ~AC_LPMODE_bm
#define CMP_HYS_set(n)      AC0.CTRLA  =  (AC0.CTRLA & ~AC_HYSMODE_gm) | (((n)<<1)&0x06)

#define CMP_NEG_PD3()       AC0.MUXCTRLA &= ~AC_MUXNEG_gm                                // AINN0
#define CMP_NEG_PD5()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~AC_MUXNEG_gm) | AC_MUXNEG_PIN1_gc   // AINN1
#define CMP_NEG_PD7()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~AC_MUXNEG_gm) | AC_MUXNEG_PIN2_gc   // AINN2
#define CMP_NEG_DACREF()    AC0.MUXCTRLA = (AC0.MUXCTRLA & ~AC_MUXNEG_gm) | AC_MUXNEG_DACREF_gc // DACREF

#define CMP_POS_PD2()       AC0.MUXCTRLA &= ~AC_MUXPOS_gm                                // AINP0
#define CMP_POS_PD4()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~AC_MUXPOS_gm) | AC_MUXPOS_PIN1_gc   // AINP1
#define CMP_POS_PD6()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~AC_MUXPOS_gm) | AC_MUXPOS_PIN2_gc   // AINP2
#define CMP_POS_PD1()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~AC_MUXPOS_gm) | AC_MUXPOS_PIN3_gc   // AINP3

#define CMP_OUT_enable()    do {PIN_output(PA7); AC0.CTRLA |= AC_OUTEN_bm;} while(0)
#define CMP_OUT_disable()   AC0.CTRLA    &= ~AC_OUTEN_bm
#define CMP_OUT_invert()    AC0.MUXCTRLA |=  AC_INVERT_bm
#define CMP_OUT_invertoff() AC0.MUXCTRLA &= ~AC_INVERT_bm
#define CMP_OUT_read()      ((AC0.STATUS & AC_STATE_bm) >> AC_STATE_bp)

#define CMP_INT_mode(n)     AC0.CTRLA    =  (AC0.CTRLA & ~AC_INTMODE_gm) | (((n)<<4)&0x30)
#define CMP_INT_enable()    AC0.INTCTRL  =  AC_CMP_bm
#define CMP_INT_disable()   AC0.INTCTRL  =  0
#define CMP_FLAG_read()     (AC0.STATUS  &  AC_CMP_bm)
#define CMP_FLAG_clear()    AC0.STATUS   =  AC_CMP_bm

// ===================================================================================
// DACREF Functions (8-bit Reference DAC of the Analog Comparator)
// ===================================================================================
#define DACREF_set(v)         AC0.DACREF = (v)

#define DACREF_VREF_enable()  VREF.CTRLB |=  VREF_AC0REFEN_bm
#define DACREF_VREF_disable() VREF.CTRLB &= ~VREF_AC0REFEN_bm
#define DACREF_VREF_0v55()    VREF.CTRLA &= ~VREF_AC0REFSEL_gm
#define DACREF_VREF_1V1()     VREF.CTRLA = (VREF.CTRLA & ~VREF_AC0REFSEL_gm) | VREF_AC0REFSEL_1V1_gc
#define DACREF_VREF_1V5()     VREF.CTRLA = (VREF.CTRLA & ~VREF_AC0REFSEL_gm) | VREF_AC0REFSEL_1V5_gc
#define DACREF_VREF_2V5()     VREF.CTRLA = (VREF.CTRLA & ~VREF_AC0REFSEL_gm) | VREF_AC0REFSEL_2V5_gc
#define DACREF_VREF_4V34()    VREF.CTRLA = (VREF.CTRLA & ~VREF_AC0REFSEL_gm) | VREF_AC0REFSEL_4V34_gc
#define DACREF_VREF_VDD()     VREF.CTRLA = (VREF.CTRLA & ~VREF_AC0REFSEL_gm) | VREF_AC0REFSEL_AVDD_gc

#ifdef __cplusplus
}
#endif
