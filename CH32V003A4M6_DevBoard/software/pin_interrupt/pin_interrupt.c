// ===================================================================================
// Project:   Example for CH32V003
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
// Control LED via push button using interrupts.
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>                       // system functions
#include <gpio.h>                         // GPIO functions

#define PIN_LED   PC0                     // define LED pin
#define PIN_KEY   PC3                     // define button pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  PIN_output(PIN_LED);                    // set LED pin as output
  PIN_high(PIN_LED);                      // turn off LED
  PIN_input_PU(PIN_KEY);                  // set button pin as input pullup

  PIN_INT_set(PIN_KEY, PIN_INT_BOTH);     // set pin change interrupt for button pin
  PIN_INT_enable();                       // enable pin interrupts

  // Loop
  while(1) STDBY_WFI_now();               // put MCU to standby, wake up by interrupt
}

// ===================================================================================
// Interrupt Service Routine
// ===================================================================================
PIN_INT_ISR {
  PIN_INTFLAG_clear(PIN_KEY);             // clear interrupt flag
  PIN_write(PIN_LED, PIN_read(PIN_KEY));  // set LED pin according to button state
}
