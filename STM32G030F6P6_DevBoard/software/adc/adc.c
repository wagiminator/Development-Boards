// ===================================================================================
// Project:   Example for STM32G03x/04x
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
// ADC example including supply voltage and temperature measurement.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with PySerial is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect your MCU board via USB to your PC.
// - Set the MCU to boot mode by holding down the BOOT key and then pressing and 
//   releasing the RESET key. Finally release the BOOT key.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "debug_serial.h"           // serial debug functions

#define PIN_ADC   PA0               // pin for ADC measurement

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  DEBUG_init();                     // init DEBUG (TX: PA2, BAUD: 115200, 8N1)
  ADC_init();                       // init ADC
  ADC_slow();                       // set slow sample rate
  
  // Loop
  while(1) {
    // Read and print PIN
    ADC_input(PIN_ADC);
    DEBUG_printS("PIN:  ");
    DEBUG_printD(ADC_read());
    DEBUG_newline();

    // Read and print VDD
    DEBUG_printS("VDD:  ");
    DEBUG_printD(ADC_read_VDD());
    DEBUG_printS("mV\n");

    // Read and print chip temperature
    DEBUG_printS("TEMP: ");
    DEBUG_printD(ADC_read_TEMP());
    DEBUG_printS("C\n");
    
    // Read and print calibration values
    DEBUG_printS("TSCAL1:  "); DEBUG_printD(ADC_TSCAL1);  DEBUG_newline();
    DEBUG_printS("TSCAL2:  "); DEBUG_printD(ADC_TSCAL2);  DEBUG_newline();
    DEBUG_printS("VREFCAL: "); DEBUG_printD(ADC_VREFCAL); DEBUG_newline();

    DLY_ms(1000);                   // delay one second
  }
}
