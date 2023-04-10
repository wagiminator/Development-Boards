// ===================================================================================
// Project:   PWM Fade Demo for CH551, CH552 and CH554
// Version:   v1.1
// Year:      2022
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Fade the built-in LED using PWM
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "src/config.h"           // user configurations
#include "src/system.h"           // system functions
#include "src/delay.h"            // delay functions
#include "src/gpio.h"             // GPIO functions

#pragma disable_warning 84        // disable warning: variable used before init

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                   // configure system clock
  PIN_output(LED_BUILTIN);        // set LED pin as output
  PWM_start(LED_BUILTIN);         // start PWM on LED pin

  // Loop
  while(1) {
    uint8_t i;
    while(++i) {                  // increase PWM value
      PWM_write(LED_BUILTIN, i);  // write PWM value
      DLY_ms(5);                  // delay a bit
    }
    while(--i) {                  // decrease PWM value
      PWM_write(LED_BUILTIN, i);  // write PWM value
      DLY_ms(5);                  // delay a bit
    }
  }
}
