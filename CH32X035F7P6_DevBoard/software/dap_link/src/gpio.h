// ===================================================================================
// Basic GPIO Functions for CH32X035/X034/X033                                * v1.0 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PB0, PB1, .. - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           set PIN as INPUT (floating, no pullup/pulldown) (*)
// PIN_input_PU(PIN)        set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        set PIN as INPUT for analog peripherals (e.g. ADC)
// PIN_output(PIN)          set PIN as OUTPUT (push-pull)
// PIN_alternate(PIN)       set PIN as alternate output mode
//
// PIN_low(PIN)             set PIN output value to LOW (*)
// PIN_high(PIN)            set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            read PIN input value
// PIN_write(PIN, val)      write PIN output value (0 = LOW / 1 = HIGH)
//
// PIN interrupt and event functions available:
// --------------------------------------------
// PIN_EVT_set(PIN,TYPE)    Setup PIN event TYPE:
//                          PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH
// PIN_INT_set(PIN,TYPE)    Setup PIN interrupt TYPE:
//                          PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH
// PIN_INT_enable()         Enable PIN interrupts
// PIN_INT_disable()        Disable PIN interrupts
// PIN_INTFLAG_read(PIN)    Read interrupt flag of PIN
// PIN_INTFLAG_clear(PIN)   Clear interrupt flag of PIN
// PIN_INT_ISR { }          Pin interrupt service routine
//
// PORT functions available:
// -------------------------
// PORT_enable(PIN)         enable GPIO PORT of PIN
// PORTA_enable()           enable GPIO PORT A
// PORTB_enable()           enable GPIO PORT B
// PORTC_enable()           enable GPIO PORT C
// PORTS_enable()           enable all GPIO PORTS
//
// PORT_disable(PIN)        disable GPIO PORT of PIN
// PORTA_disable()          disable GPIO PORT A
// PORTB_disable()          disable GPIO PORT B
// PORTC_disable()          disable GPIO PORT C
// PORTS_disable()          disable all GPIO PORTS
//
// Analog-to-Digital Converter (ADC) functions available:
// ------------------------------------------------------
// ADC_init()               init and enable ADC (must be called first)
// ADC_enable()             enable ADC (power-up)
// ADC_disable()            disable ADC (power-down) (*)
// ADC_fast()               set fast mode   (fADC ~8MHz, 4 sampling cycles)
// ADC_slow()               set slow mode   (fADC ~4MHz, 11 sampling cycles) (*)
// ADC_medium()             set medium mode (fADC ~6MHz, 7 sampling cycles)
//
// ADC_input(PIN)           set PIN as ADC input
// ADC_input_VREF()         set internal voltage referece (Vref) as ADC input
//
// ADC_read()               sample and read 12-bit ADC value (0..4095)
// ADC_read_VDD()           sample and read supply voltage (VDD) in millivolts (mV)
//
// Analog Comparator (CMP) functions available:
// --------------------------------------------
// CMP_lock()               lock comparator settings (*)
// CMP_unlock()             unlock comparator settings (must be called first)
//
// CMPx_enable()            enable CMPx (x = 1..3)
// CMPx_disable()           disable CMPx (*)
// CMPx_HYS_enable()        enable CMPx hysteresis
// CMPx_HYS_disable()       disable CMPx hysteresis (*)
//
// CMP1_OUT_PA1()           set CMP1 output to pin PA1
// CMP1_OUT_T2C1()          set CMP1 output to timer2, channel1 (*)
// CMP1_NEG_PA23()          set CMP1 negative input to pin PA23
// CMP1_NEG_PC3()           set CMP1 negative input to pin PC3 (*)
// CMP1_POS_PA0()           set CMP1 positive input to pin PA0
// CMP1_POS_PC19()          set CMP1 positive input to pin PC19 (*)
//
// CMP2_OUT_PB2()           set CMP2 output to pin PB2
// CMP2_OUT_T2C2()          set CMP2 output to timer2, channel2 (*)
// CMP2_NEG_PC3()           set CMP2 negative input to pin PC3
// CMP2_NEG_PA22()          set CMP2 negative input to pin PA22 (*)
// CMP2_POS_PA11()          set CMP2 positive input to pin PA11
// CMP2_POS_PA12()          set CMP2 positive input to pin PA12 (*)
//
// CMP3_OUT_PB3()           set CMP3 output to pin PB3
// CMP3_OUT_T2C3()          set CMP3 output to timer2, channel3 (*)
// CMP3_NEG_PC3()           set CMP3 negative input to pin PC3
// CMP3_NEG_PA2()           set CMP3 negative input to pin PA2 (*)
// CMP3_POS_PA14()          set CMP3 positive input to pin PA14
// CMP3_POS_PA13()          set CMP3 positive input to pin PA13 (*)
//
// Operational Amplifier (OPA) and Programmable Gain Amplifier (PGA) functions:
// ----------------------------------------------------------------------------
// OPA_lock()               lock OPA settings (*)
// OPA_unlock()             unlock OPA settings (must be called first)
//
// OPAx_enable()            enable OPAx (x = 1..2)
// OPAx_disable()           disable OPAx (*)
//
// OPA1_OUT_PA3()           set OPA1 output to pin PA3 (ADC channel 3) (*)
// OPA1_OUT_PB5()           set OPA1 output to pin PB5
// OPA1_POS_PB0()           set OPA1 positive input to pin PB0 (O1P0)
// OPA1_POS_PB8()           set OPA1 positive input to pin PB8 (O1P1)
// OPA1_POS_PB4()           set OPA1 positive input to pin PB4 (O1P2)
// OPA1_NEG_PA6()           set OPA1 negative input to pin PA6 (O1N0, external feedback)
// OPA1_NEG_PB6()           set OPA1 negative input to pin PB6 (O1N1, external feedback)
// OPA1_NEG_OFF()           disconnect OPA1 negative input (*)
// OPA1_PGA_4X()            use OPA1 as PGA with  4x gain (no external pins)
// OPA1_PGA_8X()            use OPA1 as PGA with  8x gain (no external pins)
// OPA1_PGA_16X()           use OPA1 as PGA with 16x gain (no external pins)
// OPA1_PGA_32X()           use OPA1 as PGA with 32x gain (no external pins)
// OPA1_PGA_PA1()           use OPA1 as PGA with 16x gain, reference/resistor on PA1
//
// OPA2_OUT_PA4()           set OPA2 output to pin PA4 (ADC channel 4) (*)
// OPA2_OUT_PA2()           set OPA2 output to pin PA2 (ADC channel 2)
// OPA2_POS_PA7()           set OPA2 positive input to pin PA7 (O2P0)
// OPA2_POS_PB3()           set OPA2 positive input to pin PB3 (O2P1)
// OPA2_POS_PB7()           set OPA2 positive input to pin PB7 (O2P2)
// OPA2_NEG_PA5()           set OPA2 negative input to pin PA5 (O2N0, external feedback)
// OPA2_NEG_PB1()           set OPA2 negative input to pin PB1 (O2N1, external feedback)
// OPA2_NEG_OFF()           disconnect OPA2 negative input (*)
// OPA2_PGA_4X()            use OPA2 as PGA with  4x gain (no external pins)
// OPA2_PGA_8X()            use OPA2 as PGA with  8x gain (no external pins)
// OPA2_PGA_16X()           use OPA2 as PGA with 16x gain (no external pins)
// OPA2_PGA_32X()           use OPA2 as PGA with 32x gain (no external pins)
// OPA2_PGA_PA1()           use OPA2 as PGA with 16x gain, reference/resistor on PA1
//
// OPA1_FILT_enable()       enable 10us input filter on PA3/PB5 (OPA1 as comparator)
// OPA1_FILT_disable()      disable input filter on PA3/PB5 (*)
// OPA2_FILT_enable()       enable 10us input filter on PA4 (OPA2 as comparator)
// OPA2_FILT_disable()      disable input filter on PA4 (*)
//
// OPA_POLL_lock()          lock OPA polling/interrupt settings (*)
// OPA_POLL_unlock()        unlock OPA polling/interrupt settings
// OPA_POLL_set(us,n1,n2)   set polling interval (1..512us) and number of positive
//                          inputs polled by OPA1 (n1) and OPA2 (n2) (1..3, P0 first)
// OPAx_POLL_enable()       enable positive input polling of OPAx
// OPAx_POLL_disable()      disable positive input polling of OPAx (*)
//
// Touch Key (TK) functions available:
// -----------------------------------
// TK_init()                init and enable touch key functions (must be called first)
// TK_input(PIN)            set PIN as touch key input
// TK_read_raw()            sample touch key and return raw 12-bit value
// TK_read()                returns TRUE if touch key is pressed (raw < TK_THRESHOLD)
//
// Notes:
// ------
// - (*) default state
// - For interrupts and events: Each PIN number can only be used once simultaneously.
//   (For example, PA1 and PC1 cannot be used simultaneously, but PA1 and PC2).
// - Pins used for ADC must be set with PIN_input_AN beforehand. ADC input pins are:
//   PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PB0, PB1, PC0, PC1, PC2, PC3.
// - Internal pull-down resistors are only available on PA0..PA15 and PC16..PC17.
// - OPA and CMP settings are locked after reset. Call OPA_unlock() or CMP_unlock()
//   before using the respective functions. OPA_lock()/CMP_lock() protects the settings.
// - Pins used for OPA/CMP inputs and outputs must be set with PIN_input_AN beforehand.
// - After reset the positive input selection of the OPAs is invalid, so one of the
//   OPAx_POS_xxx() functions must be called before the OPA is enabled.
// - OPAx_NEG_xxx() disables the internal feedback resistor, OPAx_PGA_xxx() enables it
//   (required by the reference manual for PGA mode). In PGA mode the negative input
//   pins are not used. The PGA gains are approximate values.
// - ADC_input(PA3) / ADC_input(PA4) / ADC_input(PA2) can be used to sample the outputs
//   of the OPAs. PB5 (OPA1 alternative output) is not an ADC input.
// - The ADC clock is derived from F_CPU and limited to divider 2..16, so the given ADC
//   clock frequencies are only reached at F_CPU >= 8MHz (datasheet: fADC 3..8MHz).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// Enumerate PIN Designators (use these designators to define pins)
// ===================================================================================
enum{
  PA0,  PA1,  PA2,  PA3,  PA4,  PA5,  PA6,  PA7,  PA8,  PA9,  PA10, PA11, PA12, PA13, PA14, PA15,
  PA16, PA17, PA18, PA19, PA20, PA21, PA22, PA23, PA24, PA25, PA26, PA27, PA28, PA29, PA30, PA31,
  PB0,  PB1,  PB2,  PB3,  PB4,  PB5,  PB6,  PB7,  PB8,  PB9,  PB10, PB11, PB12, PB13, PB14, PB15,
  PB16, PB17, PB18, PB19, PB20, PB21, PB22, PB23, PB24, PB25, PB26, PB27, PB28, PB29, PB30, PB31,
  PC0,  PC1,  PC2,  PC3,  PC4,  PC5,  PC6,  PC7,  PC8,  PC9,  PC10, PC11, PC12, PC13, PC14, PC15,
  PC16, PC17, PC18, PC19, PC20, PC21, PC22, PC23
};

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)   ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PA8)&&(PIN<=PA15)  ? ( GPIOA->CFGHR =  (GPIOA->CFGHR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ( GPIOA->CFGXR =  (GPIOA->CFGXR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PB0)&&(PIN<=PB7)   ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PB8)&&(PIN<=PB15)  ? ( GPIOB->CFGHR =  (GPIOB->CFGHR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ( GPIOB->CFGXR =  (GPIOB->CFGXR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PC0)&&(PIN<=PC7)   ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PC8)&&(PIN<=PC15)  ? ( GPIOC->CFGHR =  (GPIOC->CFGHR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ( GPIOC->CFGXR =  (GPIOC->CFGXR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
(0))))))))))
#define PIN_input_HI PIN_input
#define PIN_input_FL PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)   ? ({GPIOA->CFGLR  =  (GPIOA->CFGLR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOA->BSHR   =  ((uint32_t)1<<((PIN)&15));      }) : \
  ((PIN>=PA8)&&(PIN<=PA15)  ? ({GPIOA->CFGHR  =  (GPIOA->CFGHR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOA->BSHR   =  ((uint32_t)1<<((PIN)&15));      }) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ({GPIOA->CFGXR  =  (GPIOA->CFGXR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOA->BSXR   =  ((uint32_t)1<<((PIN)&15));      }) : \
  ((PIN>=PB0)&&(PIN<=PB7)   ? ({GPIOB->CFGLR  =  (GPIOB->CFGLR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOB->BSHR   =  ((uint32_t)1<<((PIN)&15));      }) : \
  ((PIN>=PB8)&&(PIN<=PB15)  ? ({GPIOB->CFGHR  =  (GPIOB->CFGHR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOB->BSHR   =  ((uint32_t)1<<((PIN)&15));      }) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ({GPIOB->CFGXR  =  (GPIOB->CFGXR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOB->BSXR   =  ((uint32_t)1<<((PIN)&15));      }) : \
  ((PIN>=PC0)&&(PIN<=PC7)   ? ({GPIOC->CFGLR  =  (GPIOC->CFGLR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOC->BSHR   =  ((uint32_t)1<<((PIN)&15));      }) : \
  ((PIN>=PC8)&&(PIN<=PC15)  ? ({GPIOC->CFGHR  =  (GPIOC->CFGHR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOC->BSHR   =  ((uint32_t)1<<((PIN)&15));      }) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ({GPIOC->CFGXR  =  (GPIOC->CFGXR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOC->BSXR   =  ((uint32_t)1<<((PIN)&15));      }) : \
(0))))))))))

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)   ? ({GPIOA->CFGLR  =  (GPIOA->CFGLR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOA->BSHR   =  (((uint32_t)1<<16)<<((PIN)&15));}) : \
  ((PIN>=PA8)&&(PIN<=PA15)  ? ({GPIOA->CFGHR  =  (GPIOA->CFGHR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOA->BSHR   =  (((uint32_t)1<<16)<<((PIN)&15));}) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ({GPIOA->CFGXR  =  (GPIOA->CFGXR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOA->BSXR   =  (((uint32_t)1<<16)<<((PIN)&15));}) : \
  ((PIN>=PB0)&&(PIN<=PB7)   ? ({GPIOB->CFGLR  =  (GPIOB->CFGLR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOB->BSHR   =  (((uint32_t)1<<16)<<((PIN)&15));}) : \
  ((PIN>=PB8)&&(PIN<=PB15)  ? ({GPIOB->CFGHR  =  (GPIOB->CFGHR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOB->BSHR   =  (((uint32_t)1<<16)<<((PIN)&15));}) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ({GPIOB->CFGXR  =  (GPIOB->CFGXR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOB->BSXR   =  (((uint32_t)1<<16)<<((PIN)&15));}) : \
  ((PIN>=PC0)&&(PIN<=PC7)   ? ({GPIOC->CFGLR  =  (GPIOC->CFGLR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOC->BSHR   =  (((uint32_t)1<<16)<<((PIN)&15));}) : \
  ((PIN>=PC8)&&(PIN<=PC15)  ? ({GPIOC->CFGHR  =  (GPIOC->CFGHR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOC->BSHR   =  (((uint32_t)1<<16)<<((PIN)&15));}) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ({GPIOC->CFGXR  =  (GPIOC->CFGXR                        \
                                              & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                              |  ((uint32_t)0b1000<<(((PIN)&7)<<2));  \
                                GPIOC->BSXR   =  (((uint32_t)1<<16)<<((PIN)&15));}) : \
(0))))))))))

// ===================================================================================
// Set PIN as INPUT for analog peripherals (e.g. ADC)
// ===================================================================================
#define PIN_input_AN(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)   ? ( GPIOA->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PA8)&&(PIN<=PA15)  ? ( GPIOA->CFGHR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ( GPIOA->CFGXR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB0)&&(PIN<=PB7)   ? ( GPIOB->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB8)&&(PIN<=PB15)  ? ( GPIOB->CFGHR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ( GPIOB->CFGXR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC0)&&(PIN<=PC7)   ? ( GPIOC->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC8)&&(PIN<=PC15)  ? ( GPIOC->CFGHR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ( GPIOC->CFGXR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
(0))))))))))
#define PIN_input_AD  PIN_input_AN
#define PIN_input_ADC PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT (push-pull)
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)   ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PA8)&&(PIN<=PA15)  ? ( GPIOA->CFGHR =  (GPIOA->CFGHR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ( GPIOA->CFGXR =  (GPIOA->CFGXR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PB0)&&(PIN<=PB7)   ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PB8)&&(PIN<=PB15)  ? ( GPIOB->CFGHR =  (GPIOB->CFGHR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ( GPIOB->CFGXR =  (GPIOB->CFGXR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PC0)&&(PIN<=PC7)   ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PC8)&&(PIN<=PC15)  ? ( GPIOC->CFGHR =  (GPIOC->CFGHR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ( GPIOC->CFGXR =  (GPIOC->CFGXR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
(0))))))))))
#define PIN_output_PP PIN_output

