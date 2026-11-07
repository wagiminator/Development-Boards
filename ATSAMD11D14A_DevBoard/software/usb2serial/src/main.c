// ===================================================================================
// Project:   Example for ATSAMD11D14A
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
// Simple USB-to-serial converter. The MCU builds a USB communication device class
// (CDC) and forwards the data between USB and UART (TX: PA8, RX: PA9). The BAUD
// rate, the parity and the number of stop bits are set by the PC. The built-in LED
// shows the state of the RTS line.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with tinyocd (via pip)
//   are installed. If necessary, grant access permissions for the SWD programmer.
// - Connect the SWD programmer (e.g. CMSIS-DAP) to the SWD pins of the board.
// - Run 'make flash'.
// - Alternatively, if the SAM-BA bootloader is installed, set LDSCRIPT and ISPTOOL
//   in the makefile accordingly (see readme) and upload the firmware via USB.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Connect the UART pins to the device you want to communicate with (TX to RX,
//   RX to TX, GND to GND).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "usb_cdc.h"                // USB CDC serial functions
#include "uart_int.h"               // UART with RX interrupt functions

#define PIN_LED   PA16              // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Variables
  uint32_t baudrate = 115200;
  uint8_t  stopbits = 0;
  uint8_t  parity   = 0;

  // Setup
  PIN_output(PIN_LED);              // LED pin to output
  CDC_init();                       // init USB CDC
  UART_init();                      // init UART

  // Loop
  while(1) {
    // Handle data transmission
    if(CDC_available() && UART_ready()) UART_write(CDC_read());
    if(UART_available() && CDC_getDTR()) {
      while(UART_available()) CDC_write(UART_read());
      CDC_flush();
    }

    // Handle control line state
    PIN_write(PIN_LED, CDC_getRTS());

    // Handle line coding - BAUD rate
    if(CDC_getBAUD() && (CDC_getBAUD() != baudrate)) {
      baudrate = CDC_getBAUD();
      UART_setBAUD(baudrate);
    }

    // Handle line coding - parity
    if(CDC_getParity() != parity) {
      parity = CDC_getParity();
      switch(parity) {
        case 1:  UART_setOddParity();  break;
        case 2:  UART_setEvenParity(); break;
        default: UART_setNoParity();   break;
      }
    }

    // Handle line coding - number of stop bits
    if(CDC_getStopBits() != stopbits) {
      stopbits = CDC_getStopBits();
      UART_setStopBits(stopbits ? 2 : 1);
    }
  }
}
