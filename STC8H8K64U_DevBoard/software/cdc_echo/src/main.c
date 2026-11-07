// ===================================================================================
// Project:   Example for STC8H8K64U
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
// Simple USB-CDC serial demo which echoes back all received messages.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with stc8usb is installed.
// - Press and hold BOOT button while connecting the MCU board to your PC via USB.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't 
//   matter).
// - Send a text message (with newline termination), it should be echoed back by
//   the device.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "gpio.h"                       // GPIO functions
#include "usb_cdc.h"                    // delay functions

// Prototypes for used interrupts
void USB_ISR(void) __interrupt(INT_NO_USB) { USB_interrupt(); }

// ===================================================================================
// Main Function
// ===================================================================================

void main() {
  // Setup
  P_SW2 |= 0x80;                        // enable access to extended registers (MUST!)
  CDC_init();                           // init USB CDC

  // Loop
  while(1) {
    char c = CDC_read();                // read incoming character
    CDC_write(c);                       // echo back character
    if(c == '\n') CDC_flush();          // flush on newline command
  }
}
