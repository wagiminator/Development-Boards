// ===================================================================================
// Project:   Example for CH32X035/X034/X033
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
// Software reset pin.
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>                       // user configurations
#include <system.h>                       // system functions
#include <gpio.h>                         // GPIO functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  PIN_output(PIN_LED);                    // set LED pin as output
  DLY_ms(100);                            // wait 100ms
  PIN_toggle(PIN_LED);                    // toggle LED

  PIN_input_PU(PIN_KEY);                  // set button pin as input pullup
  PIN_INT_set(PIN_KEY, PIN_INT_FALLING);  // set pin falling interrupt for button pin
  PIN_INT_enable();                       // enable pin interrupts

  // Loop
  while(1);                               // wait for reset
}

// ===================================================================================
// Interrupt Service Routine
// ===================================================================================
PIN_INT_ISR {
  RST_now();                              // perform software reset on button press
}
