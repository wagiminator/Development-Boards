// ===================================================================================
// Basic GPIO Functions for CH32V203                                          * v1.1 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PD14, PD15 - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           Set PIN as INPUT (floating, no pullup/pulldown) (*)
// PIN_input_PU(PIN)        Set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        Set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        Set PIN as INPUT for analog peripherals (e.g. ADC, OPA)
// PIN_output(PIN)          Set PIN as OUTPUT (push-pull, 50MHz)
// PIN_output_OD(PIN)       Set PIN as OUTPUT (open-drain, 50MHz)
// PIN_alternate(PIN)       Set PIN as alternate function output (push-pull, 50MHz)
// PIN_alternate_OD(PIN)    Set PIN as alternate function output (open-drain, 50MHz)
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
// PIN_INTFLAG_read(PIN)    Read interrupt flag of PIN
// PIN_INTFLAG_clear(PIN)   Clear interrupt flag of PIN
// PIN_INT_ISR(handler) { } Interrupt service routine, handler must match the EXTI line:
//                          EXTI0_IRQHandler .. EXTI4_IRQHandler (line 0..4),
//                          EXTI9_5_IRQHandler (line 5..9), EXTI15_10_IRQHandler (10..15)
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
// ADC_init()               Init, calibrate and enable ADC (must be called first)
// ADC_enable()             Enable ADC (power-up)
// ADC_disable()            Disable ADC (power-down)
// ADC_calibrate()          Calibrate ADC (determines offset correction, see notes)
// ADC_fast()               Set fast mode   (  1.5 ADC cycles sampling, least accurate)
// ADC_medium()             Set medium mode ( 71.5 ADC cycles sampling, default)
// ADC_slow()               Set slow mode   (239.5 ADC cycles sampling, most accurate)
//
// ADC_input(PIN)           Set PIN as ADC input
// ADC_input_VREF()         Set internal voltage referece (Vref) as ADC input
// ADC_input_TEMP()         Set internal temperature sensor as ADC input
//
// ADC_read()               Sample and read 12-bit ADC value (0..4095)
// ADC_read_VDD()           Sample and read supply voltage (VDD) in millivolts (mV)
// ADC_read_TEMP()          Sample and read temperature sensor in °C
//
// Operational Amplifier (OPA) functions available:
// ------------------------------------------------
// OPAx_enable()            Enable OPAx (x = 1, 2)
// OPAx_disable()           Disable OPAx (*)
// OPAx_negative(PIN)       Set OPAx inverting input PIN (see notes)
// OPAx_positive(PIN)       Set OPAx non-inverting input PIN (see notes)
// OPAx_output(PIN)         Set OPAx output PIN (see notes)
//
// Notes:
// ------
// - (*) reset state
// - Pins used for ADC must be set with PIN_input_AN beforehand. ADC input pins are:
//   PA0..PA7 (IN0..7), PB0, PB1 (IN8, 9), PC0..PC5 (IN10..15, only available on
//   the CH32V203RB). The temperature sensor is IN16, VREFINT is IN17.
// - The ADC clock is PCLK2/2..8 and must not exceed 14MHz. With F_CPU > 112MHz this
//   is not possible (F_CPU/8 = 18MHz @ 144MHz, as in the WCH examples). Use F_CPU
//   <= 112MHz (e.g. 96MHz) if ADC accuracy is important.
// - The CH32V203 ADC calibration result has to be applied by software. ADC_calibrate()
//   determines the offset (as in WCH's Get_CalibrationValue), ADC_read() adds it.
// - ADC_read_TEMP() uses the factory calibration value (0x1FFFF720) and a slope of
//   -4.3mV/°C. The accuracy is only about +/- 12°C (datasheet).
// - Pins used for the OPA must be set with PIN_input_AN beforehand, also the output:
//   OPA:                       OPA1                OPA2
//   Negative input (N0/N1):    PB11 / PA6          PB10 / PA5
//   Positive input (P0/P1):    PB15 / PB0          PB14 / PA7
//   Output (OUT0/OUT1):        PA3  / PB1          PA2  / PA4
//   (CH32V203F6/x6 packages only have OPA1 or some of these pins, see datasheet)
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
  PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7, PD8, PD9, PD10, PD11, PD12, PD13, PD14, PD15
};

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR = (GPIOA->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0100<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR = (GPIOA->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0100<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR = (GPIOB->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0100<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR = (GPIOB->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0100<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR = (GPIOC->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0100<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR = (GPIOC->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0100<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR = (GPIOD->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0100<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR = (GPIOD->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0100<<(((PIN)&7)<<2))   ) :     \
(0)))))))))
#define PIN_input_HI PIN_input
#define PIN_input_FL PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ({GPIOA->CFGLR = (GPIOA->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOA->BSHR = (uint32_t)1<<((PIN)&15); }) :              \
  ((PIN>=PA8)&&(PIN<=PA15) ? ({GPIOA->CFGHR = (GPIOA->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOA->BSHR = (uint32_t)1<<((PIN)&15); }) :              \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ({GPIOB->CFGLR = (GPIOB->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOB->BSHR = (uint32_t)1<<((PIN)&15); }) :              \
  ((PIN>=PB8)&&(PIN<=PB15) ? ({GPIOB->CFGHR = (GPIOB->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOB->BSHR = (uint32_t)1<<((PIN)&15); }) :              \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ({GPIOC->CFGLR = (GPIOC->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOC->BSHR = (uint32_t)1<<((PIN)&15); }) :              \
  ((PIN>=PC8)&&(PIN<=PC15) ? ({GPIOC->CFGHR = (GPIOC->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOC->BSHR = (uint32_t)1<<((PIN)&15); }) :              \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ({GPIOD->CFGLR = (GPIOD->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOD->BSHR = (uint32_t)1<<((PIN)&15); }) :              \
  ((PIN>=PD8)&&(PIN<=PD15) ? ({GPIOD->CFGHR = (GPIOD->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOD->BSHR = (uint32_t)1<<((PIN)&15); }) :              \
(0)))))))))

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ({GPIOA->CFGLR = (GPIOA->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOA->BCR = (uint32_t)1<<((PIN)&15); }) :               \
  ((PIN>=PA8)&&(PIN<=PA15) ? ({GPIOA->CFGHR = (GPIOA->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOA->BCR = (uint32_t)1<<((PIN)&15); }) :               \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ({GPIOB->CFGLR = (GPIOB->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOB->BCR = (uint32_t)1<<((PIN)&15); }) :               \
  ((PIN>=PB8)&&(PIN<=PB15) ? ({GPIOB->CFGHR = (GPIOB->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOB->BCR = (uint32_t)1<<((PIN)&15); }) :               \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ({GPIOC->CFGLR = (GPIOC->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOC->BCR = (uint32_t)1<<((PIN)&15); }) :               \
  ((PIN>=PC8)&&(PIN<=PC15) ? ({GPIOC->CFGHR = (GPIOC->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOC->BCR = (uint32_t)1<<((PIN)&15); }) :               \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ({GPIOD->CFGLR = (GPIOD->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOD->BCR = (uint32_t)1<<((PIN)&15); }) :               \
  ((PIN>=PD8)&&(PIN<=PD15) ? ({GPIOD->CFGHR = (GPIOD->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1000<<(((PIN)&7)<<2));          \
                               GPIOD->BCR = (uint32_t)1<<((PIN)&15); }) :               \
(0)))))))))

// ===================================================================================
// Set PIN as INPUT for analog peripherals (e.g. ADC)
// ===================================================================================
#define PIN_input_AN(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR = (GPIOA->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0000<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR = (GPIOA->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0000<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR = (GPIOB->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0000<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR = (GPIOB->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0000<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR = (GPIOC->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0000<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR = (GPIOC->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0000<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR = (GPIOD->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0000<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR = (GPIOD->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0000<<(((PIN)&7)<<2))   ) :     \
(0)))))))))
#define PIN_input_AD  PIN_input_AN
#define PIN_input_ADC PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT (push-pull, maximum speed 50MHz)
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR = (GPIOA->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR = (GPIOA->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR = (GPIOB->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR = (GPIOB->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR = (GPIOC->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR = (GPIOC->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR = (GPIOD->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR = (GPIOD->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0011<<(((PIN)&7)<<2))   ) :     \
(0)))))))))
#define PIN_output_PP PIN_output

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN (maximum speed 50MHz)
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR = (GPIOA->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR = (GPIOA->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR = (GPIOB->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR = (GPIOB->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR = (GPIOC->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR = (GPIOC->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR = (GPIOD->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR = (GPIOD->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b0111<<(((PIN)&7)<<2))   ) :     \
(0)))))))))

