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
// Sends a message via software-serial once a second.
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
#include "delay.h"                      // delay functions
#include "uart_tx_soft.h"               // software serial TX

// ===================================================================================
// printf Functionality
// ===================================================================================
// Libraries
#include <stdio.h>

// printf configuration
#define printf printf_tiny
//#define ALWAYS_PRINT_UNSIGNED

// putchar function for printf
#if SDCC < 370
void putchar(char c) { SUART_write((uint8_t)c); }
#else
int putchar(int c) { SUART_write((uint8_t)c); return c; }
#endif

// ===================================================================================
// Main Function
// ===================================================================================
void main() {
  // Setup
  P_SW2 |= 0x80;                        // enable access to extended registers (MUST!)
  SUART_init();                         // init soft-serial (TX: P31, BAUD: 115200, 8N1)

  // Loop
  while (1) {
    printf("Hello world!\n");           // send message via UART
    DLY_ms(1000);                       // wait a bit
  }
}
