// ===================================================================================
// Basic GPIO Functions for CH32V003                                          * v1.0 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PC0, PC1, etc. - e.g.:
// #define PIN_LED PC0      // LED on pin PC0
//
// Functions available:
// --------------------
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
// PORT_enable(PIN)         Enable GPIO PORT of PIN
// PORT_disable(PIN)        Disable GPIO PORT of PIN
// PORTS_enable()           Enable all GPIO PORTS
// PORTS_disable()          Disable all GPIO PORTS
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include "ch32v003.h"

// ===================================================================================
// Enumerate PIN designators (use these designators to define pins)
// ===================================================================================
enum{ PA0, PA1, PA2, PA3, PA4, PA5, PA6, PA7,
      PC0, PC1, PC2, PC3, PC4, PC5, PC6, PC7,
      PD0, PD1, PD2, PD3, PD4, PD5, PD6, PD7};

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)),        \
                              GPIOA->CFGLR |= 0b0100<<(((PIN)&7)<<2)        ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)),        \
                              GPIOC->CFGLR |= 0b0100<<(((PIN)&7)<<2)        ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)),        \
                              GPIOD->CFGLR |= 0b0100<<(((PIN)&7)<<2)        ) : \
(0))))
#define PIN_input_HI PIN_input
#define PIN_input_FL PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)),        \
                              GPIOA->CFGLR |= 0b1000<<(((PIN)&7)<<2),           \
                              GPIOA->OUTDR |= 1<<((PIN)&7)                  ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)),        \
                              GPIOC->CFGLR |= 0b1000<<(((PIN)&7)<<2),           \
                              GPIOC->OUTDR |= 1<<((PIN)&7)                  ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)),        \
                              GPIOD->CFGLR |= 0b1000<<(((PIN)&7)<<2),           \
                              GPIOD->OUTDR |= 1<<((PIN)&7)                  ) : \
(0))))

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)),        \
                              GPIOA->CFGLR |= 0b1000<<(((PIN)&7)<<2),           \
                              GPIOA->OUTDR &= ~(1<<((PIN)&7))               ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)),        \
                              GPIOC->CFGLR |= 0b1000<<(((PIN)&7)<<2),           \
                              GPIOC->OUTDR &= ~(1<<((PIN)&7))               ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)),        \
                              GPIOD->CFGLR |= 0b1000<<(((PIN)&7)<<2),           \
                              GPIOD->OUTDR &= ~(1<<((PIN)&7))               ) : \
(0))))

// ===================================================================================
// Set PIN as INPUT for analog peripherals (e.g. ADC)
// ===================================================================================
#define PIN_input_AN(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR &= ~(0b1111<<(((PIN)&7)<<2)) ) : \
(0))))

// ===================================================================================
// Set PIN as OUTPUT (push-pull, maximum speed 10MHz)
// ===================================================================================
#define PIN_output(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR   &= ~(0b1111<<(((PIN)&7)<<2)),      \
                              GPIOA->CFGLR   |= 0b0001<<(((PIN)&7)<<2)      ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR   &= ~(0b1111<<(((PIN)&7)<<2)),      \
                              GPIOC->CFGLR   |= 0b0001<<(((PIN)&7)<<2)      ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR   &= ~(0b1111<<(((PIN)&7)<<2)),      \
                              GPIOD->CFGLR   |= 0b0001<<(((PIN)&7)<<2)      ) : \
(0))))
#define PIN_output_PP PIN_output

// ===================================================================================
// Set PIN as OUTPUT OPEN-DRAIN (maximum speed 10MHz)
// ===================================================================================
#define PIN_output_OD(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->CFGLR   &= ~(0b1111<<(((PIN)&7)<<2)),      \
                              GPIOA->CFGLR   |= 0b0101<<(((PIN)&7)<<2)      ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->CFGLR   &= ~(0b1111<<(((PIN)&7)<<2)),      \
                              GPIOC->CFGLR   |= 0b0101<<(((PIN)&7)<<2)      ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->CFGLR   &= ~(0b1111<<(((PIN)&7)<<2)),      \
                              GPIOD->CFGLR   |= 0b0101<<(((PIN)&7)<<2)      ) : \
(0))))

// ===================================================================================
// Set PIN output value to LOW
// ===================================================================================
#define PIN_low(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->BCR = 1<<((PIN)&7) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->BCR = 1<<((PIN)&7) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->BCR = 1<<((PIN)&7) ) : \
(0))))

// ===================================================================================
// Set PIN output value to HIGH
// ===================================================================================
#define PIN_high(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->BSHR = 1<<((PIN)&7) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->BSHR = 1<<((PIN)&7) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->BSHR = 1<<((PIN)&7) ) : \
(0))))

// ===================================================================================
// Toggle PIN output value
// ===================================================================================
#define PIN_toggle(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( GPIOA->OUTDR ^= 1<<((PIN)&7) ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( GPIOC->OUTDR ^= 1<<((PIN)&7) ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( GPIOD->OUTDR ^= 1<<((PIN)&7) ) : \
(0))))

// ===================================================================================
// Read PIN input value (returns 0 for LOW, 1 for HIGH)
// ===================================================================================
#define PIN_read(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( (GPIOA->INDR>>((PIN)&7))&1 ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( (GPIOC->INDR>>((PIN)&7))&1 ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( (GPIOD->INDR>>((PIN)&7))&1 ) : \
(0))))

// ===================================================================================
// Write PIN output value (0 = LOW / 1 = HIGH)
// ===================================================================================
#define PIN_write(PIN, val) (val)?(PIN_high(PIN)):(PIN_low(PIN))

// ===================================================================================
// Enable GPIO PORT of PIN
// ===================================================================================
#define PORT_enable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( RCC->APB2PCENR |= RCC_IOPAEN ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( RCC->APB2PCENR |= RCC_IOPCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( RCC->APB2PCENR |= RCC_IOPDEN ) : \
(0))))

// ===================================================================================
// Disable GPIO PORT of PIN
// ===================================================================================
#define PORT_disable(PIN) \
  ((PIN>=PA0)&&(PIN<=PA7) ? ( RCC->APB2PCENR &= ~RCC_IOPAEN ) : \
  ((PIN>=PC0)&&(PIN<=PC7) ? ( RCC->APB2PCENR &= ~RCC_IOPCEN ) : \
  ((PIN>=PD0)&&(PIN<=PD7) ? ( RCC->APB2PCENR &= ~RCC_IOPDEN ) : \
(0))))

// ===================================================================================
// Enable all GPIO PORTS
// ===================================================================================
#define PORTS_enable()      RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPCEN | RCC_IOPDEN

// ===================================================================================
// Disable all GPIO PORTS
// ===================================================================================
#define PORTS_disable()     RCC->APB2PCENR &= ~(RCC_IOPAEN | RCC_IOPCEN | RCC_IOPDEN)