// ===================================================================================
// Set PIN as ALTERNATE FUNCTION OUTPUT (push-pull, maximum speed 50MHz)
// ===================================================================================
#define PIN_alternate(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR = (GPIOA->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR = (GPIOA->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR = (GPIOB->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR = (GPIOB->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR = (GPIOC->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR = (GPIOC->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR = (GPIOD->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1011<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR = (GPIOD->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1011<<(((PIN)&7)<<2))   ) :     \
(0)))))))))
#define PIN_alternate_PP PIN_alternate

// ===================================================================================
// Set PIN as ALTERNATE FUNCTION OUTPUT OPEN-DRAIN (maximum speed 50MHz)
// ===================================================================================
#define PIN_alternate_OD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7)  ? ( GPIOA->CFGLR = (GPIOA->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PA8)&&(PIN<=PA15) ? ( GPIOA->CFGHR = (GPIOA->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB0)&&(PIN<=PB7)  ? ( GPIOB->CFGLR = (GPIOB->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PB8)&&(PIN<=PB15) ? ( GPIOB->CFGHR = (GPIOB->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC0)&&(PIN<=PC7)  ? ( GPIOC->CFGLR = (GPIOC->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PC8)&&(PIN<=PC15) ? ( GPIOC->CFGHR = (GPIOC->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD0)&&(PIN<=PD7)  ? ( GPIOD->CFGLR = (GPIOD->CFGLR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1111<<(((PIN)&7)<<2))   ) :     \
  ((PIN>=PD8)&&(PIN<=PD15) ? ( GPIOD->CFGHR = (GPIOD->CFGHR                             \
                                        & ~((uint32_t)0b1111<<(((PIN)&7)<<2)))          \
                                        |  ((uint32_t)0b1111<<(((PIN)&7)<<2))   ) :     \
(0)))))))))

