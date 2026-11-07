// ===================================================================================
// Project:   Example for ATmega4808
// Version:   v1.1
// Year:      2020
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Blinks the LED with Core Independent Peripherals (CIP). The periodic interrupt
// timer (PIT) directly toggles pin PA7 (built-in LED) via channel 0 of the event
// system (EVSYS). Because the CPU isn't needed anymore, it is put into sleep mode
// POWER-DOWN forever.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with megaupdi (via pip)
//   are installed. If necessary, a driver for the USB-to-serial converter used by
//   the SerialUPDI programmer must be installed.
// - Connect the SerialUPDI programmer to the UPDI header of the board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions

#define PIN_LED   PA7               // LED pin (must be PA7 = EVOUTA)

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  CLK_init();                                   // set system clock frequency
  PIN_output(PIN_LED);                          // built-in LED as output
  RTC.PITCTRLA     = RTC_PITEN_bm;              // enable PIT (32768Hz from OSCULP32K)
  EVSYS.CHANNEL0   = EVSYS_GENERATOR_RTC_PIT0_gc; // PIT div 8192 (2Hz) as generator
  EVSYS.USEREVOUTA = EVSYS_CHANNEL_CHANNEL0_gc;   // event output port A as user
  PORTMUX.EVSYSROUTEA = PORTMUX_EVOUTA_bm;      // EVOUTA on alternative pin PA7
  SLEEP_set_PDOWN();                            // set sleep mode to POWER-DOWN
  while(1) SLEEP_now();                         // go to sleep forever
}
