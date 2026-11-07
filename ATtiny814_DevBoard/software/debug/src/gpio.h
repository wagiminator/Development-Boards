// ===================================================================================
// Basic GPIO Functions for tinyAVR 0-Series and 1-Series                     * v1.0 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PB0, PB1, .. - e.g.:
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
// ADC_input_DAC()          set DAC output as ADC input
// ADC_input_VREF()         set internal reference as ADC input
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
// CMP_NEG_PA6()            set pin PA6 as negative (inverting) CMP input (*)
// CMP_NEG_PB4()            set pin PB4 as negative (inverting) CMP input
// CMP_NEG_VREF()           set VREF of DAC as negative (inverting) CMP input
// CMP_NEG_DAC()            set output of DAC as negative (inverting) CMP input
//
// CMP_POS_PA7()            set pin PA7 as positive (non-inverting) CMP input (*)
// CMP_POS_PB1()            set pin PB1 as positive (non-inverting) CMP input
// CMP_POS_PB5()            set pin PB5 as positive (non-inverting) CMP input
// CMP_POS_PB6()            set pin PB6 as positive (non-inverting) CMP input
//
// CMP_OUT_enable()         enable CMP output on pin PA5 (PA3 on 8-pin devices)
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
// DAC functions available:
// ------------------------
// DAC_enable()             enable digital-to-analog converter (DAC)
// DAC_disable()            disable DAC
// DAC_OUT_enable()         enable DAC output on pin PA6 and disable digital input of PA6
// DAC_OUT_disable()        disable DAC output on pin PA6 (*)
// DAC_STDBY_enable()       enable DAC running in STANDBY sleep mode
// DAC_STDBY_disable()      disable DAC running in STANDBY sleep mode (*)
//
// DAC_set(v)               set 8-bit DAC output value (256 steps between 0V and VREF)
//
// DAC_VREF_0v55()          select 0.55V as DAC reference (*)
// DAC_VREF_1V1()           select 1.1V  as DAC reference
// DAC_VREF_1V5()           select 1.5V  as DAC reference
// DAC_VREF_2V5()           select 2.5V  as DAC reference
// DAC_VREF_4V34()          select 4.34V as DAC reference
// DAC_VREF_enable()        force DAC reference to stay always on
// DAC_VREF_disable()       DAC reference is only on when needed (*)
//
// Notes:
// ------
// - (*) reset state
// - Pins used for ADC and DAC must be set with PIN_input and PIN_disable beforehand. 
// - ADC input pins: PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PB0, PB1, PB4, PB5.
// - CMP inverting input pins: PA6, PB4; non-inverting: PA7, PB5, PB1, PB6; output: PA5
//   (8-pin devices: inverting PA6, non-inverting PA7, output PA3).
// - DAC output pin: PA6. DAC functions are only available on 1-series devices.
// - ADC_read_VDD() and ADC_read_TEMP() change reference and input settings.
// - Not all pins and functions are available for every ATtiny. Refer to the datasheet
//   for more details.
//
// 2021 by Stefan Wagner:   https://github.com/wagiminator

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
       PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7 };

// ===================================================================================
// PIN Functions
// ===================================================================================
#define PIN_input(x)        (&VPORTA.DIR)[((x)&24)>>1] &= ~(1<<((x)&7))
#define PIN_output(x)       (&VPORTA.DIR)[((x)&24)>>1] |=  (1<<((x)&7))
#define PIN_pullup(x)       (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] |=  PORT_PULLUPEN_bm
#define PIN_pulloff(x)      (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] &= ~PORT_PULLUPEN_bm
#define PIN_invert(x)       (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] |=  PORT_INVEN_bm
#define PIN_invertoff(x)    (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] &= ~PORT_INVEN_bm

#define PIN_low(x)          (&VPORTA.OUT)[((x)&24)>>1] &= ~(1<<((x)&7))
#define PIN_high(x)         (&VPORTA.OUT)[((x)&24)>>1] |=  (1<<((x)&7))
#define PIN_toggle(x)       (&VPORTA.IN )[((x)&24)>>1]  =  (1<<((x)&7))
#define PIN_read(x)         ((&VPORTA.IN)[((x)&24)>>1] &   (1<<((x)&7)))
#define PIN_write(x, v)     ((v) ? (PIN_high(x)) : (PIN_low(x)))

#define PIN_enable(x)       (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] &= ~PORT_ISC_gm
#define PIN_disable(x)      (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_INPUT_DISABLE_gc
#define PIN_INT_falling(x)  (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_FALLING_gc
#define PIN_INT_rising(x)   (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_RISING_gc
#define PIN_INT_level(x)    (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_LEVEL_gc
#define PIN_INT_change(x)   (&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] =                \
                            ((&PORTA.PIN0CTRL)[(((x)&24)<<2)+((x)&7)] & ~PORT_ISC_gm) \
                            | PORT_ISC_BOTHEDGES_gc