// ===================================================================================
// Set PIN as alternate output mode
// ===================================================================================
#define PIN_alternate(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)   ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b1011<<(((PIN)&7)<<2))) : \
  ((PIN>=PA8)&&(PIN<=PA15)  ? ( GPIOA->CFGHR =  (GPIOA->CFGHR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b1011<<(((PIN)&7)<<2))) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ( GPIOA->CFGXR =  (GPIOA->CFGXR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b1011<<(((PIN)&7)<<2))) : \
  ((PIN>=PB0)&&(PIN<=PB7)   ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b1011<<(((PIN)&7)<<2))) : \
  ((PIN>=PB8)&&(PIN<=PB15)  ? ( GPIOB->CFGHR =  (GPIOB->CFGHR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b1011<<(((PIN)&7)<<2))) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ( GPIOB->CFGXR =  (GPIOB->CFGXR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b1011<<(((PIN)&7)<<2))) : \
  ((PIN>=PC0)&&(PIN<=PC7)   ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b1011<<(((PIN)&7)<<2))) : \
  ((PIN>=PC8)&&(PIN<=PC15)  ? ( GPIOC->CFGHR =  (GPIOC->CFGHR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b1011<<(((PIN)&7)<<2))) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ( GPIOC->CFGXR =  (GPIOC->CFGXR                         \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)0b1011<<(((PIN)&7)<<2))) : \
