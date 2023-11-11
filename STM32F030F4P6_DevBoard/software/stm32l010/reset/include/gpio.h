// ===================================================================================
// Basic GPIO Functions for STM32L0xx                                         * v0.1 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PH14, PH15 - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// Functions available:
// --------------------
// PIN_input(PIN)           Set PIN as INPUT (floating, no pullup/pulldown)
// PIN_input_PU(PIN)        Set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        Set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        Set PIN as INPUT for analog peripherals (e.g. ADC) (*)
// PIN_output(PIN)          Set PIN as OUTPUT (push-pull)
// PIN_output_OD(PIN)       Set PIN as OUTPUT (open-drain)
// PIN_output_OD_PU(PIN)    Set PIN as OUTPUT (open-drain, pullup)
//
// PIN_pullup(PIN)          Enable PULLUP resistor on PIN
// PIN_pulldown(PIN)        Enable PULLDOWN resistor on PIN
// PIN_pulloff(PIN)         Disable PULLUP/PULLDOWN resistor on PIN (*)
// PIN_pushpull(PIN)        Set PIN output type to push-pull (*)
// PIN_opendrain(PIN)       Set PIN output type to open-drain
// PIN_speed(PIN, s)        Set PIN output SPEED (0: very low (*) .. 3: very high)
// PIN_alternate(PIN, AF)   Set alternate function AF (0..15) for PIN
//
// PIN_low(PIN)             Set PIN output value to LOW (*)
// PIN_high(PIN)            Set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            Read PIN input value
// PIN_write(PIN, val)      Write PIN output value (0 = LOW / 1 = HIGH)
//
// PORT_enable(PIN)         Enable GPIO PORT of PIN
// PORTA_enable()           Enable GPIO PORT A
// PORTB_enable()           Enable GPIO PORT B
// PORTC_enable()           Enable GPIO PORT C
// PORTD_enable()           Enable GPIO PORT D
// PORTE_enable()           Enable GPIO PORT E
// PORTH_enable()           Enable GPIO PORT H
// PORTS_enable()           Enable all GPIO PORTS
//
// PORT_disable(PIN)        Disable GPIO PORT of PIN
// PORTA_disable()          Disable GPIO PORT A
// PORTB_disable()          Disable GPIO PORT B
// PORTC_disable()          Disable GPIO PORT C
// PORTD_disable()          Disable GPIO PORT D
// PORTE_disable()          Disable GPIO PORT E
// PORTH_disable()          Disable GPIO PORT H
// PORTS_disable()          Disable all GPIO PORTS
//
// ADC_init()               Init, enable and calibrate ADC (must be called first)
// ADC_enable()             Enable ADC (power-up)
// ADC_disable()            Disable ADC (power-down)
// ADC_calibrate()          Calibrate ADC (ADC must be disabled)
//
// ADC_fast()               Set fast mode   (  1.5 ADC clock cycles, least accurate) (*)
// ADC_medium()             Set medium mode ( 79.5 ADC clock cycles, medium accurate)
// ADC_slow()               Set slow mode   (160.5 ADC clock cycles, most accurate)
//
// ADC_input(PIN)           Set PIN as ADC input
// ADC_input_VREF()         Set internal voltage referece (Vref) as ADC input
// ADC_input_TEMP()         Set internal temperature sensor as ADC input
//
// ADC_read()               Sample and read 12-bit ADC value (0..4095)
// ADC_read_VDD()           Sample and calculate supply voltage (VDD) in millivolts (mV)
// ADC_read_TEMP()          Sample and calculate internal temperature in °C
//
// Notes:
// ------
// - (*) reset state
// - Pins used for ADC must be set with PIN_input_AN beforehand. ADC input pins are:
//   PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PB0, PB1.
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
  PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7, PA8, PA9, PA10, PA11, PA12, PA13, PA14, PA15,
  PB0, PB1, PB2, PB3, PB4, PB5, PB6, PB7, PB8, PB9, PB10, PB11, PB12, PB13, PB14, PB15,
  PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7, PC8, PC9, PC10, PC11, PC12, PC13, PC14, PC15,
  PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15,
  PE0, PE1, PE2, PE3, PE4, PE5, PE6, PE7, PE8, PE9, PE10, PE11, PE12, PE13, PE14, PE15,
  PH0, PH1, PH2, PH3, PH4, PH5, PH6, PH7, PH8, PH9, PH10, PH11, PH12, PH13, PH14, PH15
};

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOC->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOD->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOE->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOH->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
(0)))))))
#define PIN_input_HI PIN_input
#define PIN_input_FL PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR  =  (GPIOA->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR  =  (GPIOB->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOC->PUPDR  =  (GPIOC->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOD->PUPDR  =  (GPIOD->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOE->PUPDR  =  (GPIOE->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOH->PUPDR  =  (GPIOH->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR  =  (GPIOA->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR  =  (GPIOB->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOC->PUPDR  =  (GPIOC->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOD->PUPDR  =  (GPIOD->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOE->PUPDR  =  (GPIOE->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOH->PUPDR  =  (GPIOH->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Set PIN as INPUT for analog peripherals (e.g. ADC)
// ===================================================================================
#define PIN_input_AN(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOC->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOD->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOE->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOH->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
(0)))))))
#define PIN_input_AD  PIN_input_AN
#define PIN_input_ADC PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT PUSH-PULL
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER  =   (GPIOA->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOA->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER  =   (GPIOB->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOB->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER  =   (GPIOC->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOC->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOC->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER  =   (GPIOD->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOD->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOD->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER  =   (GPIOE->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOE->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOE->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER  =   (GPIOH->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOH->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOH->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
(0)))))))
#define PIN_output_PP PIN_output

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER  =   (GPIOA->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER  =   (GPIOB->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER  =   (GPIOC->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOC->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOC->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER  =   (GPIOD->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOD->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOD->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER  =   (GPIOE->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOE->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOE->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER  =   (GPIOH->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOH->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOH->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
(0)))))))

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN PULLUP
// ===================================================================================
#define PIN_output_OD_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->MODER  =   (GPIOA->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->PUPDR  =   (GPIOA->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->MODER  =   (GPIOB->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->PUPDR  =   (GPIOB->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->MODER  =   (GPIOC->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOC->PUPDR  =   (GPIOC->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOC->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->MODER  =   (GPIOD->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOD->PUPDR  =   (GPIOD->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOD->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->MODER  =   (GPIOE->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOE->PUPDR  =   (GPIOE->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOE->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->MODER  =   (GPIOH->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOH->PUPDR  =   (GPIOH->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOH->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
(0)))))))

