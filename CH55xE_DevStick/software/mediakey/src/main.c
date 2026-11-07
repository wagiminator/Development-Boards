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
// The device identifies itself as a USB HID composite device (keyboard, mouse and
// multimedia keys). Pressing the ACT button sends PLAY/PAUSE, touching the touch
// key sends MUTE. The NeoPixel lights up while a key is pressed.
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
// - Connect the board via USB to your PC. It should be detected as a HID device.
// - Press the ACT button or touch the touch key to control your media player.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "config.h"                 // user configurations
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "gpio.h"                   // GPIO functions
#include "touch.h"                  // touch key functions
#include "neo.h"                    // NeoPixel functions
#include "usb_composite.h"          // USB HID composite functions

#define CON_PLAY_PAUSE  0xCD        // consumer usage ID: play/pause

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
  CLK_config();                     // configure system clock
  NEO_init();                       // init NeoPixel
  NEO_clearAll();                   // NeoPixel off
  PIN_input_PU(PIN_ACTKEY);         // set ACT key pin as input with pullup
  TOUCH_start(PIN_TOUCH);           // start touch key
  HID_init();                       // init USB HID composite device

  // Loop
  while(1) {
    if(!PIN_read(PIN_ACTKEY)) {     // ACT key pressed?
      NEO_writeColor(0, 0, 0, 64);  // NeoPixel blue
      NEO_update();
      CON_type(CON_PLAY_PAUSE);     // send PLAY/PAUSE
      while(!PIN_read(PIN_ACTKEY)); // wait for key release
      DLY_ms(20);                   // debounce
      NEO_clearAll();               // NeoPixel off
    }
    if(TOUCH_pressed(PIN_TOUCH)) {  // touch key pressed?
      NEO_writeColor(0, 64, 0, 0);  // NeoPixel red
      NEO_update();
      CON_type(CON_VOL_MUTE);       // send MUTE
      while(!TOUCH_released(PIN_TOUCH)); // wait for touch key release
      NEO_clearAll();               // NeoPixel off
    }
  }
}
