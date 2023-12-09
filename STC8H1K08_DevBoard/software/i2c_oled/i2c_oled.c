// ===================================================================================
// Project:   Example for STC8H1K08
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
// Displays text sent via UART on an SSD1306 128x64 pixels OLED.
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with PySerial is installed.
// - Connect the MCU board via USB to your PC.
// - Run 'make flash'.
// - Perform a power cycle (press and release DOWNLOAD button) when prompted.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "uart1_int.h"                  // UART1 functions with interrupt
#include "oled_term.h"                  // OLED terminal functions

// Prototypes for used interrupts
void UART1_interrupt(void);
void UART1_ISR(void) __interrupt(INT_NO_UART1) {
  UART1_interrupt();
}

// ===================================================================================
// Main Function
// ===================================================================================
void main() {
  // Setup
  P_SW2 |= 0x80;                        // enable access to extended registers (MUST!)
  UART1_init();                         // init UART (RX: P30, TX: P31, 8N1, BAUD: 9600)
  OLED_init();                          // init OLED (SDA: P14, SCL: P15)

  // Print start message
  OLED_print("UART TO OLED TERMINAL");
  OLED_print("---------------------");
  OLED_print("Ready\n");
  OLED_print("_\r");

  // Loop
  while(1) {
    OLED_write(UART1_read());           // print incoming character on OLED
  }
}
