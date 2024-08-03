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
// Controls PC volume via rotary encoder.
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
// - Connect a rotary encoder to the MCU pins defined in config.h (active low).
// - Connect the board via USB to your PC. It should be detected as a HID device.
// - Turn the rotary encoder to increase/decrease volume.
// - Press rotary encoder button to mute/unmute.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <config.h>                               // user configurations
#include <system.h>                               // system functions
#include <gpio.h>                                 // GPIO functions
#include <usb_consumer.h>                         // USB consumer multimedia keyboard

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Variables
  uint8_t isSwitchPressed = 0;                            // state of rotary encoder switch

  // Setup
  PIN_input_PU(PIN_ENC_A);                                // set encoder pins to input pullup
  PIN_input_PU(PIN_ENC_B);
  PIN_input_PU(PIN_ENC_SW);

  // Init USB keyboard
  DLY_ms(1);                                              // wait a bit for USB
  CON_init();                                             // init USB HID consumer keyboard

  // Loop
  while(1) {
    if(!PIN_read(PIN_ENC_A)) {                            // encoder turned ?
      if(PIN_read(PIN_ENC_B)) {                           // clockwise?
        CON_press(CON_VOL_UP);                            // -> press volume up
      }
      else {                                              // counter-clockwise?
        CON_press(CON_VOL_DOWN);                          // -> press volume down
      }
      DLY_ms(10);                                         // debounce/USP poll wait
      CON_release();                                      // release key
      while(!PIN_read(PIN_ENC_A));                        // wait until next detent
    } 
    else {
      if(!isSwitchPressed && !PIN_read(PIN_ENC_SW)) {     // switch previously pressed?
        CON_press(CON_VOL_MUTE);                          // press volume mute key
        isSwitchPressed = 1;                              // update switch state
      }
      else if(isSwitchPressed && PIN_read(PIN_ENC_SW)) {  // switch previously released?
        CON_release();                                    // release volume mute key
        isSwitchPressed = 0;                              // update switch state
      }
    }
    DLY_ms(1);                                            // debounce/USP poll wait
  }
}
