// ===================================================================================
// Project:   Example for CH554E
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Increments a boot counter in the data flash (EEPROM) at each start and sends a
// hex dump of the 128 bytes data flash via UART1 (115200 BAUD, 8N1).
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with chprog (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the board and keep it pressed while connecting the
//   USB-A female connector (USB host port) to your PC using a USB-A male-to-male
//   cable.
// - Run 'make flash' immediatly afterwards.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "debug_serial.h"           // serial debug functions
#include "eeprom.h"                 // data flash functions

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  uint8_t i, j, addr;
  CLK_config();                     // configure system clock
  DEBUG_init();                     // init DEBUG (TXD1: P17, BAUD: 115200)
  EEPROM_write(0, EEPROM_read(0) + 1);  // increment boot counter at address 0

  // Loop
  while(1) {
    DEBUG_print("\nBoot counter: ");
    DEBUG_printD(EEPROM_read(0));
    DEBUG_println("\nData flash hex dump:");
    addr = 0;
    for(j=0; j<8; j++) {
      DEBUG_printB(addr); DEBUG_print(": ");
      for(i=0; i<16; i++) {
        DEBUG_printB(EEPROM_read(addr++));
        DEBUG_write(' ');
      }
      DEBUG_println("");
    }
    DLY_ms(3000);                   // wait 3 seconds
  }
}
