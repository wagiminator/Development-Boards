// ===================================================================================
// Basic GPIO Functions for STM32C011 and STM32C031                           * v1.1 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PF14, PF15 - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// PIN functions available:
// ------------------------
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
// PIN interrupt and event functions available:
// --------------------------------------------
// PIN_EVT_set(PIN,TYPE)    Setup PIN event TYPE:
//                          PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH
// PIN_INT_set(PIN,TYPE)    Setup PIN interrupt TYPE:
//                          PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH
// PIN_INT_enable(PIN)      Enable interrupt (NVIC) of the EXTI line of PIN
// PIN_INT_disable(PIN)     Disable interrupt (NVIC) of the EXTI line of PIN
// PIN_INTFLAG_read(PIN)    Read interrupt flag of PIN (rising or falling edge)
// PIN_INTFLAG_clear(PIN)   Clear interrupt flag of PIN
// PIN_INT_ISR(handler) { } Interrupt service routine, handler must match the EXTI line:
//                          EXTI0_1_IRQHandler (line 0, 1), EXTI2_3_IRQHandler (2, 3),
//                          EXTI4_15_IRQHandler (line 4..15)
//
// PORT functions available:
// -------------------------
// PORT_enable(PIN)         Enable GPIO PORT of PIN
// PORTA_enable()           Enable GPIO PORT A
// PORTB_enable()           Enable GPIO PORT B
// PORTC_enable()           Enable GPIO PORT C
// PORTD_enable()           Enable GPIO PORT D
// PORTF_enable()           Enable GPIO PORT F
// PORTS_enable()           Enable all GPIO PORTS
//
// PORT_disable(PIN)        Disable GPIO PORT of PIN
// PORTA_disable()          Disable GPIO PORT A
// PORTB_disable()          Disable GPIO PORT B
// PORTC_disable()          Disable GPIO PORT C
// PORTD_disable()          Disable GPIO PORT D
// PORTF_disable()          Disable GPIO PORT F
// PORTS_disable()          Disable all GPIO PORTS
//
// Analog-to-Digital Converter (ADC) functions available:
// ------------------------------------------------------
// ADC_init()               Init, enable and calibrate ADC (must be called first)
// ADC_enable()             Enable ADC (power-up)
// ADC_disable()            Disable ADC (power-down)
// ADC_calibrate()          Calibrate ADC (ADC must be disabled)
//
// ADC_fast()               Set fast mode   (  1.5 ADC clock cycles, least accurate)
// ADC_medium()             Set medium mode ( 79.5 ADC clock cycles, medium accurate) (*)
// ADC_slow()               Set slow mode   (160.5 ADC clock cycles, most accurate)
//
// ADC_input(PIN)           Set PIN as ADC input
// ADC_input_VREF()         Set internal voltage referece (Vref) as ADC input
// ADC_input_TEMP()         Set internal temperature sensor as ADC input
//
// ADC_read()               Sample and read 12-bit ADC value (0..4095)
// ADC_read_VDD()           Sample and read supply voltage (VDD) in millivolts (mV)
// ADC_read_TEMP()          Sample and read temperature sensor in °C
//
// Notes:
// ------
// - (*) reset state
// - Pins used for ADC must be set with PIN_input_AN beforehand. ADC input pins are:
//   PA0..PA8 (IN0..IN8), PA11..PA14 (IN11..IN14), and on the STM32C031 only
//   PB0..PB2 (IN17..IN19), PB10..PB12 (IN20..IN22). TEMP is IN9, VREFINT is IN10.
// - (*) ADC_init() sets medium mode. The ADC clock is PCLK (F_CPU <= 35MHz) or PCLK/2.
//   TEMP and VREF always use 160.5 cycles (datasheet: tS_temp >= 5us, tS_vrefint >= 4us).
// - ADC_read_TEMP() uses the factory calibration TS_CAL1 (30°C) and the typical slope
//   of 2.53mV/°C (datasheet), the STM32C0 has no second calibration point.
// - For interrupts and events: each line number can only be used once at a time
//   (e.g. PA1 and PB1 cannot be used simultaneously, but PA1 and PB2 can).
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
  PF0, PF1, PF2, PF3, PF4, PF5, PF6, PF7, PF8, PF9, PF10, PF11, PF12, PF13, PF14, PF15
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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOF->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
(0))))))
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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOF->PUPDR  =  (GPIOF->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
(0))))))

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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOF->PUPDR  =  (GPIOF->PUPDR                        \
                                             & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                             |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
(0))))))

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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER |=  ((uint32_t)0b11<<(((PIN)&15)<<1)),   \
                               GPIOF->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1))) : \
