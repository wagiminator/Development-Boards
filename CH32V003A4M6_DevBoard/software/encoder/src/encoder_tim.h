// ===================================================================================
// Basic Rotary Encoder Functions using Timer for CH32V003                    * v1.0 *
// ===================================================================================
//
// This library contains the basic functions to read up to two rotary encoders 
// utilizing the corresponding timer functions of the MCU.
//
// Functions available:
// --------------------
// ENC1_init()              Init rotary encoder 1 pins and setup timer1 in encoder mode
// ENC1_set(cur, max)       Set rotary encoder 1 current and maximum count value
// ENC1_get()               Read rotary encoder 1 current count value
//
// ENC2_init()              Init rotary encoder 2 pins and setup timer2 in encoder mode
// ENC2_set(cur, max)       Set rotary encoder 2 current and maximum count value
// ENC2_get()               Read rotary encoder 2 current count value
//
// ENC pin mapping (set below in encoder parameters):
// --------------------------------------------------
// ENC1     0     1     2       ENC2    0     1     2     3
// ENC_A   PD2   PC6   PC4             PD4   PC5   PC1   PC1
// ENC_B   PA1   PC7   PC7             PD3   PC2   PD3   PC7
//
// Notes:
// ------
// - The available functions only read the rotation of the encoder. Checking the 
//   encoder switch must be done separately.
// - ENC1 uses Timer1, ENC2 uses Timer2. Note that when in use, these timers will 
//   no longer be available for other functionalities.
// - The encoder count value is 16-bit and increases or decreases according to the 
//   rotation of the encoder. Note that depending on the type of encoder, this
//   value changes by 2 or 4 per detent. The count value wraps around.
// - The rotary encoder must be connected so that it switches to ground.
// - For reliable operation, hardware debouncing is recommended.
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// Encoder Parameters
#define ENC1_MAP    1
#define ENC2_MAP    0

// Encoder Functions
void ENC1_init(void);
void ENC1_set(uint16_t cur, uint16_t max);
uint16_t ENC1_get(void);

void ENC2_init(void);
void ENC2_set(uint16_t cur, uint16_t max);
uint16_t ENC2_get(void);

#ifdef __cplusplus
};
#endif
