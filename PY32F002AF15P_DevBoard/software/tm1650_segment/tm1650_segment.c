// ===================================================================================
// Project:   Example for PY32F0xx
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
// Displays supply voltage and chip temperature on a 4-digit 8-segment LED Display 
// via TM1650 and I2C.
// Connections: pin PF0 (SDA) and PF1 (SCL).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with PySerial is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect the USB-to-serial converter to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"
#include "gpio.h"
#include "tm1650.h"

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  ADC_init();                                     // init ADC
  TM_init();                                      // init TM1650 display

  // Loop
  while(1) {
    TM_print(ADC_read_VDD(), 3);                  // print supply voltage on display
    DLY_ms(2000);                                 // wait two seconds
    TM_print((uint16_t)ADC_read_TEMP() * 100, 0); // print temperature
    TM_printSegment(1, TM_CHAR_DEG);              // print "°"
    TM_printSegment(0, TM_CHAR_C);                // print "C"
    DLY_ms(2000);                                 // wait two seconds
  }
}
