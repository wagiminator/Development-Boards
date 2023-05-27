// ===================================================================================
// Project:   Sleep / Automatic Wake-Up Timer Demo for CH32V003
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
// Blink an LED using deep sleep with automatic wake-up timer.
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>           // system functions
#include <gpio.h>             // GPIO functions

#define PIN_LED   PC0         // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  PIN_output(PIN_LED);        // set LED pin as output
  AWU_init();                 // init automatic wake-up timer

  // Loop
  while(1) {
    PIN_toggle(PIN_LED);      // toggle LED on/off
    AWU_set(100);             // set automatic wake-up timer to 100 milliseconds
    STDBY_WFE_now();          // put device into standby (deep sleep), wake by event
  }
}
