// ===================================================================================
// Project:   CAPS bLOCK for CH551, CH552 and CH554
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
// Tired of accidentally hitting the CAPS-LOCK key? It doesn't have to be. 
// CAPS bLOCK identifies itself as a USB HID keyboard and constantly monitors the
// state of CAPS LOCK. It immediately deactivates it if it has been activated.
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
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - Activate/deactivate the blocking function by pressing the ACT button. The 
//   built-in LED lights up when the function is activated.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/config.h"                   // user configurations
#include "src/system.h"                   // system functions
#include "src/gpio.h"                     // GPIO functions
#include "src/delay.h"                    // delay functions
#include "src/usb_keyboard.h"             // USB HID keyboard functions

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
  CLK_config();                           // configure system clock
  DLY_ms(10);                             // wait for clock to settle
  KBD_init();                             // init USB HID keyboard
  PIN_low(PIN_LED);                       // light up LED - blocking activated

  // Loop
  while(1) {
    if(!PIN_read(PIN_LED)                 // blocking function activated
       && KBD_CAPS_LOCK_state) {          // and CAPS LOCK was pressed?
        KBD_type(KBD_KEY_CAPS_LOCK);      // press CAPS LOCK to deactivate
        DLY_ms(50);                       // wait a bit
    }

    if(!PIN_read(PIN_ACTKEY)) {           // ACT button pressed?
      PIN_toggle(PIN_LED);                // toggle LED and function state
      DLY_ms(10);                         // debounce
      while(!PIN_read(PIN_ACTKEY));       // wait for ACT button released
      DLY_ms(10);                         // debounce
    }
  }
}
