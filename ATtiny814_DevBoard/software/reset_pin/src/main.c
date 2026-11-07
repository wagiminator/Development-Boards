// ===================================================================================
// Project:   Example for ATtiny814
// Version:   v1.1
// Year:      2021
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Software-defined reset pin. Pulling pin PA4 to GND resets the MCU.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with tinyupdi (via pip)
//   are installed. If necessary, a driver for the CH340N USB-to-serial converter
//   must be installed.
// - Set the Serial Mode Switch on the board to "P" (UPDI programming mode).
// - Connect the board via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions

#define PIN_RST   PA4               // define reset pin
#define PIN_LED   PA7               // define LED pin

// ===================================================================================
// Pin Interrupt Service Routine (entered when reset pin is low)
// ===================================================================================
ISR(PORTA_PORT_vect) {
  RST_now();                        // perform software reset
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  CLK_init();                       // set system clock frequency
  PIN_output(PIN_LED);              // set LED pin as output
  PIN_pullup(PIN_RST);              // internal pullup resistor on reset pin
  PIN_INT_level(PIN_RST);           // low level interrupt for reset pin
  INT_enable();                     // global interrupt enable
  PIN_toggle(PIN_LED);              // toggle LED
  while(1);                         // wait for reset
}
