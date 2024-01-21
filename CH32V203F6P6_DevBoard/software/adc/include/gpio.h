// ===================================================================================
// Basic GPIO Functions for CH32V203                                          * v1.0 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PD14, PD15 - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           Set PIN as INPUT (floating, no pullup/pulldown)
// PIN_input_PU(PIN)        Set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        Set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        Set PIN as INPUT for analog peripherals (e.g. ADC)
// PIN_output(PIN)          Set PIN as OUTPUT (push-pull)
// PIN_output_OD(PIN)       Set PIN as OUTPUT (open-drain)
//
// PIN_low(PIN)             Set PIN output value to LOW
// PIN_high(PIN)            Set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            Read PIN input value
// PIN_write(PIN, val)      Write PIN output value (0 = LOW / 1 = HIGH)
//
// PORT functions available:
// -------------------------
// PORT_enable(PIN)         Enable GPIO PORT of PIN
// PORTA_enable()           Enable GPIO PORT A
// PORTB_enable()           Enable GPIO PORT B
// PORTC_enable()           Enable GPIO PORT C
// PORTD_enable()           Enable GPIO PORT D
// PORTS_enable()           Enable all GPIO PORTS
//
// PORT_disable(PIN)        Disable GPIO PORT of PIN
// PORTA_disable()          Disable GPIO PORT A
// PORTB_disable()          Disable GPIO PORT B
// PORTC_disable()          Disable GPIO PORT C
// PORTD_disable()          Disable GPIO PORT D
// PORTS_disable()          Disable all GPIO PORTS
//
// Analog-to-Digital Converter (ADC) functions available:
// ------------------------------------------------------
// ADC_init()               Init, enable and calibrate ADC (must be called first)
// ADC_enable()             enable ADC (power-up)
// ADC_disable()            disable ADC (power-down)
// ADC_fast()               set fast mode   (fast speed, least accurate)
// ADC_slow()               set slow mode   (slow speed, most accurate)
// ADC_medium()             set medium mode (medium speed, medium accurate, default)
//
// ADC_input(PIN)           Set PIN as ADC input
// ADC_input_VREF()         Set internal voltage referece (Vref) as ADC input
// ADC_input_TEMP()         Set internal temperature sensor as ADC input
//
// ADC_read()               Sample and read 12-bit ADC value (0..4095)
// ADC_read_VDD()           Sample and read supply voltage (VDD) in millivolts (mV)
// ADC_read_TEMP()          Sample and read temperature sensor in °C
//
// Op-Amp Comparator (OPA) functions available:
// --------------------------------------------
// OPA1_enable()            Enable OPA1 comparator
// OPA1_disable()           Disable OPA1 comparator
// OPA1_negative(PIN)       Set OPA1 inverting input PIN (PA6 or PB11)
// OPA1_positive(PIN)       Set OPA1 non-inverting input PIN (PB0 or PB15)
// OPA1_output(PIN)         Enable OPA1 output (push-pull) on PIN (PA3 or PB1)
// OPA1_output_OD(PIN)      Enable OPA1 output (open-drain) on PIN (PA3 or PB1)
//
// OPA2_enable()            Enable OPA2 comparator
// OPA2_disable()           Disable OPA2 comparator
// OPA2_negative(PIN)       Set OPA2 inverting input PIN (PA6 or PB11)
// OPA2_positive(PIN)       Set OPA2 non-inverting input PIN (PB0 or PB15)
// OPA2_output(PIN)         Enable OPA2 output (push-pull) on PIN (PA3 or PB1)
// OPA2_output_OD(PIN)      Enable OPA2 output (open-drain) on PIN (PA3 or PB1)
//
// Notes:
// ------
// - Pins used for ADC must be set with PIN_input_AN beforehand. ADC input pins are:
//   PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PB0, PB1, PC0, PC1, PC2, PC3, PC4, PC5.
// - Pins used as input for OPA comparators must be set with PIN_input_AN beforehand.
//   OPA:                               OPA1            OPA2
//   Negative (inverting) input:        PA6 or PB11     PA5 or PB10
//   Positive (non-inverting) input:    PB0 or PB15     PA7 or PB14
//   Output:                            PA3 or PB1      PA2 or PA4
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32v203.h"

