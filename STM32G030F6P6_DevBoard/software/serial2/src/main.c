// ===================================================================================
// Project:   Example for STM32G03x/04x
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
// Echoes data sent via USART2.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with stm32isp is
//   installed. If necessary, a driver for the USB-to-serial converter used must
//   be installed.
// - Connect your MCU board via USB to your PC.
// - Set the MCU to boot mode by holding down the BOOT key and then pressing and 
//   releasing the RESET key. Finally release the BOOT key.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"             // system functions
#include "uart2.h"              // USART2 functions

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  UART2_init();                 // init UART (TX: PA2, RX: PA3, BAUD: 115200, 8N1)
  
  // Loop
  while(1) {
    UART2_write(UART2_read());  // echo any received character
  }
}
