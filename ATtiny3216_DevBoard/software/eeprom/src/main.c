// ===================================================================================
// Project:   Example for ATtiny3216
// Version:   v1.1
// Year:      2021
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Writes a value to EEPROM address 0 and transfers the content of the EEPROM
// via UART (115200 BAUD, 8N1) once a second.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with tinyupdi (via pip)
//   are installed. If necessary, a driver for the CH340N USB-to-serial converter
//   must be installed.
// - Set the Serial Mode Switch on the board to "UPDI" (UPDI programming mode).
// - Connect the board via USB to your PC.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "debug_serial.h"           // serial debug functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CLK_init();                       // set system clock frequency
  DEBUG_init();                     // init DEBUG (TX: PB2, BAUD: 115200, 8N1)
  EEPROM_write(0, 0x5a);            // write value to EEPROM address 0

  // Loop
  while(1) {
    uint8_t i, j;
    uint16_t addr = 0;
    DEBUG_printf("\nEEPROM Hex Dump:\n");
    for(j=EEPROM_SIZE/16; j; j--) {
      DEBUG_printf("%04x: ", addr);
      for(i=16; i; i--) DEBUG_printf("%02x ", EEPROM_read(addr++));
      DEBUG_printf("\n");
    }
    DLY_ms(1000);                   // wait a second
  }
}