// ===================================================================================
// Set PIN output value to LOW
// ===================================================================================
#define PIN_low(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BCR = (uint32_t)1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BCR = (uint32_t)1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BCR = (uint32_t)1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BCR = (uint32_t)1<<((PIN)&15) ) : \
(0)))))

// ===================================================================================
// Set PIN output value to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BSHR = (uint32_t)1<<((PIN)&15) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BSHR = (uint32_t)1<<((PIN)&15) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BSHR = (uint32_t)1<<((PIN)&15) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BSHR = (uint32_t)1<<((PIN)&15) ) : \
(0)))))

// ===================================================================================
// Toggle PIN output value (single write to BSHR, other pins are not affected)
// ===================================================================================
#define PIN_toggle(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( GPIOA->BSHR = (GPIOA->OUTDR & ((uint32_t)1<<((PIN)&15)))  \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( GPIOB->BSHR = (GPIOB->OUTDR & ((uint32_t)1<<((PIN)&15)))  \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( GPIOC->BSHR = (GPIOC->OUTDR & ((uint32_t)1<<((PIN)&15)))  \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( GPIOD->BSHR = (GPIOD->OUTDR & ((uint32_t)1<<((PIN)&15)))  \
                                         ? ((uint32_t)1<<(((PIN)&15)+16)) : ((uint32_t)1<<((PIN)&15)) ) : \
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
#define PIN_write(PIN, val) ((val)?(PIN_high(PIN)):(PIN_low(PIN)))

// ===================================================================================
// Setup PIN interrupt / event (EXTI)
// ===================================================================================
enum{PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH};
enum{PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH};

// Connect EXTI line (PIN&15) to the port of PIN
#define PIN_EXTI_set(PIN) \
  (RCC->APB2PCENR |= RCC_AFIOEN, \
   AFIO->EXTICR[((PIN)&15)>>2] = (AFIO->EXTICR[((PIN)&15)>>2]         \
                               & ~((uint32_t)0b1111 << (((PIN)&3)<<2))) \
                               |  ((uint32_t)((PIN)>>4) << (((PIN)&3)<<2)))

