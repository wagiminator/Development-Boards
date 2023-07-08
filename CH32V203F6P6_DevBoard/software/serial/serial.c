// ===================================================================================
// Project:   UART Serial Demo for CH32V203
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
// Echoes data sent via UART. TX pin: PA2, RX pin: PA3.
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>             // system functions
#include <gpio.h>               // GPIO functions
#include <uart.h>               // UART functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  UART2_init();                 // init USART2 with default BAUD rate (115200)
  
  // Loop
  while(1) {
    UART2_write(UART2_read());  // echo any received character
  }
}