// ===================================================================================
// Enable PULLUP resistor on PIN
// ===================================================================================
#define PIN_pullup(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR =  (GPIOA->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR =  (GPIOB->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->PUPDR =  (GPIOC->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->PUPDR =  (GPIOD->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->PUPDR =  (GPIOE->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->PUPDR =  (GPIOH->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Enable PULLDOWN resistor on PIN
// ===================================================================================
#define PIN_pulldown(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR =  (GPIOA->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR =  (GPIOB->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->PUPDR =  (GPIOC->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->PUPDR =  (GPIOD->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->PUPDR =  (GPIOE->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->PUPDR =  (GPIOH->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Disable PULLUP/PULLDOWN resistor on PIN
// ===================================================================================
#define PIN_pulloff(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
(0)))))))

// ===================================================================================
// Set PIN output type to push-pull
// ===================================================================================
#define PIN_pushpull(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
(0)))))))

// ===================================================================================
// Set PIN output type to open-drain
// ===================================================================================
#define PIN_opendrain(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
(0)))))))

// ===================================================================================
// Set PIN output SPEED (0: very low ... 3: very high)
// ===================================================================================
#define PIN_speed(PIN, s) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OSPEEDR =  (GPIOA->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OSPEEDR =  (GPIOB->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->OSPEEDR =  (GPIOC->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->OSPEEDR =  (GPIOD->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->OSPEEDR =  (GPIOE->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->OSPEEDR =  (GPIOH->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
(0)))))))

