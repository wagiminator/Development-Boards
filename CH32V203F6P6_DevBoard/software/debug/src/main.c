// ===================================================================================
// Project:   Serial Debug Demo for CH32V203
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
// Sends a message via UART once a second. TX pin is PA2.
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>         // system functions
#include <gpio.h>           // GPIO functions
#include <debug_serial.h>   // serial debug functions

#define PIN_LED   PB1       // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DEBUG_init();             // init debug with default BAUD rate (115200)
  PIN_output(PIN_LED);      // set LED pin as output
  
  // Loop
  while(1) {
    DLY_ms(500);           // wait a second
    PIN_toggle(PIN_LED);   // toggle LED
    DEBUG_out("Hello!");   // send string
  }
}