(0))))))))))

// ===================================================================================
// Set PIN output value to LOW
// ===================================================================================
#define PIN_low(PIN) \
  ((PIN>=PA0 )&&(PIN<=PA15) ? ( GPIOA->BCR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0 )&&(PIN<=PB15) ? ( GPIOB->BCR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0 )&&(PIN<=PC15) ? ( GPIOC->BCR = 1<<((PIN)&15) ) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ( GPIOA->BSXR = ((1<<16)<<((PIN)&15)) ) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ( GPIOB->BSXR = ((1<<16)<<((PIN)&15)) ) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ( GPIOC->BSXR = ((1<<16)<<((PIN)&15)) ) : \
(0)))))))

// ===================================================================================
// Set PIN output value to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  ((PIN>=PA0 )&&(PIN<=PA15) ? ( GPIOA->BSHR = 1<<((PIN)&15) ) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ( GPIOA->BSXR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0 )&&(PIN<=PB15) ? ( GPIOB->BSHR = 1<<((PIN)&15) ) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ( GPIOB->BSXR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0 )&&(PIN<=PC15) ? ( GPIOC->BSHR = 1<<((PIN)&15) ) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ( GPIOC->BSXR = 1<<((PIN)&15) ) : \
(0)))))))

// ===================================================================================
// Toggle PIN output value
// ===================================================================================
#define PIN_toggle(PIN) \
  ((PIN>=PA0)&&(PIN<=PA23) ? ( GPIOA->OUTDR ^= 1<<((PIN)&31) ) : \
  ((PIN>=PB0)&&(PIN<=PB23) ? ( GPIOB->OUTDR ^= 1<<((PIN)&31) ) : \
  ((PIN>=PC0)&&(PIN<=PC23) ? ( GPIOC->OUTDR ^= 1<<((PIN)&31) ) : \
(0))))