// ===================================================================================
// Enumerate PIN Designators (use these designators to define pins)
// ===================================================================================
enum{
  PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
  PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
  PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
  PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15
};

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR =  (GPIOA->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR =  (GPIOB->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR =  (GPIOC->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR =  (GPIOD->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR =  (GPIOD->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0100<<(((PIN)&7)<<2))) : \
(0)))))))))
#define PIN_input_HI PIN_input
#define PIN_input_FL PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR  =  (GPIOA->CFGLR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOA->OUTDR |=  ((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR  =  (GPIOA->CFGHR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOA->OUTDR |=  ((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR  =  (GPIOB->CFGLR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOB->OUTDR |=  ((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR  =  (GPIOB->CFGHR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOB->OUTDR |=  ((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR  =  (GPIOC->CFGLR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOC->OUTDR |=  ((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR  =  (GPIOC->CFGHR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOC->OUTDR |=  ((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR  =  (GPIOD->CFGLR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOD->OUTDR |=  ((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR  =  (GPIOD->CFGHR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOD->OUTDR |=  ((uint32_t)1<<((PIN)&15))        ) : \
(0)))))))))

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR  =  (GPIOA->CFGLR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOA->OUTDR &= ~((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR  =  (GPIOA->CFGHR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOA->OUTDR &= ~((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR  =  (GPIOB->CFGLR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOB->OUTDR &= ~((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR  =  (GPIOB->CFGHR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOB->OUTDR &= ~((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR  =  (GPIOC->CFGLR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOC->OUTDR &= ~((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR  =  (GPIOC->CFGHR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOC->OUTDR &= ~((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR  =  (GPIOD->CFGLR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOD->OUTDR &= ~((uint32_t)1<<((PIN)&15))        ) : \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR  =  (GPIOD->CFGHR                        \
                                             & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))  \
                                             |  ((uint32_t)0b1000<<(((PIN)&7)<<2)),  \
                               GPIOD->OUTDR &= ~((uint32_t)1<<((PIN)&15))        ) : \
(0)))))))))

// ===================================================================================
// Set PIN as INPUT for analog peripherals (e.g. ADC)
// ===================================================================================
#define PIN_input_AN(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR &= ~((uint32_t)0b1111<<(((PIN)&7)<<2)) ) : \
(0)))))))))
#define PIN_input_AD  PIN_input_AN
#define PIN_input_ADC PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT (push-pull, maximum speed 50MHz)
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR =  (GPIOA->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR =  (GPIOB->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR =  (GPIOC->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR =  (GPIOD->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR =  (GPIOD->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0011<<(((PIN)&7)<<2))) : \
(0)))))))))
#define PIN_output_PP PIN_output

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN (maximum speed 50MHz)
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR =  (GPIOA->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0111<<(((PIN)&7)<<2))) : \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR =  (GPIOA->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0111<<(((PIN)&7)<<2))) : \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR =  (GPIOB->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0111<<(((PIN)&7)<<2))) : \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR =  (GPIOB->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0111<<(((PIN)&7)<<2))) : \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR =  (GPIOC->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0111<<(((PIN)&7)<<2))) : \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR =  (GPIOC->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0111<<(((PIN)&7)<<2))) : \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR =  (GPIOD->CFGLR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0111<<(((PIN)&7)<<2))) : \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR =  (GPIOD->CFGHR                         \
                                            & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))   \
                                            |  ((uint32_t)0b0111<<(((PIN)&7)<<2))) : \
(0)))))))))

// ===================================================================================
// Set PIN output value to LOW
// ===================================================================================
#define PIN_low(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BCR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BCR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BCR = 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BCR = 1<<((PIN)&15) ) : \
(0)))))

// ===================================================================================
// Set PIN output value to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BSHR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BSHR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BSHR = 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BSHR = 1<<((PIN)&15) ) : \
(0)))))

// ===================================================================================
// Toggle PIN output value
// ===================================================================================
#define PIN_toggle(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OUTDR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OUTDR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->OUTDR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->OUTDR ^= 1<<((PIN)&15) ) : \
(0)))))

// ===================================================================================
// Read PIN input value (returns 0 for LOW, 1 for HIGH)
// ===================================================================================
#define PIN_read(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( (GPIOA->INDR>>((PIN)&15))&1 ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( (GPIOB->INDR>>((PIN)&15))&1 ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( (GPIOC->INDR>>((PIN)&15))&1 ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( (GPIOD->INDR>>((PIN)&15))&1 ) : \
(0)))))

// ===================================================================================
// Write PIN output value (0 = LOW / 1 = HIGH)
// ===================================================================================
#define PIN_write(PIN, val) (val)?(PIN_high(PIN)):(PIN_low(PIN))

// ===================================================================================
// Enable GPIO PORTS
// ===================================================================================
#define PORTA_enable()    RCC->APB2PCENR |= RCC_IOPAEN;
#define PORTB_enable()    RCC->APB2PCENR |= RCC_IOPBEN;
#define PORTC_enable()    RCC->APB2PCENR |= RCC_IOPCEN;
#define PORTD_enable()    RCC->APB2PCENR |= RCC_IOPDEN;
#define PORTS_enable()    RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_IOPDEN