#define PIN_INT_set(PIN, TYPE) \
  (PIN_EXTI_set(PIN), \
  ((TYPE) & 3) ? (EXTI->INTENR |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->INTENR &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 1) ? (EXTI->RTENR  |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->RTENR  &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 2) ? (EXTI->FTENR  |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->FTENR  &= ~((uint32_t)1<<((PIN)&15))))

#define PIN_EVT_set(PIN, TYPE) \
  (PIN_EXTI_set(PIN), \
  ((TYPE) & 3) ? (EXTI->EVENR  |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->EVENR  &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 1) ? (EXTI->RTENR  |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->RTENR  &= ~((uint32_t)1<<((PIN)&15))), \
  ((TYPE) & 2) ? (EXTI->FTENR  |=  (uint32_t)1<<((PIN)&15)) : \
                 (EXTI->FTENR  &= ~((uint32_t)1<<((PIN)&15))))

// NVIC interrupt number of the EXTI line of PIN
#define PIN_IRQn(PIN) \
  ((((PIN)&15) <= 4) ? (IRQn_Type)(EXTI0_IRQn + ((PIN)&15)) : \
   (((PIN)&15) <= 9) ? EXTI9_5_IRQn : EXTI15_10_IRQn)

#define PIN_INT_enable(PIN)     NVIC_EnableIRQ(PIN_IRQn(PIN))
#define PIN_INT_disable(PIN)    NVIC_DisableIRQ(PIN_IRQn(PIN))

#define PIN_INTFLAG_read(PIN)   (EXTI->INTFR & ((uint32_t)1 << ((PIN) & 15)))
#define PIN_INTFLAG_clear(PIN)  (EXTI->INTFR = ((uint32_t)1 << ((PIN) & 15)))

#define PIN_INT_ISR(handler)    void handler(void) __attribute__((interrupt)); \
                                void handler(void)

// ===================================================================================
// Enable GPIO PORTS
// ===================================================================================
#define PORTA_enable()    (RCC->APB2PCENR |= RCC_IOPAEN)
#define PORTB_enable()    (RCC->APB2PCENR |= RCC_IOPBEN)
#define PORTC_enable()    (RCC->APB2PCENR |= RCC_IOPCEN)
#define PORTD_enable()    (RCC->APB2PCENR |= RCC_IOPDEN)
#define PORTS_enable()    (RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_IOPDEN)

#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->APB2PCENR |= RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->APB2PCENR |= RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->APB2PCENR |= RCC_IOPCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->APB2PCENR |= RCC_IOPDEN ) : \
(0)))))

// ===================================================================================
// Disable GPIO PORTS
// ===================================================================================
#define PORTA_disable()   (RCC->APB2PCENR &= ~RCC_IOPAEN)
#define PORTB_disable()   (RCC->APB2PCENR &= ~RCC_IOPBEN)
#define PORTC_disable()   (RCC->APB2PCENR &= ~RCC_IOPCEN)
#define PORTD_disable()   (RCC->APB2PCENR &= ~RCC_IOPDEN)
#define PORTS_disable()   (RCC->APB2PCENR &= ~(RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_IOPDEN))

#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA15) ? ( RCC->APB2PCENR &= ~RCC_IOPAEN ) : \
  ((PIN>=PB0)&&(PIN<=PB15) ? ( RCC->APB2PCENR &= ~RCC_IOPBEN ) : \
  ((PIN>=PC0)&&(PIN<=PC15) ? ( RCC->APB2PCENR &= ~RCC_IOPCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD15) ? ( RCC->APB2PCENR &= ~RCC_IOPDEN ) : \
(0)))))

// ===================================================================================
// ADC Functions
// ===================================================================================

// ADC clock prescaler (ADC clock = PCLK2 / n, max 14MHz)
#if   F_CPU <= 28000000
  #define ADC_PRESCALER     RCC_ADCPRE_DIV2
#elif F_CPU <= 56000000
  #define ADC_PRESCALER     RCC_ADCPRE_DIV4
#elif F_CPU <= 84000000
  #define ADC_PRESCALER     RCC_ADCPRE_DIV6
