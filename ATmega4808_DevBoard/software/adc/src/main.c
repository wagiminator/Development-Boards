// ===================================================================================
// Project:   Example for ATmega4808
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
// ADC example including supply voltage and temperature measurement. Reads the
// analog value on pin PD0, the supply voltage and the chip temperature once a
// second and sends the results via UART (115200 BAUD, 8N1).
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
#include "debug_serial.h"           // serial debug functions

#define PIN_ADC   PD0               // pin for ADC measurement

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CLK_init();                       // set system clock frequency
  DEBUG_init();                     // init DEBUG (TX: PA0, BAUD: 115200, 8N1)
  ADC_init();                       // init ADC

  // Loop
  while(1) {
    // Read and print PIN
    ADC_REF_VDD();                                  // select VDD as ADC reference
    ADC_input(PIN_ADC);                             // set ADC input pin
    DEBUG_printf("PIN:  %u\n", ADC_read());         // read value and send via UART

    // Read and print VDD in mV
    DEBUG_printf("VDD:  %u mV\n", ADC_read_VDD());  // read supply voltage and send via UART

    // Read and print chip temperature in degrees Celsius
    DEBUG_printf("TEMP: %d C\n", ADC_read_TEMP());  // read temperature and send via UART

    DLY_ms(1000);                                   // wait a second
  }
}
