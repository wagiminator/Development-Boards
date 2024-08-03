// ===================================================================================
// Project:   Millis Demo for CH32V203
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
// Blink built-in LED using millis counter.
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>                 // system functions
#include <gpio.h>                   // GPIO functions
#include <millis.h>                 // millis functions

#define PIN_LED   PB1               // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  uint32_t nexttime = 0;            // next time variable
  PIN_output(PIN_LED);              // set LED pin as output
  MIL_init();                       // init millis counter
  
  // Loop
  while(1) {
    PIN_toggle(PIN_LED);            // toggle LED on/off
    while(MIL_read() < nexttime);   // wait for next time
    nexttime += 100;                // increase next timeby 100 milliseconds
  }
}
