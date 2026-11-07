// ===================================================================================
// Project:   Example for ATSAMD11D14A
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
// Fades the built-in LED in and out using hardware PWM (TCC0).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with tinyocd (via pip)
//   are installed. If necessary, grant access permissions for the SWD programmer.
// - Connect the SWD programmer (e.g. CMSIS-DAP) to the SWD pins of the board.
// - Run 'make flash'.
// - Alternatively, if the SAM-BA bootloader is installed, set LDSCRIPT and ISPTOOL
//   in the makefile accordingly (see readme) and upload the firmware via USB.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "pwm.h"                    // PWM functions

#define PIN_LED   PA16              // define LED pin (must be PWM capable)

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  PWM_init();                       // init PWM (TCC0)
  PWM_start(PIN_LED);               // start PWM output on LED pin

  // Loop
  while(1) {
    for(uint16_t i=0;   i<256; i++) { PWM_write(PIN_LED, i); DLY_ms(4); }
    for(uint16_t i=256; i>0;   i--) { PWM_write(PIN_LED, i); DLY_ms(4); }
  }
}
