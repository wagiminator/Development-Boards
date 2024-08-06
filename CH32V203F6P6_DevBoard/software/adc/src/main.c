// ===================================================================================
// Project:   ADC Demo for CH32V203
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
// Sends ADC value of PA0, Vdd and chip temperature via UART (TX pin is PA2).
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with
//   chprog and rvprog (via pip) are installed. In addition, Linux requires access
//   rights to the USB bootloader.
// - Press the BOOT0 button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>         // system functions
#include <gpio.h>           // GPIO functions
#include <debug_serial.h>   // serial debug functions

#define PIN_LED   PB1       // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  ADC_init();               // init ADC
  DEBUG_init();             // init debug with default BAUD rate (115200)
  PIN_output(PIN_LED);      // set LED pin as output
  
  // Loop
  while(1) {
    DLY_ms(500);           // wait a second
    PIN_toggle(PIN_LED);   // toggle LED
    ADC_input(PA0);
    DEBUG_print("ADC-value PA0:    "); DEBUG_printD(ADC_read()); DEBUG_newline();
    DEBUG_print("Supply voltage:   "); DEBUG_printD(ADC_read_VDD()); DEBUG_println("mV");
    DEBUG_print("Chip temperature: "); DEBUG_printD(ADC_read_TEMP()); DEBUG_println("C");
  }
}