#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->APB2PCENR |= RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->APB2PCENR |= RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->APB2PCENR |= RCC_IOPCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->APB2PCENR |= RCC_IOPDEN ) : \
(0)))))

// ===================================================================================
// Disable GPIO PORTS
// ===================================================================================
#define PORTA_disable()   RCC->APB2PCENR &= ~RCC_IOPAEN
#define PORTB_disable()   RCC->APB2PCENR &= ~RCC_IOPBEN
#define PORTC_disable()   RCC->APB2PCENR &= ~RCC_IOPCEN
#define PORTD_disable()   RCC->APB2PCENR &= ~RCC_IOPDEN
#define PORTS_disable()   RCC->APB2PCENR &= ~(RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_IOPDEN)

#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->APB2PCENR &= ~RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->APB2PCENR &= ~RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->APB2PCENR &= ~RCC_IOPCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->APB2PCENR &= ~RCC_IOPDEN ) : \
(0)))))

// ===================================================================================
// ADC Functions
// ===================================================================================
#define ADC_enable()        ADC1->CTLR2  |=   ADC_ADON | ADC_TSVREFE
#define ADC_disable()       ADC1->CTLR2  &= ~(ADC_ADON | ADC_TSVREFE)

#define ADC_fast()          { ADC1->SAMPTR1 = 0b00000000000000000000000000000000; \
                              ADC1->SAMPTR2 = 0b00000000000000000000000000000000; }
#define ADC_slow()          { ADC1->SAMPTR1 = 0b00111111111111111111111111111111; \
                              ADC1->SAMPTR2 = 0b00111111111111111111111111111111; }
#define ADC_medium()        { ADC1->SAMPTR1 = 0b00110110110110110110110110110110; \
                              ADC1->SAMPTR2 = 0b00110110110110110110110110110110; }

#define ADC_input_TEMP()    ADC1->RSQR3 = 16
#define ADC_input_VREF()    ADC1->RSQR3 = 17

#define ADC_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? (ADC1->RSQR3 = (PIN)&7)       : \
  ((PIN>=PB0)&&(PIN<=PB1) ? (ADC1->RSQR3 = ((PIN)&7)+8)   : \
  ((PIN>=PC0)&&(PIN<=PC5) ? (ADC1->RSQR3 = ((PIN)&7)+10)  : \
(0))))

static inline void ADC_init(void) {
  RCC->APB2PCENR |= RCC_ADC1EN | RCC_AFIOEN;    // enable ADC and AFIO
  #if F_CPU > 84000000                          // set prescaler for max 14 MHz
  RCC->CTLR |= RCC_ADCPRE_DIV8;
  #elif F_CPU > 56000000
  RCC->CTLR |= RCC_ADCPRE_DIV6;
  #elif F_CPU > 28000000
  RCC->CTLR |= RCC_ADCPRE_DIV4;
  #endif
  ADC_medium();                                 // set medium speed as default
  ADC1->CTLR2  = ADC_ADON                       // turn on ADC
               | ADC_EXTSEL                     // software triggering
               | ADC_TSVREFE;                   // enable TEMP + VREF
  ADC1->CTLR2 |= ADC_RSTCAL;                    // reset calibration
  while(ADC1->CTLR2 & ADC_RSTCAL);              // wait until finished
  ADC1->CTLR2 |= ADC_CAL;                       // start calibration
  while(ADC1->CTLR2 & ADC_CAL);                 // wait until finished
}

static inline uint16_t ADC_read(void) {
  ADC_enable();                                 // make sure ADC is enabled
  ADC1->CTLR2 |= ADC_SWSTART;                   // start conversion
  while(!(ADC1->STATR & ADC_EOC));              // wait until finished
  return ADC1->RDATAR;                          // return result
}

static inline uint16_t ADC_read_VDD(void) {
  ADC_input_VREF();                             // set VREF as ADC input
  return((uint32_t)1200 * 4096 / ADC_read());   // return VDD in mV
}

static inline int8_t ADC_read_TEMP(void) {
  ADC_input_TEMP();                             // set temp sensor as ADC input
  return(((int32_t)ADC_read()*33000/4096-14000)/43+25);  // return temp in °C
}

// ===================================================================================
// OPA Functions
// ===================================================================================
#define OPA1_enable()       OPA->CTLR |=  OPA_CTLR_EN1
#define OPA1_disable()      OPA->CTLR &= ~OPA_CTLR_EN1
#define OPA2_enable()       OPA->CTLR |=  OPA_CTLR_EN2
#define OPA2_disable()      OPA->CTLR &= ~OPA_CTLR_EN2

