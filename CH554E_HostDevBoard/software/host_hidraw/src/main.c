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
// Shows the raw HID reports of any USB HID device (keyboard, mouse, gamepad,
// joystick, game controller, etc.) connected to the USB host port via the serial
// interface. Useful for analyzing unknown HID devices.
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
  uint8_t i, len, ep;
  CLK_config();                     // configure system clock
  DLY_ms(50);                       // wait a bit
  PIN_output(PIN_LED);              // set LED pin as output
  PIN_high(PIN_LED);                // LED off
  DEBUG_init();                     // init DEBUG (TXD1: P17, BAUD: 115200)
  printf("CH554E USB HID Report Monitor\n");
  HOST_init();                      // init USB host

  // Loop
  while(1) {
    if(HOST_update()) {             // device connected/disconnected?
      if(HOST_findHID()) printf("\nHID device connected\n");
    }
    for(ep=0; ep<4; ep++) {         // poll all interrupt endpoints of the device
      len = HOST_readHID(ep);       // read report (0: no new report)
      if(len) {
        PIN_low(PIN_LED);           // LED on
        printf("EP%d:", (int)ep);
        for(i=0; i<len; i++) printf(" %02X", (int)HOST_buffer[i]);
        printf("\n");
        PIN_high(PIN_LED);          // LED off
      }
    }
  }
}
