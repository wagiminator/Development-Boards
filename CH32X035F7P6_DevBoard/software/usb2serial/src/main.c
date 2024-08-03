// ===================================================================================
// Project:   Example for CH32X035/X034/X033
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
// Simple USB-to-Serial Converter.
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port.
// - Serial port:  DTR: PA0, RTS: PA1, TXD: PA2, RXD: PA3


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "config.h"                 // user configurations
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "usb_cdc.h"                // USB CDC serial functions
#include "uart2_dma.h"              // USART2 with DMA functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Variables
  uint32_t baudrate = 115200;
  uint8_t  stopbits = 0;
  uint8_t  parity   = 0;

  // Setup pins
  PIN_output(PIN_LED);              // LED pin
  PIN_output(PIN_RTS);              // ready to send
  PIN_output(PIN_DTR);              // data terminal ready

  // Setup peripherals
  CDC_init();                       // init USB CDC
  UART2_init();                     // init UART

  // Loop
  while(1) {

    // Handle data transmission
    if(CDC_available() && UART2_ready()) UART2_write(CDC_read());
    if(UART2_available() && CDC_getDTR()) {
      while(UART2_available()) CDC_write(UART2_read());
      CDC_flush();
    }

    // Handle control line state
    PIN_write(PIN_LED, !CDC_getRTS());
    PIN_write(PIN_RTS, !CDC_getRTS());
    PIN_write(PIN_DTR, !CDC_getDTR());

    // Handle line coding - BAUD rate
    if(CDC_getBAUD() != baudrate) {
      baudrate = CDC_getBAUD();
      UART2_setBAUD(baudrate);
    }

    // Handle line coding - parity
    if(CDC_getParity() != parity) {
      parity = CDC_getParity();
      switch(parity) {
        case 0: UART2_setNoParity(); break;
        case 1: UART2_setOddParity(); break;
        case 2: UART2_setEvenParity(); break;
      }
    }

    // Handle line coding - number of stop bits
    if(CDC_getStopBits() != stopbits) {
      stopbits = CDC_getStopBits();
      if(stopbits) UART2_setStopBits(2);
      else         UART2_setStopBits(1);
    }
  }
}
