// ===================================================================================
// Project:   Example for CH32X035/X034/X033
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
// This code implements a simple USB controlled 128x64 pixels I2C OLED display.
// The CH32X03x builds a USB communication device class (CDC) for serial communication
// via USB. Text messages of all kinds can be sent via the USB interface and shown
// on the OLED display.
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the I2C OLED to the board (define pins in config.h).
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't 
//   matter).
// - Send a text message, it should be displayed on the OLED.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                       // system functions
#include "usb_cdc.h"                      // USB CDC serial functions
#include "oled_term.h"                    // OLED terminal functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CDC_init();                             // init USB CDC
  OLED_init();                            // init OLED

  // Print start message
  OLED_print("USB CDC OLED TERMINAL");
  OLED_print("---------------------");
  OLED_print("Ready\n");
  OLED_print("_\r");

  // Loop
  while(1) OLED_write(CDC_read());        // send incoming characters to OLED
}
