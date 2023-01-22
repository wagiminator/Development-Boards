// ===================================================================================
// Project:   Mouse Wiggler for CH551, CH552 and CH554
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
// no screen saver is activated and the PC or notebook is not put to sleep.
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
// - Connect the board via USB to your PC. It should be detected as a HID mouse.
// - Activate/deactivate the wiggle function by pressing the ACT button. The 
//   built-in LED lights up when the function is activated.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include <config.h>                       // user configurations
#include <ch554.h>                        // CH55x header file
#include <gpio.h>                         // GPIO functions
#include <clock.h>                        // system clock functions
#include <delay.h>                        // delay functions
#include <usb_mouse.h>                    // USB HID mouse functions

#pragma disable_warning 84                // disable warning: variable used before init

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
  uint8_t cnt;                            // timing counter
  CLK_config();                           // configure system clock
  DLY_ms(5);                              // wait for clock to settle
  MOUSE_init();                           // init USB HID mouse
  PIN_low(PIN_LED);                       // light up LED -> wiggling activated

  // Loop
  while(1) {
    if(!PIN_read(PIN_LED) && !--cnt) {    // wiggling activated and time to wiggle?
      MOUSE_move( 1, 0);                  // move mouse pointer just a tiny bit
      MOUSE_move(-1, 0);                  // move pointer back to where it was
    }
    if(!PIN_read(PIN_ACTKEY)) {           // ACT button pressed?
      PIN_toggle(PIN_LED);                // toggle LED and function state
      DLY_ms(10);                         // debounce
      while(!PIN_read(PIN_ACTKEY));       // wait for ACT button released
    }
    DLY_ms(100);                          // delay a little
  }
}
