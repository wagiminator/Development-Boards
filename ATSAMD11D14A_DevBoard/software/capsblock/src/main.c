// ===================================================================================
// Project:   Example for ATSAMD11D14A
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
// Tired of accidentally hitting the CAPS-LOCK key? It doesn't have to be.
// CAPS bLOCK identifies itself as a USB HID keyboard and constantly monitors the
// state of CAPS LOCK. It immediately deactivates it if it has been activated.
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
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - The blocking function is automatically activated.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "usb_keyboard.h"           // USB HID keyboard functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  KBD_init();                       // init USB HID keyboard

  // Loop
  while(1) {
    if(KBD_CAPS_LOCK_state) {       // CAPS LOCK was pressed?
      KBD_type(KBD_KEY_CAPS_LOCK);  // press CAPS LOCK again to deactivate
      DLY_ms(30);                   // wait a bit
    }
  }
}
