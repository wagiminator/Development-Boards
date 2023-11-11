// ===================================================================================
// Project:   Example for STM32L010
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
// Transfers content of data EEPROM via UART.
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
#include "eeprom.h"                 // EEPROM functions
#include "debug_serial.h"           // serial debug functions

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  DEBUG_init();                     // init DEBUG (TX: PA9, BAUD: 115200, 8N1)
  EEPROM_write8(0, 0x55);           // write 8-bit byte to EEPROM address 0
  
  // Loop
  while(1) {
    uint8_t i,j;
    uint16_t addr = 0;
    DEBUG_println("Data Flash Hex Dump:");
    for(j=8; j; j--) {
      DEBUG_printH(addr); DEBUG_print(": ");
      for(i=16; i; i--) {
        DEBUG_printB(EEPROM_read8(addr++)); DEBUG_write(' ');
      }
      DEBUG_newline();
    }
    DEBUG_newline();
    DLY_ms(1000);
  }
}