(0))))))
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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER  =   (GPIOF->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOF->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOF->OTYPER &= ~((uint32_t)1<<((PIN)&15)))        : \
(0))))))
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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER  =   (GPIOF->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOF->PUPDR  &= ~((uint32_t)0b11<<(((PIN)&15)<<1)),  \
                               GPIOF->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
(0))))))

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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->MODER  =   (GPIOF->MODER                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOF->PUPDR  =   (GPIOF->PUPDR                       \
                                             &  ~((uint32_t)0b11<<(((PIN)&15)<<1)))  \
                                             |   ((uint32_t)0b01<<(((PIN)&15)<<1)),  \
                               GPIOF->OTYPER |=  ((uint32_t)1<<((PIN)&15)))        : \
(0))))))

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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->PUPDR =  (GPIOF->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b01<<(((PIN)&15)<<1))) : \
(0))))))

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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->PUPDR =  (GPIOF->PUPDR                        \
                                            & ~((uint32_t)0b11<<(((PIN)&15)<<1)))   \
                                            |  ((uint32_t)0b10<<(((PIN)&15)<<1))) : \
(0))))))

// ===================================================================================
// Disable PULLUP/PULLDOWN resistor on PIN
// ===================================================================================
#define PIN_pulloff(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->PUPDR &= ~((uint32_t)0b11<<(((PIN)&15)<<1)) ) : \
(0))))))

// ===================================================================================
// Set PIN output type to push-pull
// ===================================================================================
#define PIN_pushpull(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->OTYPER &= ~((uint32_t)1<<((PIN)&15)) ) : \
(0))))))

// ===================================================================================
// Set PIN output type to open-drain
// ===================================================================================
#define PIN_opendrain(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->OTYPER |=  ((uint32_t)1<<((PIN)&15)) ) : \
(0))))))

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
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->OSPEEDR =  (GPIOF->OSPEEDR                         \
                                              & ~((uint32_t)   0b11<<(((PIN)&15)<<1)))   \
                                              |  ((uint32_t)((s)&3)<<(((PIN)&15)<<1))) : \
(0))))))

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
  ((PIN>=PF0)&&(PIN<=PF7)  ? ( GPIOF->AFR[0] =  (GPIOF->AFR[0]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
  ((PIN>=PF8)&&(PIN<=PF15) ? ( GPIOF->AFR[1] =  (GPIOF->AFR[1]                           \
                                             & ~((uint32_t)   0b1111<<(((PIN)&7)<<2)))   \
                                             |  ((uint32_t)((AF)&15)<<(((PIN)&7)<<2))) : \
(0)))))))))))

// ===================================================================================
// Set PIN output value to LOW
// ===================================================================================
#define PIN_low(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BRR = 1<<((PIN)&15) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->BRR = 1<<((PIN)&15) ) : \
(0))))))

// ===================================================================================
// Set PIN output value to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BSRR = 1<<((PIN)&15) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->BSRR = 1<<((PIN)&15) ) : \
(0))))))