#define OPA1_negative(PIN) \
  (PIN == PB11 ? (OPA->CTLR &= ~OPA_CTLR_NSEL1) : \
  (PIN == PA6  ? (OPA->CTLR |=  OPA_CTLR_NSEL1) : \
(0)))

#define OPA1_positive(PIN) \
  (PIN == PB15 ? (OPA->CTLR &= ~OPA_CTLR_PSEL1) : \
  (PIN == PB0  ? (OPA->CTLR |=  OPA_CTLR_PSEL1) : \
(0)))

#define OPA1_output(PIN) \
  (PIN == PA3  ? ( RCC->APB2PCENR |=  RCC_AFIOEN,                                 \
                   GPIOA->CFGLR    = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(3<<2))) \
                                                   |  ((uint32_t)0b1011<<(3<<2)), \
                   OPA->CTLR      &= ~OPA_CTLR_MODE1                          ) : \
  (PIN == PB1  ? ( RCC->APB2PCENR |=  RCC_AFIOEN,                                 \
                   GPIOB->CFGLR    = (GPIOB->CFGLR & ~((uint32_t)0b1111<<(1<<2))) \
                                                   |  ((uint32_t)0b1011<<(1<<2)), \
                   OPA->CTLR      |=  OPA_CTLR_MODE1                          ) : \
(0)))

#define OPA1_output_OD(PIN) \
  (PIN == PA3  ? ( RCC->APB2PCENR |=  RCC_AFIOEN,          \
                   GPIOA->CFGLR   |=  0b1111<<(3<<2),      \
                   OPA->CTLR      &= ~OPA_CTLR_MODE1   ) : \
  (PIN == PB1  ? ( RCC->APB2PCENR |=  RCC_AFIOEN,          \
                   GPIOB->CFGLR   |=  0b1111<<(1<<2),      \
                   OPA->CTLR      |=  OPA_CTLR_MODE1   ) : \
(0)))

#define OPA2_negative(PIN) \
  (PIN == PB10 ? (OPA->CTLR &= ~OPA_CTLR_NSEL1) : \
  (PIN == PA5  ? (OPA->CTLR |=  OPA_CTLR_NSEL1) : \
(0)))

#define OPA2_positive(PIN) \
  (PIN == PB14 ? (OPA->CTLR &= ~OPA_CTLR_PSEL1) : \
  (PIN == PA7  ? (OPA->CTLR |=  OPA_CTLR_PSEL1) : \
(0)))

#define OPA2_output(PIN) \
  (PIN == PA2  ? ( RCC->APB2PCENR |=  RCC_AFIOEN,                                 \
                   GPIOA->CFGLR    = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(2<<2))) \
                                                   |  ((uint32_t)0b1011<<(2<<2)), \
                   OPA->CTLR      &= ~OPA_CTLR_MODE1                          ) : \
  (PIN == PA4  ? ( RCC->APB2PCENR |=  RCC_AFIOEN,                                 \
                   GPIOA->CFGLR    = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(4<<2))) \
                                                   |  ((uint32_t)0b1011<<(4<<2)), \
                   OPA->CTLR      |=  OPA_CTLR_MODE1                          ) : \
(0)))

#define OPA2_output_OD(PIN) \
  (PIN == PA2  ? ( RCC->APB2PCENR |=  RCC_AFIOEN,          \
                   GPIOA->CFGLR   |=  0b1111<<(2<<2),      \
                   OPA->CTLR      &= ~OPA_CTLR_MODE1   ) : \
  (PIN == PA4  ? ( RCC->APB2PCENR |=  RCC_AFIOEN,          \
                   GPIOA->CFGLR   |=  0b1111<<(4<<2),      \
                   OPA->CTLR      |=  OPA_CTLR_MODE1   ) : \
(0)))

#define OPA1_output_PP      OPA1_output
#define OPA2_output_PP      OPA2_output

// ===================================================================================
// CMP Functions (alias)
// ===================================================================================
#define CMP1_enable         OPA1_enable
#define CMP1_disable        OPA1_disable
#define CMP1_negative       OPA1_negative
#define CMP1_positive       OPA1_positive
#define CMP1_output         OPA1_output
#define CMP1_output_PP      OPA1_output_PP
#define CMP1_output_OD      OPA1_output_OD

#define CMP2_enable         OPA2_enable
#define CMP2_disable        OPA2_disable
#define CMP2_negative       OPA2_negative
#define CMP2_positive       OPA2_positive
#define CMP2_output         OPA2_output
#define CMP2_output_PP      OPA2_output_PP
#define CMP2_output_OD      OPA2_output_OD

#ifdef __cplusplus
};
#endif
