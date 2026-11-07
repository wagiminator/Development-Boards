// ===================================================================================
// Project:   Example for CH55x
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
// Controls the color of the NeoPixel via USB CDC (virtual serial port). Send
// "r", "g", "b", "w" or "0" to set the color to red, green, blue, white or off, or
// a digit "1" to "9" to set a hue value. The current color is reported back.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with chprog (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the board and keep it pressed while connecting it via
//   USB to your PC.
// - Run 'make flash' immediatly afterwards.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't
//   matter).
// - Send the commands described above.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "config.h"                 // user configurations
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "neo.h"                    // NeoPixel functions
#include "usb_cdc.h"                // USB CDC serial functions

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
  char c;
  CLK_config();                     // configure system clock
  NEO_init();                       // init NeoPixel
  NEO_clearAll();                   // NeoPixel off
  CDC_init();                       // init USB CDC

  // Loop
  while(1) {
    c = CDC_read();                 // wait for command character
    switch(c) {
      case 'r': NEO_writeColor(0, 64,  0,  0); CDC_println("red");   break;
      case 'g': NEO_writeColor(0,  0, 64,  0); CDC_println("green"); break;
      case 'b': NEO_writeColor(0,  0,  0, 64); CDC_println("blue");  break;
      case 'w': NEO_writeColor(0, 48, 48, 48); CDC_println("white"); break;
      case '0': NEO_writeColor(0,  0,  0,  0); CDC_println("off");   break;
      default:
        if((c >= '1') && (c <= '9')) {
          NEO_writeHue(0, (c - '1') * 21, 1);
          CDC_println("hue");
        }
        else continue;
    }
    NEO_update();                   // update NeoPixel
  }
}
