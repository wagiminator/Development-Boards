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
  ADC_slow();                       // slow sample rate -> more accurate
  
  // Loop
  while(1) {
    // Read and print PIN
    ADC_input(PIN_ADC);                             // set ADC input pin
    DEBUG_printf("PIN:  %d\n", ADC_read());         // read value and send via UART

    // Read and print VDD in mV
    DEBUG_printf("VDD:  %d mV\n", ADC_read_VDD());  // read supply voltage and send via UART

    // Read and print chip temperature in °C
    DEBUG_printf("TEMP: %d C\n", ADC_read_TEMP());  // read temperature and send via UART
    
    // Read and print calibration values
    DEBUG_printf("TSCAL1:  %d\n", ADC_TSCAL1);
    DEBUG_printf("TSCAL2:  %d\n", ADC_TSCAL2);
    DEBUG_printf("VREFCAL: %d\n", ADC_VREFCAL);

    DLY_ms(1000);                                   // delay one second
  }
}
