// ===================================================================================
// Project:   Supply Voltage Measurement Demo for CH32V003
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
// Measures 1.2V internal voltage reference (Vref) against Vdd with analog to digital
// converter (ADC), calculates supply voltage in millivolts (mV) and sends value via 
// UART.
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
#include <system.h>                 // system functions
#include <gpio.h>                   // GPIO functions
#include <debug_serial.h>           // serial debug functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DEBUG_init();                     // init debug with default BAUD rate (115200)
  ADC_init();                       // init ADC
  
  // Loop
  while(1) {
    DEBUG_printD(ADC_read_VDD());   // read Vdd and send value via UART
    DEBUG_printS("mV\n");           // print unit
    DLY_ms(1000);                   // delay one second
  }
}