// ===================================================================================
// Read PIN input value (returns 0 for LOW, 1 for HIGH)
// ===================================================================================
#define PIN_read(PIN) \
  ((PIN>=PA0)&&(PIN<=PA23) ? ( (GPIOA->INDR>>((PIN)&31))&1 ) : \
  ((PIN>=PB0)&&(PIN<=PB23) ? ( (GPIOB->INDR>>((PIN)&31))&1 ) : \
  ((PIN>=PC0)&&(PIN<=PC23) ? ( (GPIOC->INDR>>((PIN)&31))&1 ) : \
(0))))

// ===================================================================================
// Write PIN output value (0 = LOW / 1 = HIGH)
// ===================================================================================
#define PIN_write(PIN, val) (val)?(PIN_high(PIN)):(PIN_low(PIN))

// ===================================================================================
// Setup PIN interrupt
// ===================================================================================
enum{PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH};

#define EXTICR1 EXTICR[0]
#define EXTICR2 EXTICR[1]

#define PIN_INT_set(PIN, TYPE) { \
  ((PIN>=PA0 )&&(PIN<=PA15) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPAEN;             \
                                AFIO->EXTICR1  &= ~((uint32_t)3<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPAEN;             \
                                AFIO->EXTICR2  &= ~((uint32_t)3<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PB0 )&&(PIN<=PB15) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPBEN;             \
                                AFIO->EXTICR1   =  (AFIO->EXTICR1                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)1<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPBEN;             \
                                AFIO->EXTICR2   =  (AFIO->EXTICR2                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)1<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PC0 )&&(PIN<=PC15) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;             \
                                AFIO->EXTICR1   =  (AFIO->EXTICR1                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)2<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;             \
                                AFIO->EXTICR2   =  (AFIO->EXTICR2                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)2<<(((PIN)&15)<<1)); }) : \
  (0))))))); \
  (TYPE & 3) ? (EXTI->INTENR |=   (uint32_t)1<<((PIN)&31)) : \
               (EXTI->INTENR &= ~((uint32_t)1<<((PIN)&31))); \
  (TYPE & 1) ? (EXTI->RTENR  |=   (uint32_t)1<<((PIN)&31)) : \
               (EXTI->RTENR  &= ~((uint32_t)1<<((PIN)&31))); \
  (TYPE & 2) ? (EXTI->FTENR  |=   (uint32_t)1<<((PIN)&31)) : \
               (EXTI->FTENR  &= ~((uint32_t)1<<((PIN)&31))); \
}

