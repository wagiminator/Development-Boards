// ===================================================================================
// Project:   Example for STC8H8K64U
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
// - Make sure SDCC toolchain and Python3 with stc8usb (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press and hold the BOOT button while connecting the board via USB to your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID mouse.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "usb_mouse.h"              // USB HID mouse functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  uint8_t i;
  P_SW2 |= 0x80;                    // enable access to extended registers (MUST!)
  MOUSE_init();                     // init USB HID mouse

  // Loop
  while(1) {
    for(i=30; i; i--) DLY_ms(1000); // wait half a minute
    MOUSE_move( 1, 0);              // move mouse pointer just a tiny bit
    MOUSE_move(-1, 0);              // move pointer back to where it was
  }
}
