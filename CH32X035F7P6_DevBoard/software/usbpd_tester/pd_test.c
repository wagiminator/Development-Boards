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
// Displays source capabilities of the connected USB PD power supply on an OLED.
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
#include <oled_text.h>                    // OLED text functions
#include <usbpd_sink.h>                   // USB PD sink functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Variables
  uint8_t i;

  // Setup
  OLED_init();                            // init OLED
  OLED_printf("Connecting...");
  if(!PD_connect()) {                     // init USB PD
    OLED_printf("FAILED");
    while(1);
  }
  
  // Print source capabilities
  OLED_cursor(0, 0);
  for(i = 1; i <= PD_getPDONum(); i++) {
    if(i <= PD_getFixedNum())
      OLED_printf(" (%d)%6dmV %5dmA ", i, PD_getPDOVoltage(i), PD_getPDOMaxCurrent(i));
    else
      OLED_printf(" [%d]%6dmV-%5dmV ", i, PD_getPDOMinVoltage(i), PD_getPDOMaxVoltage(i));
  }
  
  // Loop
  while(1);                               // nothing to do anymore
}