#define PIN_INT_enable()        {NVIC_EnableIRQ(EXTI7_0_IRQn);   \
                                 NVIC_EnableIRQ(EXTI15_8_IRQn);  \
                                 NVIC_EnableIRQ(EXTI25_16_IRQn); }
#define PIN_INT_disable()       {NVIC_DisableIRQ(EXTI7_0_IRQn);  \
                                 NVIC_DisableIRQ(EXTI15_8_IRQn); \
                                 NVIC_DisableIRQ(EXTI25_16_IRQn);}

#define PIN_INTFLAG_read(PIN)   (EXTI->INTFR & ((uint32_t)1 << ((PIN) & 31)))
#define PIN_INTFLAG_clear(PIN)  EXTI->INTFR = ((uint32_t)1 << ((PIN) & 31))

#define PIN_INT_ISR \
  void PIN_INT_IRQHandler(void)   __attribute__((interrupt)); \
  void EXTI7_0_IRQHandler(void)   __attribute__((alias("PIN_INT_IRQHandler"))); \
  void EXTI15_8_IRQHandler(void)  __attribute__((alias("PIN_INT_IRQHandler"))); \
  void EXTI25_16_IRQHandler(void) __attribute__((alias("PIN_INT_IRQHandler"))); \
  void PIN_INT_IRQHandler(void)

// ===================================================================================
// Setup PIN event
// ===================================================================================
enum{PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH};

#define PIN_EVT_set(PIN, TYPE) { \
  ((PIN>=PA0 )&&(PIN<=PA15) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPAEN;             \
                                AFIO->EXTICR1  &= ~((uint32_t)3<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PA16)&&(PIN<=PA23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPAEN;             \
                                AFIO->EXTICR2  &= ~((uint32_t)3<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PB0 )&&(PIN<=PB15) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPBEN;             \
                                AFIO->EXTICR1   =  (AFIO->EXTICR1                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)1<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPBEN;             \
                                AFIO->EXTICR2   =  (AFIO->EXTICR2                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)1<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PC0 )&&(PIN<=PC15) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;             \
                                AFIO->EXTICR1   =  (AFIO->EXTICR1                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)2<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;             \
                                AFIO->EXTICR2   =  (AFIO->EXTICR2                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)2<<(((PIN)&15)<<1)); }) : \
  (0))))))); \
  (TYPE & 3) ? (EXTI->EVENR |=   (uint32_t)1<<((PIN)&31)) : \
               (EXTI->EVENR &= ~((uint32_t)1<<((PIN)&31))); \
  (TYPE & 1) ? (EXTI->RTENR |=   (uint32_t)1<<((PIN)&31)) : \
               (EXTI->RTENR &= ~((uint32_t)1<<((PIN)&31))); \
  (TYPE & 2) ? (EXTI->FTENR |=   (uint32_t)1<<((PIN)&31)) : \
               (EXTI->FTENR &= ~((uint32_t)1<<((PIN)&31))); \
}

// ===================================================================================
// Enable GPIO PORTS
// ===================================================================================
#define PORTA_enable()    RCC->APB2PCENR |= RCC_IOPAEN
#define PORTB_enable()    RCC->APB2PCENR |= RCC_IOPBEN
#define PORTC_enable()    RCC->APB2PCENR |= RCC_IOPCEN
#define PORTS_enable()    RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN

#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA23) ? ( RCC->APB2PCENR |= RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB23) ? ( RCC->APB2PCENR |= RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC23) ? ( RCC->APB2PCENR |= RCC_IOPCEN ) : \
(0))))

// ===================================================================================
// Disable GPIO PORTS
// ===================================================================================
#define PORTA_disable()   RCC->APB2PCENR &= ~RCC_IOPAEN
#define PORTB_disable()   RCC->APB2PCENR &= ~RCC_IOPBEN
#define PORTC_disable()   RCC->APB2PCENR &= ~RCC_IOPCEN
#define PORTS_disable()   RCC->APB2PCENR &= ~(RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN)

#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA23) ? ( RCC->APB2PCENR &= ~RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB23) ? ( RCC->APB2PCENR &= ~RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC23) ? ( RCC->APB2PCENR &= ~RCC_IOPCEN ) : \
(0))))

// ===================================================================================
// Analog-to-Digital Converter (ADC) Functions
// ===================================================================================
#define ADC_enable()        ADC1->CTLR2  |=  ADC_ADON
#define ADC_disable()       ADC1->CTLR2  &= ~ADC_ADON

// ADC clock divider code for CTLR3 (CLK_DIV[8:0]) for a target ADC clock frequency f
#define ADC_DIV_CALC(f)     ((F_CPU + (f) - 1) / (f))
#define ADC_DIV_LIMIT(n)    ((n) < 2 ? 2 : (n) == 3 ? 4 : (n) > 16 ? 16 : (n))
#define ADC_DIV_CODE(n)     ((n) == 2 ? 0x01 : (((((n) - 2) >> 1) << 4) | ((n) - 1)))
#define ADC_CLK_CODE(f)     ADC_DIV_CODE(ADC_DIV_LIMIT(ADC_DIV_CALC(f)))

#define ADC_fast()          { ADC1->CTLR3   = (ADC1->CTLR3 & ~ADC_CTLR3_CLK_DIV)   \
                                            | ADC_CLK_CODE(8000000);              \
                              ADC1->SAMPTR1 = 0x00000000;   /*  4 cycles */       \
                              ADC1->SAMPTR2 = 0x00000000; }
#define ADC_slow()          { ADC1->CTLR3   = (ADC1->CTLR3 & ~ADC_CTLR3_CLK_DIV)   \
                                            | ADC_CLK_CODE(4000000);              \
                              ADC1->SAMPTR1 = 0x00FFFFFF;   /* 11 cycles */       \
                              ADC1->SAMPTR2 = 0x3FFFFFFF; }
#define ADC_medium()        { ADC1->CTLR3   = (ADC1->CTLR3 & ~ADC_CTLR3_CLK_DIV)   \
                                            | ADC_CLK_CODE(6000000);              \
                              ADC1->SAMPTR1 = 0x006DB6DB;   /*  7 cycles */       \
                              ADC1->SAMPTR2 = 0x1B6DB6DB; }

#define ADC_input_VREF()    ADC1->RSQR3 = 15

#define ADC_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? (ADC1->RSQR3 = (PIN)&7)       : \
  ((PIN>=PB0)&&(PIN<=PB1) ? (ADC1->RSQR3 = ((PIN)&7)+8)   : \
  ((PIN>=PC0)&&(PIN<=PC3) ? (ADC1->RSQR3 = ((PIN)&7)+10)  : \
(0))))

static inline void ADC_init(void) {
  RCC->APB2PCENR |= RCC_ADC1EN | RCC_AFIOEN;    // enable ADC and AFIO
  ADC_slow();                                   // set slow speed as default
  ADC1->CTLR2  = ADC_ADON                       // turn on ADC
               | ADC_EXTSEL                     // select software trigger (SWSTART)
               | ADC_EXTTRIG;                   // enable trigger (as WCH SDK)
}

static inline uint16_t ADC_read(void) {
  ADC1->CTLR2 |= ADC_SWSTART;                   // start conversion
  while(!(ADC1->STATR & ADC_EOC));              // wait until finished
  return ADC1->RDATAR;                          // return result
}

// Vref is about 1.2V (1.16..1.24V), use ADC_slow() for best results
static inline uint16_t ADC_read_VDD(void) {
  ADC_input_VREF();                             // set VREF as ADC input
  return((uint32_t)1200 * 4095 / ADC_read());   // return VDD in mV
}

// ===================================================================================
// Analog Comparator (CMP) Functions
// ===================================================================================
#define CMP_lock()          OPA->CTLR2 |= OPA_CTLR2_CMP_LOCK
#define CMP_unlock()        {OPA->CMPKEY = CMP_KEY1; OPA->CMPKEY = CMP_KEY2;}

#define CMP1_enable()       OPA->CTLR2 |=  OPA_CTLR2_EN1
#define CMP1_disable()      OPA->CTLR2 &= ~OPA_CTLR2_EN1
#define CMP1_HYS_enable()   OPA->CTLR2 |=  OPA_CTLR2_HYEN1
#define CMP1_HYS_disable()  OPA->CTLR2 &= ~OPA_CTLR2_HYEN1
#define CMP1_OUT_PA1()      OPA->CTLR2 |=  OPA_CTLR2_MODE1
#define CMP1_OUT_T2C1()     OPA->CTLR2 &= ~OPA_CTLR2_MODE1
#define CMP1_NEG_PA23()     OPA->CTLR2 |=  OPA_CTLR2_NSEL1
#define CMP1_NEG_PC3()      OPA->CTLR2 &= ~OPA_CTLR2_NSEL1
#define CMP1_POS_PA0()      OPA->CTLR2 |=  OPA_CTLR2_PSEL1
#define CMP1_POS_PC19()     OPA->CTLR2 &= ~OPA_CTLR2_PSEL1