// ===================================================================================
// Toggle PIN output value (single write to BSRR, other pins are not affected)
// ===================================================================================
#define PIN_toggle(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BSRR = (GPIOA->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BSRR = (GPIOB->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BSRR = (GPIOC->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BSRR = (GPIOD->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( GPIOF->BSRR = (GPIOF->ODR & ((uint32_t)1<<((PIN)&15))) \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
(0))))))

// ===================================================================================
// Read PIN input value (returns 0 for LOW, 1 for HIGH)
// ===================================================================================
#define PIN_read(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( (GPIOA->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( (GPIOB->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( (GPIOC->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( (GPIOD->IDR>>((PIN)&15))&1 ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( (GPIOF->IDR>>((PIN)&15))&1 ) : \
(0))))))

// ===================================================================================
// Write PIN output value (0 = LOW / 1 = HIGH)
// ===================================================================================
#define PIN_write(PIN, val) ((val)?(PIN_high(PIN)):(PIN_low(PIN)))

// ===================================================================================
// Setup PIN interrupt / event (EXTI)
// ===================================================================================
enum{PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH};
enum{PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH};

// EXTI port code of PIN (PA:0, PB:1, PC:2, PD:3, PF:5)
#define PIN_EXTI_PORT(PIN)  (((PIN)>>4) == 4 ? 5 : ((PIN)>>4))

// Connect EXTI line (PIN&15) to the port of PIN (8 bits per line in EXTI_EXTICRx)
#define PIN_EXTI_set(PIN) \
  (EXTI->EXTICR[((PIN)&15)>>2] = (EXTI->EXTICR[((PIN)&15)>>2]        \
                               & ~((uint32_t)0xff << (((PIN)&3)<<3))) \
                               |  ((uint32_t)PIN_EXTI_PORT(PIN) << (((PIN)&3)<<3)))

#define PIN_INT_set(PIN, TYPE) \
  (PIN_EXTI_set(PIN), \
  ((TYPE) & 3) ? (EXTI->IMR1  |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->IMR1  &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 1) ? (EXTI->RTSR1 |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->RTSR1 &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 2) ? (EXTI->FTSR1 |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->FTSR1 &= ~((uint32_t)1<<((PIN)&15))))

#define PIN_EVT_set(PIN, TYPE) \
  (PIN_EXTI_set(PIN), \
  ((TYPE) & 3) ? (EXTI->EMR1  |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->EMR1  &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 1) ? (EXTI->RTSR1 |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->RTSR1 &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 2) ? (EXTI->FTSR1 |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->FTSR1 &= ~((uint32_t)1<<((PIN)&15))))

// NVIC interrupt number of the EXTI line of PIN
#define PIN_IRQn(PIN) \
  ((((PIN)&15) <= 1) ? EXTI0_1_IRQn : (((PIN)&15) <= 3) ? EXTI2_3_IRQn : EXTI4_15_IRQn)

#define PIN_INT_enable(PIN)     NVIC_EnableIRQ(PIN_IRQn(PIN))
#define PIN_INT_disable(PIN)    NVIC_DisableIRQ(PIN_IRQn(PIN))

#define PIN_INTFLAG_read(PIN)   ((EXTI->RPR1 | EXTI->FPR1) & ((uint32_t)1 << ((PIN) & 15)))
#define PIN_INTFLAG_clear(PIN)  (EXTI->RPR1 = ((uint32_t)1 << ((PIN) & 15)), \
                                 EXTI->FPR1 = ((uint32_t)1 << ((PIN) & 15)))

#define PIN_INT_ISR(handler)    void handler(void) __attribute__((interrupt)); \
                                void handler(void)

// ===================================================================================
// Enable GPIO PORTS
// ===================================================================================
#define PORTA_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIOAEN)
#define PORTB_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIOBEN)
#define PORTC_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIOCEN)
#define PORTD_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIODEN)
#define PORTF_enable()   (RCC->IOPENR |= RCC_IOPENR_GPIOFEN)
#define PORTS_enable()    (RCC->IOPENR  = RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN \
                                        | RCC_IOPENR_GPIOCEN | RCC_IOPENR_GPIODEN \
                                        | RCC_IOPENR_GPIOFEN)

