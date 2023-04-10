// ===================================================================================
// Project:   Mouse Wiggler for CH551, CH552 and CH554
// Version:   v1.1
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
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.
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
#include "src/config.h"                   // user configurations
#include "src/system.h"                   // system functions
#include "src/gpio.h"                     // GPIO functions
#include "src/delay.h"                    // delay functions
#include "src/usb_mouse.h"                // USB HID mouse functions

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
  uint8_t cnt = 0;                        // timing counter
  CLK_config();                           // configure system clock
  DLY_ms(10);                             // wait for clock to settle
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
