// ===================================================================================
// Project:   Example for ATmega4808
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
// Displays text sent via UART (115200 BAUD, 8N1) on an SSD1306 128x64 pixels I2C
// OLED. Connect the OLED to PA2 (SDA) and PA3 (SCL).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with megaupdi (via pip)
//   are installed. If necessary, a driver for the USB-to-serial converter used by
//   the SerialUPDI programmer must be installed.
// - Connect the SerialUPDI programmer to the UPDI header of the board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "uart_int.h"               // UART with RX interrupt functions
#include "oled_term.h"              // OLED terminal functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CLK_init();                       // set system clock frequency
  UART_init();                      // init UART
  DLY_ms(100);                      // wait a bit for OLED
  OLED_init();                      // init OLED

  // Print start message
  OLED_print("UART TO OLED TERMINAL");
  OLED_print("---------------------");
  OLED_print("Ready\n");
  OLED_print("_\r");

  // Loop
  while(1) {
    OLED_write(UART_read());        // print incoming character on OLED
  }
}