#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIODEN ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( RCC->IOPENR |= RCC_IOPENR_GPIOFEN ) : \
(0))))))

// ===================================================================================
// Disable GPIO PORTS
// ===================================================================================
#define PORTA_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN)
#define PORTB_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN)
#define PORTC_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIOCEN)
#define PORTD_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIODEN)
#define PORTF_disable()  (RCC->IOPENR &= ~RCC_IOPENR_GPIOFEN)
#define PORTS_disable()  (RCC->IOPENR  = 0)
#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIODEN ) : \
  ((PIN>=PF0)&&(PIN<=PF15) ? ( RCC->IOPENR &= ~RCC_IOPENR_GPIOFEN ) : \
(0))))))

// ===================================================================================
// ADC Functions
// ===================================================================================

// ADC calibration registers (engineering bytes, see datasheet)
#define ADC_VREFCAL         (*(__I uint16_t*)(0x1FFF756A))    // VREFINT at 3000mV
#define ADC_TSCAL1          (*(__I uint16_t*)(0x1FFF7568))    // TEMP at 30°C, 3000mV

// ADC calibration values
#define ADC_CVOLT           3000  // calibration voltage of VREF and TEMP sensor
#define ADC_CTEMP1          30    // calibration temperature of TSCAL1
#define ADC_SLOPE           10363 // temperature slope: 2.53mV/°C * 4096

// ADC clock (max. 35MHz, synchronous to PCLK): PCLK/1 up to 35MHz, PCLK/2 above
#if F_CPU <= 35000000
  #define ADC_CKMODE        (0b11 << ADC_CFGR2_CKMODE_Pos)  // ADC clock = PCLK
#else
  #define ADC_CKMODE        (0b01 << ADC_CFGR2_CKMODE_Pos)  // ADC clock = PCLK / 2
#endif

// Set ADC sampling time of the GPIO channels (SMP1). The internal channels TEMP (IN9)
// and VREF (IN10) always use SMP2 = 160.5 ADC clock cycles (datasheet: >= 5us/4us).
#define ADC_fast()          (ADC1->SMPR =  ADC1->SMPR & ~ADC_SMPR_SMP1)
#define ADC_medium()        (ADC1->SMPR = (ADC1->SMPR & ~ADC_SMPR_SMP1) | 0b110)
#define ADC_slow()          (ADC1->SMPR =  ADC1->SMPR |  ADC_SMPR_SMP1)

// Select ADC channel (0..22)
static inline void ADC_setChannel(uint8_t ch) {
  if(ch > 22) return;                           // invalid channel
  ADC1->ISR = ADC_ISR_CCRDY;                    // clear config ready flag
  ADC1->CHSELR = (uint32_t)1 << ch;             // set ADC channel
  while(!(ADC1->ISR & ADC_ISR_CCRDY));          // wait until configured
}

// Set GPIO pin as ADC input
#define ADC_input(PIN) ADC_setChannel( \
  ((PIN>=PA0 )&&(PIN<=PA8 ) ? ( (PIN)&15     ) : \
  ((PIN>=PA11)&&(PIN<=PA14) ? ( (PIN)&15     ) : \
  ((PIN>=PB0 )&&(PIN<=PB2 ) ? (((PIN)&7)+17  ) : \
  ((PIN>=PB10)&&(PIN<=PB12) ? (((PIN)&7)+18  ) : \
  (0xff))))))

// Set temperature sensor / VREF as ADC input
#define ADC_input_TEMP()    ADC_setChannel(9)       // temperature sensor (IN9)
#define ADC_input_VREF()    ADC_setChannel(10)      // internal reference (IN10)