// ===================================================================================
// Set alternate function AF (0..15) for PIN
// ===================================================================================
#define PIN_alternate(PIN, AF) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->AFR[0] =  (GPIOA->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->AFR[1] =  (GPIOA->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->AFR[0] =  (GPIOB->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->AFR[1] =  (GPIOB->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->AFR[0] =  (GPIOC->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->AFR[1] =  (GPIOC->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->AFR[0] =  (GPIOD->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->AFR[1] =  (GPIOD->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PE0)&&(PIN<=PE7)  ? ( GPIOE->AFR[0] =  (GPIOE->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PE8)&&(PIN<=PE15) ? ( GPIOE->AFR[1] =  (GPIOE->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PH0)&&(PIN<=PH7)  ? ( GPIOH->AFR[0] =  (GPIOH->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PH8)&&(PIN<=PH15) ? ( GPIOH->AFR[1] =  (GPIOH->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
(0)))))))))))))

// ===================================================================================
// Set PIN output value to LOW
// ===================================================================================
#define PIN_low(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->BRR = 1<<((PIN)&15) ) : \
(0)))))))

// ===================================================================================
// Set PIN output value to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->BSRR = 1<<((PIN)&15) ) : \
(0)))))))

// ===================================================================================
// Toggle PIN output value
// ===================================================================================
#define PIN_toggle(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->ODR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->ODR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->ODR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->ODR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( GPIOE->ODR ^= 1<<((PIN)&15) ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( GPIOH->ODR ^= 1<<((PIN)&15) ) : \
(0)))))))

// ===================================================================================
// Read PIN input value (returns 0 for LOW, 1 for HIGH)
// ===================================================================================
#define PIN_read(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( (GPIOA->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( (GPIOB->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( (GPIOC->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( (GPIOD->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( (GPIOE->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( (GPIOH->IDR>>((PIN)&15))&1 ) : \
(0)))))))

// ===================================================================================
// Write PIN output value (0 = LOW / 1 = HIGH)
// ===================================================================================
#define PIN_write(PIN, val) (val)?(PIN_high(PIN)):(PIN_low(PIN))

// ===================================================================================
// Enable GPIO PORTS
// ===================================================================================
#define PORTA_enable()    RCC->IOPENR |= RCC_IOPENR_GPIOAEN
#define PORTB_enable()    RCC->IOPENR |= RCC_IOPENR_GPIOBEN
#define PORTC_enable()    RCC->IOPENR |= RCC_IOPENR_GPIOCEN
#define PORTD_enable()    RCC->IOPENR |= RCC_IOPENR_GPIODEN
#define PORTE_enable()    RCC->IOPENR |= RCC_IOPENR_GPIOEEN
#define PORTH_enable()    RCC->IOPENR |= RCC_IOPENR_GPIOHEN
#define PORTS_enable()    RCC->IOPENR  = RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN \
                                       | RCC_IOPENR_GPIOCEN | RCC_IOPENR_GPIODEN \
                                       | RCC_IOPENR_GPIOEEN | RCC_IOPENR_GPIOHEN

#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIODEN ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOEEN ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOHEN ) : \
(0)))))))

// ===================================================================================
// Disable GPIO PORTS
// ===================================================================================
#define PORTA_disable()   RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN
#define PORTB_disable()   RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN
#define PORTC_disable()   RCC->IOPENR &= ~RCC_IOPENR_GPIOCEN
#define PORTD_disable()   RCC->IOPENR &= ~RCC_IOPENR_GPIODEN
#define PORTE_disable()   RCC->IOPENR &= ~RCC_IOPENR_GPIOEEN
#define PORTH_disable()   RCC->IOPENR &= ~RCC_IOPENR_GPIOHEN
#define PORTS_disable()   RCC->IOPENR  = 0

#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIODEN ) : \
  ((PIN>=PE0)&&(PIN<=PE15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOEEN ) : \
  ((PIN>=PH0)&&(PIN<=PH15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOHEN ) : \
(0)))))))

// ===================================================================================
// ADC Functions
// ===================================================================================

// ADC calibration registers
#define ADC_VREFCAL         (*(__I uint16_t*)(0x1FF80078))    // at 3000mV
#define ADC_TSCAL1          (*(__I uint16_t*)(0x1FF8007A))    // at 30°C
#define ADC_TSCAL2          (*(__I uint16_t*)(0x1FF8007E))    // at 130°C