#define PIN_FLAG_read(x)    ((&VPORTA.INTFLAGS)[((x)&24)>>1] &  (1<<((x)&7)))
#define PIN_FLAG_clear(x)   ( &VPORTA.INTFLAGS)[((x)&24)>>1] =  (1<<((x)&7))

// Disable all pins to save power
static inline void PIN_all_disable(void) {
  for(uint8_t pin=0; pin<8; pin++) {
    (&PORTA.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;
    #ifdef PORTB
    (&PORTB.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;
    #endif
    #ifdef PORTC
    (&PORTC.PIN0CTRL)[pin] = PORT_ISC_INPUT_DISABLE_gc;
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

#define ADC_REF_VDD()       ADC0.CTRLC |=  (1<<4)
#define ADC_REF_VREF()      ADC0.CTRLC &= ~(1<<4)

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
  ((PIN>=PA0)&&(PIN<=PA7) ? ( ADC0.MUXPOS =     (PIN)&7  ) : \
  ((PIN>=PB0)&&(PIN<=PB1) ? ( ADC0.MUXPOS = 11-((PIN)&7) ) : \
  ((PIN>=PB4)&&(PIN<=PB5) ? ( ADC0.MUXPOS = 13-((PIN)&7) ) : \
  (0))))

#define ADC_input_DAC()     ADC0.MUXPOS = 0x1c
#define ADC_input_VREF()    ADC0.MUXPOS = 0x1d
#define ADC_input_TEMP()    ADC0.MUXPOS = 0x1e
#define ADC_input_GND()     ADC0.MUXPOS = 0x1f

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
  ADC_VREF_1V5();                             // select 1.5V internal reference
  ADC_REF_VDD();                              // select VDD as ADC reference
  ADC_input_VREF();                           // set internal reference as ADC input
  DLY_us(32);                                 // give time to settle
  uint16_t raw = ADC_read();                  // sample internal reference
  if(!raw) return 0xffff;                     // prevent division by zero
  return((1500UL * 1024) / raw);              // calculate and return VDD in mV
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

#define CMP_NEG_PA6()       AC0.MUXCTRLA &= ~0x06               // AINN0
#define CMP_NEG_PB4()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~0x06) | (1<<1)  // AINN1
#define CMP_NEG_VREF()      AC0.MUXCTRLA = (AC0.MUXCTRLA & ~0x06) | (2<<1)  // VREF
#define CMP_NEG_DAC()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~0x06) | (3<<1)  // DAC0

#define CMP_POS_PA7()       AC0.MUXCTRLA &= ~0x18               // AINP0
#define CMP_POS_PB5()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~0x18) | (1<<3)  // AINP1
#define CMP_POS_PB1()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~0x18) | (2<<3)  // AINP2
#define CMP_POS_PB6()       AC0.MUXCTRLA = (AC0.MUXCTRLA & ~0x18) | (3<<3)  // AINP3

#ifdef PORTB
  #define CMP_OUT_enable()  do {PIN_output(PA5); AC0.CTRLA |= AC_OUTEN_bm;} while(0)
#else
  #define CMP_OUT_enable()  do {PIN_output(PA3); AC0.CTRLA |= AC_OUTEN_bm;} while(0)
#endif
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
// DAC Functions
// ===================================================================================
#define DAC_enable()        DAC0.CTRLA |=  DAC_ENABLE_bm
#define DAC_disable()       DAC0.CTRLA &= ~DAC_ENABLE_bm
#define DAC_STDBY_enable()  DAC0.CTRLA |=  DAC_RUNSTDBY_bm
#define DAC_STDBY_disable() DAC0.CTRLA &= ~DAC_RUNSTDBY_bm
#define DAC_OUT_enable()    do {PIN_disable(PA6); DAC0.CTRLA |= DAC_OUTEN_bm;} while(0)
#define DAC_OUT_disable()   DAC0.CTRLA &= ~DAC_OUTEN_bm

#define DAC_set(v)          DAC0.DATA = (v)

#define DAC_VREF_enable()   VREF.CTRLB |=  VREF_DAC0REFEN_bm
#define DAC_VREF_disable()  VREF.CTRLB &= ~VREF_DAC0REFEN_bm
#define DAC_VREF_0v55()     VREF.CTRLA &= ~VREF_DAC0REFSEL_gm
#define DAC_VREF_1V1()      VREF.CTRLA = (VREF.CTRLA & ~VREF_DAC0REFSEL_gm) | VREF_DAC0REFSEL_1V1_gc
#define DAC_VREF_1V5()      VREF.CTRLA = (VREF.CTRLA & ~VREF_DAC0REFSEL_gm) | VREF_DAC0REFSEL_1V5_gc
#define DAC_VREF_2V5()      VREF.CTRLA = (VREF.CTRLA & ~VREF_DAC0REFSEL_gm) | VREF_DAC0REFSEL_2V5_gc
#define DAC_VREF_4V34()     VREF.CTRLA = (VREF.CTRLA & ~VREF_DAC0REFSEL_gm) | VREF_DAC0REFSEL_4V34_gc

#ifdef __cplusplus
};
#endif
