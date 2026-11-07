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
// The MCU builds a USB communication device class (CDC) for serial communication
// via USB. Any character received is sent back (echo).
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
// - Send a text message, it should be echoed by the device.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "usb_cdc.h"                // USB CDC serial functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CDC_init();                       // init USB CDC

  // Loop
  while(1) {
    char c = CDC_read();            // read incoming character
    CDC_write(c);                   // echo back character
    if(c == '\n') CDC_flush();      // flush on newline command
  }
}
