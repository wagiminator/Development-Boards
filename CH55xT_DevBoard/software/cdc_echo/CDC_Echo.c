// ===================================================================================
// Project:   USB-CDC Echo Demo for CH551, Ch552, CH554
// Version:   v1.0
// Year:      2022
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Simple USB-CDC demo which echoes back all received messages.
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
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
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

// Libraries
#include <ch554.h>                        // CH55x header file
#include <clock.h>                        // system clock functions
#include <usb_cdc.h>                      // for USB-CDC serial

// Prototypes for used interrupts
void USB_interrupt(void);
void DeviceUSBInterrupt(void) __interrupt (INT_NO_USB) {
  USB_interrupt();
}

// Global variables
__xdata char message[64];                 // stores the received message string
uint8_t msgptr = 0;                       // message string pointer
uint8_t complete = 0;                     // message string complete flag


// ===================================================================================
// Main Function
// ===================================================================================

void main(void) {
  // Setup
  CLK_config();                           // configure system clock
  USB_init();                             // init USB

  // Loop
  while(1) {
    while (CDC_available()) {             // incoming data?
      char serialChar = CDC_read();       // read incoming character
      if (serialChar == '\n') {           // is it newline command?
        message[msgptr] = '\0';           // add string terminator
        if (msgptr > 0) {                 // string not empty?
          complete = 1;                   // set string complete flag
          break;                          // exit loop
        }
      }
      else {                              // normal character? (not newline command)
        message[msgptr++] = serialChar;   // add the character to the string
        if (msgptr == 63) {               // reached end of string buffer?
          message[msgptr] = '\0';         // add string terminator
          complete = 1;                   // set string complete flag
          break;                          // exit loop
        }
      }
    }

    if (complete) {                       // string complete?
      CDC_print("ECHO: ");                // send 'ECHO:'
      CDC_println(message);               // send received message string
      CDC_flush();                        // flush
      complete = 0;                       // clear string complete flag
      msgptr = 0;                         // reset pointer
    }
  }
}
