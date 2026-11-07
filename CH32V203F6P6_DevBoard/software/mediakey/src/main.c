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
// The device identifies itself as a USB HID consumer control device (multimedia
// keyboard). A short press of the BOOT0 button (PB8) sends PLAY/PAUSE, a long
// press (more than half a second) sends MUTE.
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
// - Connect the board via USB to your PC. It should be detected as a HID device.
// - Press the BOOT0 button to control your media player.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "usb_consumer.h"           // USB HID consumer functions

#define PIN_KEY   PB8               // BOOT0 button (active high)
#define CON_PLAY_PAUSE  0xCD        // consumer usage ID: play/pause
#define PIN_LED   PB1               // LED pin (active low)

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  uint16_t duration;
  PIN_input_PD(PIN_KEY);            // set key pin as input with pulldown
  PIN_output(PIN_LED);              // set LED pin as output
  PIN_high(PIN_LED);                // LED off
  HID_init();                       // init USB HID consumer device

  // Loop
  while(1) {
    if(PIN_read(PIN_KEY)) {         // key pressed?
      PIN_low(PIN_LED);             // LED on
      duration = 0;
      while(PIN_read(PIN_KEY)) {    // wait for key release
        DLY_ms(10);
        if(duration < 1000) duration += 10;
      }
      if(duration < 500) CON_type(CON_PLAY_PAUSE);  // short press
      else               CON_type(CON_VOL_MUTE);          // long press
      PIN_high(PIN_LED);            // LED off
      DLY_ms(20);                   // debounce
    }
  }
}
