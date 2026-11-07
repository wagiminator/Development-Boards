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
// Simple USB-to-serial converter. The MCU builds a USB communication device class
// (CDC) and forwards the data between USB and UART1 (TXD1: P1.7, RXD1: P1.6). The
// BAUD rate is set by the PC. The built-in LED flashes on data transfer.
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
// - Connect the UART pins to the device you want to communicate with (TXD1 to RX,
//   RXD1 to TX, GND to GND).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "config.h"                 // user configurations
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "usb_cdc.h"                // USB CDC serial functions
#include "uart_int.h"               // UART with RX interrupt functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

// Set UART1 BAUD rate
void UART1_setBAUD(uint32_t baud) {
  SBAUD1 = 256 - (uint8_t)((F_CPU / 8 / baud + 1) / 2);
}

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  uint32_t baudrate = 115200;
  CLK_config();                     // configure system clock
  PIN_output(PIN_LED);              // set LED pin as output
  CDC_init();                       // init USB CDC
  UART_init();                      // init UART1 with RX interrupt

  // Loop
  while(1) {
    PIN_low(PIN_LED);               // LED off
    if(CDC_available()) {           // data from USB?
      PIN_high(PIN_LED);            // LED on
      UART_write(CDC_read());       // send it via UART
    }
    if(UART_available()) {          // data from UART?
      PIN_high(PIN_LED);            // LED on
      while(UART_available()) CDC_write(UART_read());
      CDC_flush();                  // send it via USB
    }
    if(CDC_getBAUD() && (CDC_getBAUD() != baudrate)) {
      baudrate = CDC_getBAUD();     // BAUD rate changed by PC
      UART1_setBAUD(baudrate);      // set new BAUD rate
    }
  }
}
