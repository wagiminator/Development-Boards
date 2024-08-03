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
// Mouse Wiggler is a tool that simulates movements of the mouse pointer so that 
// no screen saver is activated and the PC or notebook is not put to sleep. The
// mouse pointer movements are imperceptibly small, so that you can continue to
// work normally on your PC even with the function activated.
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
// - Connect the board via USB to your PC. It should be detected as a HID mouse.
// - The wiggle function is automatically activated.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                   // system functions
#include "usb_mouse.h"                // USB HID mouse functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  MOUSE_init();                       // init USB HID mouse

  // Loop
  while(1) {
    DLY_ms(30000);                    // wait half a minute
    MOUSE_move( 1, 0);                // move mouse pointer just a tiny bit
    MOUSE_move(-1, 0);                // move pointer back to where it was
  }
}