// ADC calibration values
#define ADC_CVOLT           3000  // calibration voltage of VREF and TEMP sensor
#define ADC_CTEMP1          30    // calibration temperature of TSCAL1
#define ADC_CTEMP2          130   // calibration temperature of TSCAL2

// Set ADC sampling rate
#define ADC_fast()          ADC1->SMPR &= ~ADC_SMPR_SMP
#define ADC_medium()        ADC1->SMPR  = (ADC1->SMPR & ~ADC_SMPR_SMP) | 0b110
#define ADC_slow()          ADC1->SMPR |=  ADC_SMPR_SMP

// Set ADC input
#define ADC_input(PIN) \
  ((PIN>=PA0 )&&(PIN<=PA7 ) ? ( ADC1->CHSELR = (uint32_t)1<<( (PIN)&7)     ) : \
  ((PIN>=PB0 )&&(PIN<=PB1 ) ? ( ADC1->CHSELR = (uint32_t)1<<(((PIN)&7)+8)  ) : \
  (0)))
#define ADC_input_VREF()    ADC1->CHSELR = (uint16_t)1<<17  // VREF: channel 17
#define ADC_input_TEMP()    ADC1->CHSELR = (uint16_t)1<<18  // TEMP: channel 18

// Enable ADC
static inline void ADC_enable(void) {
  ADC->CCR  = ADC_CCR_VREFEN | ADC_CCR_TSEN;    // enable VREF and TEMP sensor
  ADC1->ISR = 0xffff;                           // clear all ADC flags
  ADC1->CR |= ADC_CR_ADEN;                      // enable ADC
  while(!(ADC1->ISR & ADC_ISR_ADRDY));          // wait until ready
}

// Disable ADC
static inline void ADC_disable(void) {
  if(ADC1->CR & ADC_CR_ADSTART) {               // conversion in progress?
    ADC1->CR |= ADC_CR_ADSTP;                   // stop conversion
    while(ADC1->CR & ADC_CR_ADSTP);             // wait until stopped
  }
  ADC1->CR |= ADC_CR_ADDIS;                     // disable ADC
  while(ADC1->CR & ADC_CR_ADEN);                // wait until disabled
  ADC->CCR  = 0;                                // disable VREF
}

// Calibrate ADC (ADC must be disabled)
static inline void ADC_calibrate(void) {
  ADC1->CR |= ADC_CR_ADCAL;                     // start calibration
  while(!(ADC1->ISR & ADC_ISR_EOCAL));          // wait until finished
  ADC1->ISR |= ADC_ISR_EOCAL;                   // clear flag
}

// Setup, calibrate and enable ADC
static inline void ADC_init(void) {
  RCC->APB2ENR |= RCC_APB2ENR_ADCEN;            // enable ADC module clock
  DLY_us(20);                                   // wait until stable
  ADC_calibrate();                              // calibrate ADC
  ADC_enable();                                 // enable ADC
}

// Sample and read ADC value of current input
static inline uint16_t ADC_read(void) {
  ADC1->CR |= ADC_CR_ADSTART;                   // start conversion
  while(ADC1->CR & ADC_CR_ADSTART);             // wait until finished
  return ADC1->DR;                              // return result
}

// Sample and read supply voltage (VDD) in millivolts (mV)
static inline uint16_t ADC_read_VDD(void) {
  ADC_input_VREF();                             // set VREF as ADC input
  return((uint32_t)ADC_CVOLT*ADC_VREFCAL/ADC_read()); // return VDD in mV
}

// Sample and read temperature sensor in °C
static inline int8_t ADC_read_TEMP(void) {
  int32_t vdd = ADC_read_VDD();                 // read current supply voltage
  ADC_input_TEMP();                             // set temp sensor as ADC input
  int32_t tdata = vdd*ADC_read()/ADC_CVOLT;     // get vdd-compensated temp sensor val
  return((tdata-ADC_TSCAL1)*(ADC_CTEMP2-ADC_CTEMP1)/(ADC_TSCAL2-ADC_TSCAL1)+ADC_CTEMP1);
}

#ifdef __cplusplus
};
#endif
