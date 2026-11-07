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
// Barcode scanner interface. Most USB barcode scanners identify themselves as a
// USB keyboard and "type" the scanned code followed by ENTER. This example collects
// the characters of each scanned code and sends it as a numbered line via the
// serial interface. The BUSY LED flashes on each scan.
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
// - Connect a USB barcode scanner to the USB-A female connector (USB host port).


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

#define CODE_MAX  48                // maximum length of a code

__xdata char code[CODE_MAX + 1];    // buffer for scanned code

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  char c;
  uint8_t  len = 0;
  uint16_t cnt = 0;
  CLK_config();                     // configure system clock
  DLY_ms(50);                       // wait a bit
  PIN_output(PIN_LED);              // set LED pin as output
  PIN_high(PIN_LED);                // LED off
  DEBUG_init();                     // init DEBUG (TXD1: P17, BAUD: 115200)
  printf("CH554E USB Barcode Scanner Interface\n");
  HOST_init();                      // init USB host

  // Loop
  while(1) {
    if(HOST_update()) {             // device connected/disconnected?
      if(HOST_findKeyboard()) printf("Scanner connected\n");
      len = 0;
    }
    c = HOST_readKey();             // read character from scanner (0: none)
    if(!c) continue;
    if((c == '\r') || (c == '\t')) {   // end of code?
      if(len) {
        code[len] = 0;
        PIN_low(PIN_LED);           // LED on
        printf("%u: %s\n", ++cnt, code);
        PIN_high(PIN_LED);          // LED off
        len = 0;
      }
    }
    else if(len < CODE_MAX) code[len++] = c;
  }
}
