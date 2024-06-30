// ===================================================================================
// Basic GPIO Functions for CH32X035/X034/X033                                * v0.4 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PB0, PB1, .. - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           set PIN as INPUT (floating, no pullup/pulldown)
// PIN_input_PU(PIN)        set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        set PIN as INPUT for analog peripherals (e.g. ADC) (*)
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
// ADC_fast()               set fast mode   (fast speed, least accurate)
// ADC_slow()               set slow mode   (slow speed, most accurate) (*)
// ADC_medium()             set medium mode (medium speed, medium accurate)
//
// ADC_input(PIN)           set PIN as ADC input
// ADC_input_VREF()         set internal voltage referece (Vref) as ADC input
//
// ADC_read()               sample and read 12-bit ADC value (0..4095)
// ADC_read_VDD()           sample and read supply voltage (VDD) in millivolts (mV)
//
// Analog Comparator (CMP) functions available:
// --------------------------------------------
// CMP_lock()               lock comparators (*)
// CMP_unlock()             unlock comparators
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
// Operational Amplifier (OPA) functions available:
// ------------------------------------------------
// not yet implemented
//
// Touch Key (TK) functions available:
// -----------------------------------
// TK_init()                init and enable touch key functions (must be called first)
// TK_input(PIN)            set PIN as touch key input
// TK_read()                returns TRUE if touch key is pressed
//
// Notes:
// ------
// - (*) default state
// - For interrupts and events: Each PIN number can only be used once simultaneously.
//   (For example, PA1 and PC1 cannot be used simultaneously, but PA1 and PC2).
// - Pins used for ADC must be set with PIN_input_AN beforehand. ADC input pins are:
//   PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PB0, PB1, PC0, PC1, PC2, PC3.
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
                                                |  ((uint32_t)2<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPBEN;             \
                                AFIO->EXTICR2   =  (AFIO->EXTICR2                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)2<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PC0 )&&(PIN<=PC15) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;             \
                                AFIO->EXTICR1  |=  ((uint32_t)3<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;             \
                                AFIO->EXTICR2  |=  ((uint32_t)3<<(((PIN)&15)<<1)); }) : \
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
                                                |  ((uint32_t)2<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PB16)&&(PIN<=PB23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPBEN;             \
                                AFIO->EXTICR2   =  (AFIO->EXTICR2                       \
                                                & ~((uint32_t)3<<(((PIN)&15)<<1)))      \
                                                |  ((uint32_t)2<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PC0 )&&(PIN<=PC15) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;             \
                                AFIO->EXTICR1  |=  ((uint32_t)3<<(((PIN)&15)<<1)); }) : \
  ((PIN>=PC16)&&(PIN<=PC23) ? ({RCC->APB2PCENR |=  RCC_AFIOEN | RCC_IOPCEN;             \
                                AFIO->EXTICR2  |=  ((uint32_t)3<<(((PIN)&15)<<1)); }) : \
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
#define PORTA_enable()    RCC->APB2PCENR |= RCC_IOPAEN;
#define PORTB_enable()    RCC->APB2PCENR |= RCC_IOPBEN;
#define PORTC_enable()    RCC->APB2PCENR |= RCC_IOPCEN;
#define PORTS_enable()    RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN

#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->APB2PCENR |= RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->APB2PCENR |= RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->APB2PCENR |= RCC_IOPCEN ) : \
(0))))

// ===================================================================================
// Disable GPIO PORTS
// ===================================================================================
#define PORTA_disable()   RCC->APB2PCENR &= ~RCC_IOPAEN
#define PORTB_disable()   RCC->APB2PCENR &= ~RCC_IOPBEN
#define PORTC_disable()   RCC->APB2PCENR &= ~RCC_IOPCEN
#define PORTS_disable()   RCC->APB2PCENR &= ~(RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN)

#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->APB2PCENR &= ~RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->APB2PCENR &= ~RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->APB2PCENR &= ~RCC_IOPCEN ) : \
(0))))

// ===================================================================================
// Analog-to-Digital Converter (ADC) Functions
// ===================================================================================
#define ADC_enable()        ADC1->CTLR2  |=  ADC_ADON
#define ADC_disable()       ADC1->CTLR2  &= ~ADC_ADON

#define ADC_fast()          { ADC1->CTLR3   = 0b00000000000000000000000000000101; \
                              ADC1->SAMPTR1 = 0b00000000000000000000000000000000; \
                              ADC1->SAMPTR2 = 0b00000000000000000000000000000000; }
#define ADC_slow()          { ADC1->CTLR3   = 0b00000000000000000000000000001011; \
                              ADC1->SAMPTR1 = 0b00111111111111111111111111111111; \
                              ADC1->SAMPTR2 = 0b00111111111111111111111111111111; }
#define ADC_medium()        { ADC1->CTLR3   = 0b00000000000000000000000000001000; \
                              ADC1->SAMPTR1 = 0b00011011011011011011011011011011; \
                              ADC1->SAMPTR2 = 0b00011011011011011011011011011011; }

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
               | ADC_EXTSEL;                    // software triggering
}

static inline uint16_t ADC_read(void) {
  ADC_enable();                                 // make sure ADC is enabled
  ADC1->CTLR2 |= ADC_SWSTART;                   // start conversion
  while(!(ADC1->STATR & ADC_EOC));              // wait until finished
  return ADC1->RDATAR;                          // return result
}

static inline uint16_t ADC_read_VDD(void) {
  ADC_input_VREF();                             // set VREF as ADC input
  return((uint32_t)1200 * 4095 / ADC_read());   // return VDD in mV
}

// ===================================================================================
// Analog Comparator (CMP) Functions
// ===================================================================================
#define CMP_lock()          OPA->CTLR2 |= OPA_CTLR2_CMP_LOCK
#define CMP_unlock()        {CMP->KEY = CMP_KEY1; CMP->KEY = CMP_KEY2;}

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
// not yet implemented

// ===================================================================================
// Touch Key (TK) Functions
// ===================================================================================
#define TK_input(PIN)       ADC_input(PIN)

static inline void TK_init(void) {
  ADC_init();                       // init ADC
  ADC1->CTLR1  |= ADC_TKENABLE;     // enable touch key
  ADC1->IDATAR1 = 0x80;             // TKEY1->CHGOFFSET = 0x80;
}

static inline uint8_t TK_read(void) {
  uint8_t  result;
  uint16_t value;
  ADC_enable();                     // (re-)enable ADC
  ADC1->RDATAR = 0x08;              // (TKEY1->ACT_DCG) set discharge time and start
  while(!(ADC1->STATR & ADC_EOC));  // wait until sampling completed
  value = ADC1->RDATAR;             // read sampling value
  result = (value == 2047);         // 2047 if pressed
  ADC1->RDATAR = 0x08;              // second sampling (blind)
  while(!(ADC1->STATR & ADC_EOC));
  value = ADC1->RDATAR;
  return result;
}

#ifdef __cplusplus
};
#endif
