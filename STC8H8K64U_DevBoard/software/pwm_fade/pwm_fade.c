// ===================================================================================
// Project:   Example for STC8H8K64U
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Fades built-in LED using PWM.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with pyhidapi is installed.
// - Press and hold BOOT button while connecting the MCU board to your PC via USB.
// - Run 'make flash'.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "gpio.h"                       // GPIO functions
#include "delay.h"                      // delay functions

// Pin definitions
#define PIN_LED   P34                   // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
void main() {
  // Variables
  uint8_t i = 0;

  // Setup
  P_SW2 |= 0x80;                        // enable access to extended registers (MUST!)
  PWM_init();                           // init PWM
  PWM_start(PIN_LED);                   // start PWM on LED pin

  // Loop
  while (1) {
    for(i = 0; i < 255; i++) {
      PWM_write(PIN_LED, i);            // fade in
      DLY_ms(10);
    }
    for(i = 255; i; i--) {
      PWM_write(PIN_LED, i);            // fade out
      DLY_ms(10);
    }
  }
}