#define CMP2_enable()       OPA->CTLR2 |=  OPA_CTLR2_EN2
#define CMP2_disable()      OPA->CTLR2 &= ~OPA_CTLR2_EN2
#define CMP2_HYS_enable()   OPA->CTLR2 |=  OPA_CTLR2_HYEN2
#define CMP2_HYS_disable()  OPA->CTLR2 &= ~OPA_CTLR2_HYEN2
#define CMP2_OUT_PB2()      OPA->CTLR2 |=  OPA_CTLR2_MODE2
#define CMP2_OUT_T2C2()     OPA->CTLR2 &= ~OPA_CTLR2_MODE2
#define CMP2_NEG_PC3()      OPA->CTLR2 |=  OPA_CTLR2_NSEL2
#define CMP2_NEG_PA22()     OPA->CTLR2 &= ~OPA_CTLR2_NSEL2
#define CMP2_POS_PA11()     OPA->CTLR2 |=  OPA_CTLR2_PSEL2
#define CMP2_POS_PA12()     OPA->CTLR2 &= ~OPA_CTLR2_PSEL2

#define CMP3_enable()       OPA->CTLR2 |=  OPA_CTLR2_EN3
#define CMP3_disable()      OPA->CTLR2 &= ~OPA_CTLR2_EN3
#define CMP3_HYS_enable()   OPA->CTLR2 |=  OPA_CTLR2_HYEN3
#define CMP3_HYS_disable()  OPA->CTLR2 &= ~OPA_CTLR2_HYEN3
#define CMP3_OUT_PB3()      OPA->CTLR2 |=  OPA_CTLR2_MODE3
#define CMP3_OUT_T2C3()     OPA->CTLR2 &= ~OPA_CTLR2_MODE3
#define CMP3_NEG_PC3()      OPA->CTLR2 |=  OPA_CTLR2_NSEL3
#define CMP3_NEG_PA2()      OPA->CTLR2 &= ~OPA_CTLR2_NSEL3
#define CMP3_POS_PA14()     OPA->CTLR2 |=  OPA_CTLR2_PSEL3
#define CMP3_POS_PA13()     OPA->CTLR2 &= ~OPA_CTLR2_PSEL3

// ===================================================================================
// Operational Amplifier (OPA) Functions
// ===================================================================================
#define OPA_lock()          OPA->CTLR1 |= OPA_CTLR1_OPA_LOCK
#define OPA_unlock()        {OPA->OPAKEY = OPA_KEY1; OPA->OPAKEY = OPA_KEY2;}

#define OPA1_enable()       OPA->CTLR1 |=  OPA_CTLR1_EN1
#define OPA1_disable()      OPA->CTLR1 &= ~OPA_CTLR1_EN1
#define OPA1_OUT_PA3()      OPA->CTLR1 &= ~OPA_CTLR1_MODE1
#define OPA1_OUT_PB5()      OPA->CTLR1 |=  OPA_CTLR1_MODE1
#define OPA1_POS_set(x)     OPA->CTLR1  = (OPA->CTLR1 & ~OPA_CTLR1_PSEL1) | (x)
#define OPA1_POS_PB0()      OPA1_POS_set(OPA_CTLR1_PSEL1_PB0)
#define OPA1_POS_PB8()      OPA1_POS_set(OPA_CTLR1_PSEL1_PB8)
#define OPA1_POS_PB4()      OPA1_POS_set(OPA_CTLR1_PSEL1_PB4)
#define OPA1_NEG_set(x)     OPA->CTLR1  = (OPA->CTLR1 & ~(OPA_CTLR1_NSEL1 | OPA_CTLR1_FB_EN1)) | (x)
#define OPA1_NEG_PA6()      OPA1_NEG_set(OPA_CTLR1_NSEL1_PA6)
#define OPA1_NEG_PB6()      OPA1_NEG_set(OPA_CTLR1_NSEL1_PB6)
#define OPA1_NEG_OFF()      OPA1_NEG_set(OPA_CTLR1_NSEL1_OFF)
#define OPA1_PGA_4X()       OPA1_NEG_set(OPA_CTLR1_NSEL1_PGA_4X  | OPA_CTLR1_FB_EN1)
#define OPA1_PGA_8X()       OPA1_NEG_set(OPA_CTLR1_NSEL1_PGA_8X  | OPA_CTLR1_FB_EN1)
#define OPA1_PGA_16X()      OPA1_NEG_set(OPA_CTLR1_NSEL1_PGA_16X | OPA_CTLR1_FB_EN1)
#define OPA1_PGA_32X()      OPA1_NEG_set(OPA_CTLR1_NSEL1_PGA_32X | OPA_CTLR1_FB_EN1)
#define OPA1_PGA_PA1()      OPA1_NEG_set(OPA_CTLR1_NSEL1_PA1     | OPA_CTLR1_FB_EN1)

