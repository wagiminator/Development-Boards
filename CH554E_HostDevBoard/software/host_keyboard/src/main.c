// ===================================================================================
// Project:   Example for CH554E
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
// USB keyboard to serial terminal. Characters typed on a USB keyboard connected to
// the USB host port are sent via the serial interface. The BUSY LED flashes with
// each keystroke. NUM LOCK, CAPS LOCK and SCROLL LOCK are handled and shown on the
// keyboard LEDs.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with chprog (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the board and keep it pressed while connecting the
//   USB-A female connector (USB host port) to your PC using a USB-A male-to-male
//   cable.
// - Run 'make flash' immediatly afterwards.
//
// Operating Instructions:
// -----------------------
// - Connect the USB-A male connector of the board to your PC and open a serial
//   monitor (115200 BAUD, 8N1) with the serial port of the CH340N.
// - Connect a USB device to the USB-A female connector (USB host port).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "gpio.h"                   // GPIO functions
#include "debug_serial.h"           // serial debug functions
#include "usb_hid_host.h"           // USB HID host functions
#include <stdio.h>                  // for printf

#define PIN_LED   P14               // BUSY LED pin (active low)

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  char c;
  CLK_config();                     // configure system clock
  DLY_ms(50);                       // wait a bit
  PIN_output(PIN_LED);              // set LED pin as output
  PIN_high(PIN_LED);                // LED off
  DEBUG_init();                     // init DEBUG (TXD1: P17, BAUD: 115200)
  printf("CH554E USB Keyboard to Serial\n");
  HOST_init();                      // init USB host

  // Loop
  while(1) {
    if(HOST_update()) {             // device connected/disconnected?
      if(HOST_findKeyboard()) printf("\nKeyboard connected\n");
    }
    c = HOST_readKey();             // read key from keyboard (0: no key)
    if(c) {
      PIN_low(PIN_LED);             // LED on
      if(c == '\r') DEBUG_write('\n');  // convert ENTER to newline
      else          DEBUG_write(c);      // send character via UART
      PIN_high(PIN_LED);            // LED off
    }
  }
}
