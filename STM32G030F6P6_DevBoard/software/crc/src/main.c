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
// Send a message through UART with a newline at the end, and the CRC of the message 
// will be sent back. We're checking if the CRC module works.
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
#include "system.h"                 // system functions
#include "uart2.h"                  // USART2 functions

// ===================================================================================
// Main Function
// ===================================================================================
int main (void) {
  // Setup
  UART2_init();                     // init USART2, 8N1, BAUD: 115200, PA2/PA3
  CRC_enable();                     // enable CRC module

  // Loop
  while(1) {
    char c = UART2_read();          // read character from UART
    if(c != '\n') CRC_write8(c);    // not newline -> add 8-bit to CRC calculation
    else {                          // newline ->
      UART2_print("CRC: ");         // 
      UART2_printB(CRC_read8());    //   read CRC calculation and send it via UART
      UART2_newline();              //   print newline
      CRC_reset();                  //   reset CRC calculation
    }
  }
}
