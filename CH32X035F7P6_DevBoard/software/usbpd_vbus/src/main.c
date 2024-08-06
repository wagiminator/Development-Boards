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
// Request the specified target voltage from a USB PD power supply.
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
#include <config.h>                       // user configurations
#include <gpio.h>                         // GPIO functions
#include <usbpd_sink.h>                   // USB PD sink functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup, request target voltage and light up LED if successful
  if(PD_connect()) {
    if(PD_setVoltage(TARGET_VOLTAGE)) PIN_output(PIN_LED);
  }
  
  // Loop
  while(1);                               // nothing to do anymore
}
