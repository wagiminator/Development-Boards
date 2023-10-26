// ===================================================================================
// Project:   OLED Terminal Demo for CH32V003
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
// Displays text sent via UART on an SSD1306 128x64 pixels OLED.
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
#include <system.h>           // system functions
#include <gpio.h>             // GPIO functions
#include <uart_dma_rx.h>      // UART RX functions
#include <oled_term.h>        // OLED terminal functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  UART_init();                // init UART
  OLED_init();                // init OLED
  
  // Print start message
  OLED_print("UART TO OLED TERMINAL");
  OLED_print("---------------------");
  OLED_print("Ready\n");
  OLED_print("_\r");

  // Loop
  while(1) {
    OLED_write(UART_read());  // print incoming character on OLED
  }
}
