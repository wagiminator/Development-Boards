// ===================================================================================
// Project:   Blink Demo for CH551, CH552, CH554
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
// Blink an LED connected to pin 3.3
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>           // system functions
#include <delay.h>            // delay functions
#include <gpio.h>             // GPIO functions

#define PIN_LED   P33         // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================

void main() {
  CLK_config();               // configure system clock
  PIN_output(PIN_LED);        // set LED pin as output

  while (1) {
    DLY_ms(100);              // wait 100ms
    PIN_toggle(PIN_LED);      // toggle LED on/off
  }
}
