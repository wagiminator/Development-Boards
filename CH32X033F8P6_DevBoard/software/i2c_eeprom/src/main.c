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
// Transfers content of 24Cxx I2C EEPROM via USB-CDC.
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
#include <system.h>                               // system functions
#include <usb_cdc.h>                              // USB CDC functions
#include <i2c_soft.h>                             // software I2C functions
#include <eeprom_24c.h>                           // 24Cxx I2C EEPROM functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CDC_init();                                     // init USB CDC
  I2C_init();                                     // init I2C
  EEPROM_update(0, 0xaa);                         // write value to address 0
  
  // Loop
  while(1) {
    uint8_t i,j;
    uint8_t addr = 0;
    CDC_println("EEPROM Hex Dump:");
    for(j=16; j; j--) {
      CDC_printH(addr); CDC_print(": ");
      for(i=16; i; i--) {
        CDC_printB(EEPROM_read(addr++)); CDC_write(' ');
      }
      CDC_println("");
    }
    CDC_println("");
    DLY_ms(1000);                                 // wait a second
  }
}
