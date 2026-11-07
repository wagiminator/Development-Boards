// ===================================================================================
// Project:   Example for CH55x
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Fades an LED in and out using hardware PWM. Connect an LED with a series
// resistor (e.g. 470R) between pin P3.4 and GND.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with chprog (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the board and keep it pressed while connecting it via
//   USB to your PC.
// - Run 'make flash' immediatly afterwards.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "gpio.h"                   // GPIO functions (PWM)

#define PIN_PWM   P34               // PWM pin (P15/P30 or P34/P31)

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  uint8_t i;
  CLK_config();                     // configure system clock
  PIN_output(PIN_PWM);              // set PWM pin as output
  PWM_set_freq(1000);               // set PWM frequency to 1kHz
  PWM_start(PIN_PWM);               // start PWM output

  // Loop
  while(1) {
    for(i=0;   i<255; i++) { PWM_write(PIN_PWM, i); DLY_ms(4); }
    for(i=255; i>0;   i--) { PWM_write(PIN_PWM, i); DLY_ms(4); }
  }
}
