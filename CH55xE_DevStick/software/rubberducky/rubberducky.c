// ===================================================================================
// Project:   Rubber Ducky for CH551, CH552 and CH554
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
// Identifies itself as a USB HID keyboard and types a message when the ACT key is
// pressed. It can be used to control the PC via keyboard shortcuts. The built-in
// LED shows the status of CAPS LOCK (just for demonstration).
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: min. 6 MHz internal
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - Open a text editor und press the ACT button on the board.
// - The built-in LED can be controlled by the CAPS LOCK key on your regular keyboard.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include <config.h>                       // user configurations
#include <system.h>                       // system functions
#include <gpio.h>                         // GPIO functions
#include <delay.h>                        // delay functions
#include <usb_keyboard.h>                 // USB HID keyboard functions

// Prototypes for used interrupts
void USB_interrupt(void);
void DeviceUSBInterrupt(void) __interrupt (INT_NO_USB) {
  USB_interrupt();
}

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                           // configure system clock
  DLY_ms(5);                              // wait for clock to settle
  KBD_init();                             // init USB HID keyboard

  // Loop
  while(1) {
    if(!PIN_read(PIN_ACTKEY)) {           // ACT button pressed?
      KBD_print("Hello World!");          // type message
      KBD_type(KBD_KEY_RETURN);           // press return key
      while(!PIN_read(PIN_ACTKEY));       // wait for ACT button released
      DLY_ms(10);                         // debounce
    }
    PIN_write(PIN_LED, !KBD_CAPS_LOCK_state); // set built-in LED according to CAPS LOCK state
  }
}
