// ===================================================================================
// Project:   Example for PY32F0xx
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
// Echoes data sent via UART.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with PySerial is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect the development board via USB to your PC.
// - Set the MCU to boot mode by holding down the BOOT key and then pressing and 
//   releasing the RESET key.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"
#include "uart.h"

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  UART_init();                // init UART (TX: PA2, RX: PA3, BAUD: 115200, 8N1)
  
  // Loop
  while(1) {
    UART_write(UART_read());  // echo any received character
  }
}
