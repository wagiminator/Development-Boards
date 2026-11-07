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
// Shows the state of the CAPS LOCK key of a USB keyboard connected to the USB host
// port on the BUSY LED of the board and reports changes via the serial interface.
// The lock keys also switch the LEDs on the keyboard.
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
  uint8_t state = 0xFF;
  CLK_config();                     // configure system clock
  DLY_ms(50);                       // wait a bit
  PIN_output(PIN_LED);              // set LED pin as output
  PIN_high(PIN_LED);                // LED off
  DEBUG_init();                     // init DEBUG (TXD1: P17, BAUD: 115200)
  printf("CH554E USB Keyboard Lock State\n");
  HOST_init();                      // init USB host

  // Loop
  while(1) {
    if(HOST_update()) {             // device connected/disconnected?
      if(HOST_findKeyboard()) printf("\nKeyboard connected\n");
    }
    HOST_readKey();                 // process keyboard reports
    if(HOST_kbdLEDs != state) {     // lock state changed?
      state = HOST_kbdLEDs;
      PIN_write(PIN_LED, !(state & HOST_LED_CAPS));  // CAPS LOCK -> BUSY LED
      printf("NUM: %s  CAPS: %s  SCROLL: %s\n",
        state & HOST_LED_NUM    ? "ON " : "OFF",
        state & HOST_LED_CAPS   ? "ON " : "OFF",
        state & HOST_LED_SCROLL ? "ON " : "OFF");
    }
  }
}
