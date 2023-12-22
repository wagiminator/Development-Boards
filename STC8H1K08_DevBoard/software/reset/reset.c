// ===================================================================================
// Project:   Example for STC8H1K08
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
// Blinks built-in LED using continuous software resets.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with PySerial is installed.
// - Connect the MCU board via USB to your PC.
// - Run 'make flash'.
// - Perform a power cycle (press and release DOWNLOAD button) when prompted.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "gpio.h"                       // GPIO functions
#include "delay.h"                      // delay functions
#include "system.h"                     // system functions

// Pin definitions
#define PIN_LED   P34                   // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
void main() {
  PIN_output(PIN_LED);                  // set LED pin as output
  DLY_ms(100);                          // wait 100ms
  PIN_toggle(PIN_LED);                  // toggle LED on/off
  DLY_ms(100);                          // wait 100ms
  RST_now();                            // reset
}
