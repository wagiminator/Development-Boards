// ===================================================================================
// Project:   Example for ATtiny814
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
// OLED. Connect the OLED to PB1 (SDA) and PB0 (SCL).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with tinyupdi (via pip)
//   are installed. If necessary, a driver for the CH340N USB-to-serial converter
//   must be installed.
// - Set the Serial Mode Switch on the board to "P" (UPDI programming mode).
// - Connect the board via USB to your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Set the Serial Mode Switch to UART mode.
// - Open a serial monitor (115200 BAUD) and send some text.


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
