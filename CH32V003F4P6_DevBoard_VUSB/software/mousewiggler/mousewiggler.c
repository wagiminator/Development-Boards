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
// Mouse Wiggler is a tool that simulates movements of the mouse pointer so that 
// no screen saver is activated and the PC or notebook is not put to sleep. The
// mouse pointer movements are imperceptibly small, so that you can continue to
// work normally on your PC even with the function activated.
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
// - Connect the board via USB to your PC. It should be detected as a HID device.
// - The wiggle function is automatically activated.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"
#include "rv003usb.h"

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DLY_ms(1);
  usb_setup();

  // Loop
  while(1) {

  }
}

// ===================================================================================
// USB User Handle Functions
// ===================================================================================
void usb_handle_user_in_request(struct usb_endpoint * e, uint8_t * scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal * ist) {

  // Mouse
  if(endp == 1) {
    static uint16_t counter;
    static uint8_t mousereport[4] = { 0, 0, 0, 0 };

    // Move the mouse back and forth occasionally.
    switch((counter++) & 0x0fff) {
      case 0:   mousereport[1] =  1; break;
      case 1:   mousereport[1] = -1; break;
      default:  mousereport[1] =  0; break;
    }
    usb_send_data(mousereport, 4, 0, sendtok);
  }

  // Keyboard
  else if(endp == 2) {
    // No keys in this case
    static uint8_t keyreport[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };
    usb_send_data(keyreport, 8, 0, sendtok);
  }

  // Control transfer
  else usb_send_empty(sendtok);
}
