// ===================================================================================
// Project:   Example for STC8H1K08
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
// Transfers content of EEPROM via UART.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with PySerial is installed.
// - Connect the MCU board via USB to your PC.
// - Run 'make flash'.
// - Perform a power cycle (press and release DOWNLOAD button) when prompted.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "delay.h"                      // delay functions
#include "eeprom.h"                     // EEPROM functions
#include "debug_serial.h"               // serial debug functions

// ===================================================================================
// Main Function
// ===================================================================================
void main() {
  // Setup
  P_SW2 |= 0x80;                        // enable access to extended registers (MUST!)
  DEBUG_init();                         // init DEBUG (TX: P31, BAUD: 115200, 8N1)
  EEPROM_erase(0);                      // erase first sector if EEPROM
  EEPROM_write(0, 0x5a);                // write value to EEPROM address 0

  // Loop
  while (1) {
    uint8_t i,j;
    uint16_t addr = 0;
    DEBUG_printf("\nEEPROM Hex Dump:\n");
    for(j=8; j; j--) {
      DEBUG_printf("%04x: ", addr);
      for(i=16; i; i--) {
        DEBUG_printf("%02x ", EEPROM_read(addr++));
      }
      DEBUG_printf("\n");
    }
    DLY_ms(1000);
  }
}
