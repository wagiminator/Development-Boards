// ===================================================================================
// Project:   Example for CH32V003
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
// Transfers content of 24Cxx I2C EEPROM via UART.
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
#include <system.h>                               // system functions
#include <debug_serial.h>                         // serial debug functions
#include <i2c.h>                                  // I2C functions
#include <eeprom_24c.h>                           // 24Cxx I2C EEPROM functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DEBUG_init();                                   // init serial debug
  I2C_init();                                     // init I2C
  EEPROM_update(0, 0x55);                         // write value to address 0
  
  // Loop
  while(1) {
    uint8_t i,j;
    uint8_t addr = 0;
    DEBUG_println("EEPROM Hex Dump:");
    for(j=16; j; j--) {
      DEBUG_printH(addr); DEBUG_print(": ");
      for(i=16; i; i--) {
        DEBUG_printB(EEPROM_read(addr++)); DEBUG_write(' ');
      }
      DEBUG_println("");
    }
    DEBUG_println("");
    DLY_ms(1000);                                 // wait a second
  }
}
