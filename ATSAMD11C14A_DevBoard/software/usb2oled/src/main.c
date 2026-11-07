// ===================================================================================
// Project:   Example for ATSAMD11C14A
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
// This code implements a simple USB controlled 128x64 pixels I2C OLED display.
// The MCU builds a USB communication device class (CDC) for serial communication
// via USB. Text messages of all kinds can be sent via the USB interface and shown
// on the OLED display. Connect an SSD1306 128x64 pixels I2C OLED to PA14 (SDA) and
// PA15 (SCL).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with tinyocd (via pip)
//   are installed. If necessary, grant access permissions for the SWD programmer.
// - Connect the SWD programmer (e.g. CMSIS-DAP) to the SWD pins of the board.
// - Run 'make flash'.
// - Alternatively, if the SAM-BA bootloader is installed, set LDSCRIPT and ISPTOOL
//   in the makefile accordingly (see readme) and upload the firmware via USB.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't
//   matter).
// - Send a text message, it should be displayed on the OLED.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "usb_cdc.h"                // USB CDC serial functions
#include "oled_term.h"              // OLED terminal functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CDC_init();                       // init USB CDC
  OLED_init();                      // init OLED

  // Print start message
  OLED_print("USB CDC OLED TERMINAL");
  OLED_print("---------------------");
  OLED_print("Ready\n");
  OLED_print("_\r");

  // Loop
  while(1) OLED_write(CDC_read());  // send incoming characters to OLED
}
