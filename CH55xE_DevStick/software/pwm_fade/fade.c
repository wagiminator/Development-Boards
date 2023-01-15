// ===================================================================================
// Project:   Fade Demo for CH551, CH552, CH554
// Version:   v1.0
// Year:      2022
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Fade an LED connected to pin 1.5
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: doesn't matter
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

#include <ch554.h>            // CH55x header file
#include <clock.h>            // system clock functions
#include <delay.h>            // delay functions
#include <gpio.h>             // GPIO functions

#define PIN_LED   P15         // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================

void main(void) {
  CLK_config();               // configure system clock
  PIN_output(PIN_LED);        // set LED pin as output
  PWM_start(PIN_LED);         // start PWM on LED pin

  while(1) {
    uint8_t i;
    while(++i) {              // increase PWM value
      PWM_write(PIN_LED, i);  // write PWM value
      DLY_ms(10);             // delay a bit
    }
    while(--i) {              // decrease PWM value
      PWM_write(PIN_LED, i);  // write PWM value
      DLY_ms(10);             // delay a bit
    }
  }
}
