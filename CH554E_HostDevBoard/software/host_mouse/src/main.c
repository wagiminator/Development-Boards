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
// Shows the movements and button states of a USB mouse connected to the USB host
// port via the serial interface. The BUSY LED lights up while a mouse button is
// pressed.
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
  CLK_config();                     // configure system clock
  DLY_ms(50);                       // wait a bit
  PIN_output(PIN_LED);              // set LED pin as output
  PIN_high(PIN_LED);                // LED off
  DEBUG_init();                     // init DEBUG (TXD1: P17, BAUD: 115200)
  printf("CH554E USB Mouse Monitor\n");
  HOST_init();                      // init USB host

  // Loop
  while(1) {
    if(HOST_update()) {             // device connected/disconnected?
      if(HOST_findMouse()) printf("\nMouse connected\n");
    }
    if(HOST_readMouse()) {          // new mouse report?
      PIN_write(PIN_LED, !HOST_mouse.buttons);  // LED on while button pressed
      printf("Buttons: %c%c%c  X: %4d  Y: %4d  Wheel: %3d\n",
        HOST_mouse.buttons & 1 ? 'L' : '-',
        HOST_mouse.buttons & 4 ? 'M' : '-',
        HOST_mouse.buttons & 2 ? 'R' : '-',
        (int)HOST_mouse.x, (int)HOST_mouse.y, (int)HOST_mouse.wheel);
    }
  }
}