// Enable ADC
static inline void ADC_enable(void) {
  ADC->CCR |= ADC_CCR_TSEN | ADC_CCR_VREFEN;    // enable TEMP and VREF
  ADC1->ISR = ADC_ISR_ADRDY;                    // clear ready flag
  do {
    ADC1->CR |= ADC_CR_ADEN;                    // enable ADC (may be ignored right
  } while(!(ADC1->ISR & ADC_ISR_ADRDY));        // after calibration -> repeat)
  DLY_us(120);                                  // TEMP sensor start-up time (max)
}

// Disable ADC
static inline void ADC_disable(void) {
  if(ADC1->CR & ADC_CR_ADSTART) {               // conversion in progress?
    ADC1->CR |= ADC_CR_ADSTP;                   // stop conversion
    while(ADC1->CR & ADC_CR_ADSTP);             // wait until stopped
  }
  if(ADC1->CR & ADC_CR_ADEN) {                  // ADC enabled?
    ADC1->CR |= ADC_CR_ADDIS;                   // disable ADC
    while(ADC1->CR & ADC_CR_ADEN);              // wait until disabled
  }
  ADC->CCR &= ~(ADC_CCR_TSEN | ADC_CCR_VREFEN); // disable TEMP and VREF
}

// Calibrate ADC (ADC must be disabled)
static inline void ADC_calibrate(void) {
  ADC1->CR |= ADC_CR_ADCAL;                     // start calibration
  while(ADC1->CR & ADC_CR_ADCAL);               // wait until finished
}

// Setup, calibrate and enable ADC
static inline void ADC_init(void) {
  RCC->APBENR2  |=  RCC_APBENR2_ADCEN;          // enable ADC module clock
  RCC->APBRSTR2 |=  RCC_APBRSTR2_ADCRST;        // reset ADC module
  RCC->APBRSTR2 &= ~RCC_APBRSTR2_ADCRST;
  ADC1->CFGR2 = ADC_CKMODE;                     // set ADC clock (ADEN must be 0)
  ADC1->CR    = ADC_CR_ADVREGEN;                // enable ADC voltage regulator
  DLY_us(20);                                   // wait until stable (tADCVREG_STUP)
  ADC_calibrate();                              // calibrate ADC
  ADC1->SMPR  = (0b110 << ADC_SMPR_SMP1_Pos)    // SMP1: medium (79.5 cycles)
              | (0b111 << ADC_SMPR_SMP2_Pos)    // SMP2: slow  (160.5 cycles)
              | ADC_SMPR_SMPSEL9                // TEMP uses SMP2
              | ADC_SMPR_SMPSEL10;              // VREF uses SMP2
  ADC_enable();                                 // enable ADC
}

// Sample and read ADC value of current input
static inline uint16_t ADC_read(void) {
  ADC1->CR |= ADC_CR_ADSTART;                   // start conversion
  while(!(ADC1->ISR & ADC_ISR_EOC));            // wait until finished
  return ADC1->DR;                              // return result (clears EOC)
}

// Sample and read supply voltage (VDD) in millivolts (mV)
static inline uint16_t ADC_read_VDD(void) {
  uint16_t val;
  ADC_input_VREF();                             // set VREF as ADC input
  val = ADC_read();
  return(val ? (uint32_t)ADC_CVOLT * ADC_VREFCAL / val : 0); // return VDD in mV
}

// Sample and read temperature sensor in °C
static inline int8_t ADC_read_TEMP(void) {
  int32_t vdd = ADC_read_VDD();                 // read current supply voltage
  ADC_input_TEMP();                             // set temp sensor as ADC input
  int32_t tdata = vdd * ADC_read() / ADC_CVOLT; // get vdd-compensated temp sensor val
  return((tdata - ADC_TSCAL1) * ADC_CVOLT / ADC_SLOPE + ADC_CTEMP1);
}

#ifdef __cplusplus
};
#endif