#else
  #define ADC_PRESCALER     RCC_ADCPRE_DIV8   // > 14MHz if F_CPU > 112MHz !
#endif

// ADC offset correction (determined by ADC_calibrate(), one instance for all files)
__attribute__((weak)) int16_t ADC_offset = 0;

#define ADC_enable()        (ADC1->CTLR2 |=  ADC_ADON)
#define ADC_disable()       (ADC1->CTLR2 &= ~ADC_ADON)

#define ADC_fast()          (ADC1->SAMPTR1 = 0x00000000, ADC1->SAMPTR2 = 0x00000000)
#define ADC_medium()        (ADC1->SAMPTR1 = 0x00DB6DB6, ADC1->SAMPTR2 = 0x36DB6DB6)
#define ADC_slow()          (ADC1->SAMPTR1 = 0x00FFFFFF, ADC1->SAMPTR2 = 0x3FFFFFFF)

#define ADC_input_TEMP()    (ADC1->RSQR3 = 16)
#define ADC_input_VREF()    (ADC1->RSQR3 = 17)
#define ADC_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? (ADC1->RSQR3 =  (PIN)&7)      : \
  ((PIN>=PB0)&&(PIN<=PB1) ? (ADC1->RSQR3 = ((PIN)&7)+8)   : \
  ((PIN>=PC0)&&(PIN<=PC5) ? (ADC1->RSQR3 = ((PIN)&7)+10)  : \
(0))))

// Sample and read raw ADC value of current input (without offset correction)
static inline uint16_t ADC_readRaw(void) {
  ADC1->CTLR2 |= ADC_SWSTART;                   // start conversion
  while(!(ADC1->STATR & ADC_EOC));              // wait until finished
  return ADC1->RDATAR;                          // return result (clears EOC)
}

// Calibrate ADC and determine offset correction (algorithm from WCH CH32V20x_D6 SDK)
static inline void ADC_calibrate(void) {
  uint16_t buf[10], t;
  uint32_t s1 = 0, s2 = 0;
  uint8_t  i, j, n1 = 0, n2 = 0;
  uint32_t ctlr1 = ADC1->CTLR1, ctlr2 = ADC1->CTLR2;
  ADC1->CTLR1 &= ~ADC_BUFEN;                    // buffer must be off for calibration
  ADC1->CTLR2 &= ~ADC_TSVREFE;                  // TSVREFE forces buffer on
  for(i=0; i<10; i++) {
    ADC1->CTLR2 |= ADC_RSTCAL;                  // reset calibration
    while(ADC1->CTLR2 & ADC_RSTCAL);            // wait until finished
    ADC1->CTLR2 |= ADC_CAL;                     // start calibration
    while(ADC1->CTLR2 & ADC_CAL);               // wait until finished
    buf[i] = ADC1->RDATAR;                      // read calibration code
  }
  for(i=0; i<9; i++) for(j=0; j<9-i; j++)       // sort values
    if(buf[j] > buf[j+1]) {t = buf[j]; buf[j] = buf[j+1]; buf[j+1] = t;}
  for(i=2; i<8; i++) {                          // use the 6 middle values
    if(buf[i] <= 1536) {s1 += buf[i]; n1++;}    // values around 1024
    else               {s2 += buf[i]; n2++;}    // values around 2048
  }
  s1 = n1 ? (s1 + n1/2) / n1 : 1024;            // average of each group
  s2 = n2 ? (s2 + n2/2) / n2 : 2048;
  {
    int16_t x  = (int16_t)(1024 - s1) + (int16_t)(2048 - s2);
    ADC_offset = x / 2 + x % 2;                 // rounded mean of both offsets
  }
  ADC1->CTLR1 = ctlr1;                          // restore settings
  ADC1->CTLR2 = ctlr2;
}

