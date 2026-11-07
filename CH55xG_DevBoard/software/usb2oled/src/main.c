// ===================================================================================
// Project:   Example for CH55x
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// This code implements a simple USB controlled 128x64 pixels I2C OLED display.
// The MCU builds a USB communication device class (CDC) for serial communication
// via USB. Text messages of all kinds can be sent via the USB interface and shown
// on the OLED display. Connect an SSD1306 OLED to P1.6 (SDA) and P1.7 (SCL).
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with chprog (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the board and keep it pressed while connecting it via
//   USB to your PC.
// - Run 'make flash' immediatly afterwards.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't
//   matter).
// - Send a text message, it should be displayed on the OLED.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "config.h"                 // user configurations
#include "system.h"                 // system functions
#include "usb_cdc.h"                // USB CDC serial functions
#include "oled_term.h"              // OLED terminal functions

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
  CLK_config();                     // configure system clock
  CDC_init();                       // init USB CDC
  OLED_init();                      // init OLED

  // Print start message
  OLED_print("USB CDC OLED TERMINAL");
  OLED_print("---------------------");
  OLED_print("Ready\n");
  OLED_print("_\r");

  // Loop
  while(1) OLED_write(CDC_read());  // send incoming characters to OLED
}