#define OPA2_enable()       OPA->CTLR1 |=  OPA_CTLR1_EN2
#define OPA2_disable()      OPA->CTLR1 &= ~OPA_CTLR1_EN2
#define OPA2_OUT_PA4()      OPA->CTLR1 &= ~OPA_CTLR1_MODE2
#define OPA2_OUT_PA2()      OPA->CTLR1 |=  OPA_CTLR1_MODE2
#define OPA2_POS_set(x)     OPA->CTLR1  = (OPA->CTLR1 & ~OPA_CTLR1_PSEL2) | (x)
#define OPA2_POS_PA7()      OPA2_POS_set(OPA_CTLR1_PSEL2_PA7)
#define OPA2_POS_PB3()      OPA2_POS_set(OPA_CTLR1_PSEL2_PB3)
#define OPA2_POS_PB7()      OPA2_POS_set(OPA_CTLR1_PSEL2_PB7)
#define OPA2_NEG_set(x)     OPA->CTLR1  = (OPA->CTLR1 & ~(OPA_CTLR1_NSEL2 | OPA_CTLR1_FB_EN2)) | (x)
#define OPA2_NEG_PA5()      OPA2_NEG_set(OPA_CTLR1_NSEL2_PA5)
#define OPA2_NEG_PB1()      OPA2_NEG_set(OPA_CTLR1_NSEL2_PB1)
#define OPA2_NEG_OFF()      OPA2_NEG_set(OPA_CTLR1_NSEL2_OFF)
#define OPA2_PGA_4X()       OPA2_NEG_set(OPA_CTLR1_NSEL2_PGA_4X  | OPA_CTLR1_FB_EN2)
#define OPA2_PGA_8X()       OPA2_NEG_set(OPA_CTLR1_NSEL2_PGA_8X  | OPA_CTLR1_FB_EN2)
#define OPA2_PGA_16X()      OPA2_NEG_set(OPA_CTLR1_NSEL2_PGA_16X | OPA_CTLR1_FB_EN2)
#define OPA2_PGA_32X()      OPA2_NEG_set(OPA_CTLR1_NSEL2_PGA_32X | OPA_CTLR1_FB_EN2)
#define OPA2_PGA_PA1()      OPA2_NEG_set(OPA_CTLR1_NSEL2_PA1     | OPA_CTLR1_FB_EN2)

#define OPA1_FILT_enable()  {RCC->APB2PCENR |= RCC_AFIOEN; \
                             AFIO->CTLR |=  (AFIO_CTLR_PA3_FILT_EN | AFIO_CTLR_PB5_FILT_EN);}
#define OPA1_FILT_disable() AFIO->CTLR &= ~(AFIO_CTLR_PA3_FILT_EN | AFIO_CTLR_PB5_FILT_EN)
#define OPA2_FILT_enable()  {RCC->APB2PCENR |= RCC_AFIOEN; \
                             AFIO->CTLR |=  AFIO_CTLR_PA4_FILT_EN;}
#define OPA2_FILT_disable() AFIO->CTLR &= ~AFIO_CTLR_PA4_FILT_EN

#define OPA_POLL_lock()     OPA->CFGR1 |= OPA_CFGR1_POLL_LOCK
#define OPA_POLL_unlock()   {OPA->POLLKEY = POLL_KEY1; OPA->POLLKEY = POLL_KEY2;}
#define OPA_POLL_set(us, n1, n2) \
  OPA->CFGR2 = ((((us) - 1) & 0x1ff) | ((((n1) - 1) & 3) << 9) | ((((n2) - 1) & 3) << 11))
#define OPA1_POLL_enable()  OPA->CFGR1 |=  OPA_CFGR1_POLL_EN1
#define OPA1_POLL_disable() OPA->CFGR1 &= ~OPA_CFGR1_POLL_EN1
#define OPA2_POLL_enable()  OPA->CFGR1 |=  OPA_CFGR1_POLL_EN2
#define OPA2_POLL_disable() OPA->CFGR1 &= ~OPA_CFGR1_POLL_EN2

// ===================================================================================
// Touch Key (TK) Functions
// ===================================================================================
#ifndef TK_THRESHOLD
  #define TK_THRESHOLD      4088    // raw values below are detected as "pressed"
#endif
#ifndef TK_CHARGE
  #define TK_CHARGE         0x80    // charge time offset (system clock cycles)
#endif
#ifndef TK_DISCHARGE
  #define TK_DISCHARGE      0x08    // discharge time (ADC clock cycles)
#endif

#define TK_input(PIN)       ADC_input(PIN)

static inline void TK_init(void) {
  ADC_init();                       // init ADC
  ADC1->CTLR1  |= ADC_TKENABLE;     // enable touch key
}

static inline uint16_t TK_read_raw(void) {
  ADC1->IDATAR1 = TK_CHARGE;        // (TKEY1->CHGOFFSET) set charge time offset
  ADC1->RDATAR  = TK_DISCHARGE;     // (TKEY1->ACT_DCG) set discharge time and start
  while(!(ADC1->STATR & ADC_EOC));  // wait until sampling completed
  return ADC1->RDATAR;              // return sampled value
}

static inline uint8_t TK_read(void) {
  return(TK_read_raw() < TK_THRESHOLD); // return TRUE if pressed
}

#ifdef __cplusplus
};
#endif
