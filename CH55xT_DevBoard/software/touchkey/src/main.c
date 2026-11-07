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
// The built-in LED lights up as long as the touch key on pin P1.4 is touched.
// Connect a touch pad (e.g. a piece of copper foil) to pin P1.4. The thresholds
// can be adjusted in config.h.
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
#include "config.h"                 // user configurations
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "touch.h"                  // touch key functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                     // configure system clock
  PIN_output(PIN_LED);              // set LED pin as output
  TOUCH_start(PIN_TOUCH);           // start touch key

  // Loop
  while(1) {
    if(TOUCH_on(PIN_TOUCH)) PIN_high(PIN_LED);  // touched -> LED on
    else                    PIN_low(PIN_LED);   // not touched -> LED off
  }
}
