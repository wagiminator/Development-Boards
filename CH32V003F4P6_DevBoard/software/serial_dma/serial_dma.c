// ===================================================================================
// Project:   UART DMA Serial Demo for CH32V003
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
// Echoes data sent via UART. Uses DMA for UART RX.
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>           // system functions
#include <gpio.h>             // GPIO functions
#include <uart_dma.h>         // UART functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  UART_init();                // init UART with default BAUD rate (115200)
  
  // Loop
  while(1) {
    UART_write(UART_read());  // echo any received character
  }
}
