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
// Blink built-in BUSY LED using software resets. The number of resets is kept
// across resets and sent via UART1 (115200 BAUD, 8N1).
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with chprog (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the board and keep it pressed while connecting the
//   USB-A female connector (USB host port) to your PC using a USB-A male-to-male
//   cable.
// - Run 'make flash' immediatly afterwards.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "gpio.h"                   // GPIO functions
#include "debug_serial.h"           // serial debug functions

#define PIN_LED   P14               // LED pin (active low)

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  uint8_t cnt;
  CLK_config();                     // configure system clock
  DEBUG_init();                     // init DEBUG (TXD1: P17, BAUD: 115200)
  PIN_output(PIN_LED);              // set LED pin as output
  cnt = RST_wasSOFT() ? RST_getKeep() + 1 : 0;  // number of software resets
  DEBUG_print("Software resets: ");
  DEBUG_printD(cnt);
  DEBUG_println("");

  // Blink and reset
  DLY_ms(200);                      // wait a bit
  PIN_toggle(PIN_LED);              // toggle LED
  DLY_ms(200);                      // wait a bit
  RST_keep(cnt);                    // keep counter value
  RST_now();                        // conduct software reset
}