// Init, calibrate and enable ADC
static inline void ADC_init(void) {
  RCC->APB2PCENR |=  RCC_ADC1EN | RCC_AFIOEN;   // enable ADC and AFIO
  RCC->APB2PRSTR |=  RCC_ADC1RST;               // reset ADC
  RCC->APB2PRSTR &= ~RCC_ADC1RST;
  RCC->CFGR0 = (RCC->CFGR0 & ~RCC_ADCPRE) | ADC_PRESCALER; // set ADC clock
  ADC_medium();                                 // set medium speed as default
  ADC1->CTLR2  = ADC_ADON                       // turn on ADC
               | ADC_EXTTRIG                    // enable trigger for regular group
               | ADC_EXTSEL;                    // trigger by SWSTART
  DLY_us(10);                                   // wait for ADC to stabilize
  ADC_calibrate();                              // calibrate ADC
  ADC1->CTLR2 |= ADC_TSVREFE;                   // enable TEMP + VREF
}

// Sample and read ADC value of current input (with offset correction)
static inline uint16_t ADC_read(void) {
  int32_t val = ADC_readRaw() + ADC_offset;
  if(val < 0)    return 0;
  if(val > 4095) return 4095;
  return val;
}

// Sample and read supply voltage (VDD) in millivolts (mV)
static inline uint16_t ADC_read_VDD(void) {
  uint16_t val;
  ADC_input_VREF();                             // set VREF as ADC input
  val = ADC_read();
  return(val ? (uint32_t)1200 * 4096 / val : 0);// return VDD in mV (VREFINT = 1.2V)
}

// Sample and read temperature sensor in °C
static inline int8_t ADC_read_TEMP(void) {
  int32_t vdd, mv, cal_mv, cal_temp;
  vdd = ADC_read_VDD();                         // supply voltage as reference
  ADC_input_TEMP();                             // set temp sensor as ADC input
  mv  = (int32_t)ADC_read() * vdd / 4096;       // sensor voltage in mV
  cal_mv   = (*(__I uint32_t*)0x1FFFF720) & 0xFFFF;         // factory calibration
  cal_temp = ((*(__I uint32_t*)0x1FFFF720) >> 16) & 0xFFFF;
  return cal_temp - ((mv - cal_mv) * 10 + 21) / 43;  // slope -4.3mV/°C
}

// ===================================================================================
// OPA Functions
// ===================================================================================
#define OPA1_enable()       (OPA->CR |=  OPA_CTLR_EN1)
#define OPA1_disable()      (OPA->CR &= ~OPA_CTLR_EN1)
#define OPA2_enable()       (OPA->CR |=  OPA_CTLR_EN2)
#define OPA2_disable()      (OPA->CR &= ~OPA_CTLR_EN2)

#define OPA1_negative(PIN) \
  ((PIN) == PB11 ? (OPA->CR &= ~OPA_CTLR_NSEL1) : \
  ((PIN) == PA6  ? (OPA->CR |=  OPA_CTLR_NSEL1) : \
(0)))

#define OPA1_positive(PIN) \
  ((PIN) == PB15 ? (OPA->CR &= ~OPA_CTLR_PSEL1) : \
  ((PIN) == PB0  ? (OPA->CR |=  OPA_CTLR_PSEL1) : \
(0)))

#define OPA1_output(PIN) \
  ((PIN) == PA3  ? (OPA->CR &= ~OPA_CTLR_MODE1) : \
  ((PIN) == PB1  ? (OPA->CR |=  OPA_CTLR_MODE1) : \
(0)))

#define OPA2_negative(PIN) \
  ((PIN) == PB10 ? (OPA->CR &= ~OPA_CTLR_NSEL2) : \
  ((PIN) == PA5  ? (OPA->CR |=  OPA_CTLR_NSEL2) : \
(0)))

#define OPA2_positive(PIN) \
  ((PIN) == PB14 ? (OPA->CR &= ~OPA_CTLR_PSEL2) : \
  ((PIN) == PA7  ? (OPA->CR |=  OPA_CTLR_PSEL2) : \
(0)))

#define OPA2_output(PIN) \
  ((PIN) == PA2  ? (OPA->CR &= ~OPA_CTLR_MODE2) : \
  ((PIN) == PA4  ? (OPA->CR |=  OPA_CTLR_MODE2) : \
(0)))

// Legacy names (the OPA output is analog, set output pin with PIN_input_AN)
#define OPA1_output_PP      OPA1_output
#define OPA1_output_OD      OPA1_output
#define OPA2_output_PP      OPA2_output
#define OPA2_output_OD      OPA2_output

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
