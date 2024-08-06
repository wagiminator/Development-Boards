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
// Samples PA0 and VDD with ADC and sends values via USB CDC.
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with
//   chprog (via pip) are installed. In addition, Linux requires access rights to
//   the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "config.h"             // user configurations
#include "system.h"             // system functions
#include "gpio.h"               // GPIO functions
#include "usb_cdc.h"            // USB CDC serial functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  ADC_init();                   // init ADC
  CDC_init();                   // init USB CDC
  
  // Loop
  while(1) {
    DLY_ms(500);                // wait half a second
    ADC_input(PIN_ADC);         // set pin as ADC input
    CDC_print("PIN: "); CDC_printD(ADC_read()); CDC_newline();
    CDC_print("VDD: "); CDC_printD(ADC_read_VDD()); CDC_println("mV");
  }
}
