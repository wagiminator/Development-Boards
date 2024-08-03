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
// Identifies itself as a USB HID keyboard and types a message on button press.
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
// - Connect a push button to the MCU pin defined in config.h (active low).
// - Connect the board via USB to your PC. It should be detected as a HID keyboard.
// - Open a text editor and place the cursor inside the text box.
// - Press the button and you can see a message typed in by the device.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>                   // user configurations
#include <system.h>                   // system functions
#include <gpio.h>                     // GPIO functions
#include <usb_keyboard.h>             // USB HID keyboard functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup pins
  PIN_input_PU(PIN_KEY);              // button pin as input with pullup
  PIN_output(PIN_LED);                // LED pin as input
  PIN_high(PIN_LED);                  // turn off LED

  // Setup USB keyboard
  DLY_ms(1);                          // wait a bit for USB
  KBD_init();                         // init USB HID keyboard
  
  // Loop
  while(1) {
    if(!PIN_read(PIN_KEY)) {          // button pressed ?
      PIN_low(PIN_LED);               // turn on LED
      KBD_print("Hello world!");      // type message
      KBD_type(KBD_KEY_RETURN);       // press return key
      PIN_high(PIN_LED);              // turn off LED
      while(!PIN_read(PIN_KEY));      // wait for button released
      DLY_ms(10);                     // debounce
    }
  }

  // For more keyboard commands refer to usb_keyboard.h. Of course, you can also 
  // press any keyboard shortcut or other key combinations you like.
}
