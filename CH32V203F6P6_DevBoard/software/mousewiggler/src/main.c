// ===================================================================================
// Project:   Example for CH32V203
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
// Keeps the PC awake: the device identifies itself as a USB HID mouse and moves
// the mouse pointer a tiny bit every 30 seconds.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with
//   chprog (via pip) are installed. In addition, Linux requires access rights to
//   the USB bootloader.
// - Press the BOOT0 button on the board and keep it pressed while connecting it via
//   USB to your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID mouse.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "usb_mouse.h"              // USB HID mouse functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  MOUSE_init();                     // init USB HID mouse

  // Loop
  while(1) {
    DLY_ms(30000);                  // wait half a minute
    MOUSE_move( 1, 0);              // move mouse pointer just a tiny bit
    MOUSE_move(-1, 0);              // move pointer back to where it was
  }
}
