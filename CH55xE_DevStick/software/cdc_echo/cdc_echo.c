// ===================================================================================
// Project:   USB-CDC Echo Demo for CH551, CH552 and CH554
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

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                           // configure system clock
  CDC_init();                             // init USB CDC

  // Loop
  while(1) {
    char c = CDC_read();                  // read incoming character
    CDC_write(c);                         // echo back character
    if(c == '\n') CDC_flush();            // flush on newline command
  }
}
