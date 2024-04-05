// ===================================================================================
// Project:   Example for CH32V003
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
// Identifies itself as a USB HID keyboard and types a message every now and then.
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - CNLohr rv003usb:    https://github.com/cnlohr/rv003usb
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Open a text editor and place the cursor inside the text box.
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - You can see a message typed in by the device.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                   // system functions
#include "usb_keyboard.h"             // USB HID keyboard functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DLY_ms(1);                          // wait a bit for USB
  KBD_init();                         // init USB HID keyboard
  
  // Loop
  while(1) {
    DLY_ms(1500);                     // wait one and a half second
    KBD_print("Hello world!");        // type message
    KBD_type(KBD_KEY_RETURN);         // press return key
    // For more keyboard commands refer to usb_keyboard.h. Of course, you can also 
    // press any keyboard shortcut or other key combinations you like.
  }
}
