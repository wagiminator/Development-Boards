// ===================================================================================
// Project:   Example for STC8H8K64U
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
// Reads the supply voltage and the analog value on pin P1.1 (A1) once a second
// and sends the results via USB CDC (virtual serial port).
//
// Compilation Instructions:
// -------------------------
// - Make sure SDCC toolchain and Python3 with stc8usb (via pip) are installed. In
//   addition, Linux requires access rights to the USB bootloader.
// - Press and hold the BOOT button while connecting the board via USB to your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't
//   matter).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "delay.h"                  // delay functions
#include "gpio.h"                   // GPIO functions (ADC)
#include "usb_cdc.h"                // USB CDC serial functions

#define PIN_ADC   P11               // pin for ADC measurement

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

// Print 16-bit decimal value via CDC
void CDC_printD(uint16_t value) {
  uint8_t digits[5];
  uint8_t i = 0;
  do {
    digits[i++] = value % 10;
    value /= 10;
  } while(value);
  while(i) CDC_write('0' + digits[--i]);
}

// Print 8-bit hex value via CDC
void CDC_printB(uint8_t value) {
  uint8_t d = value >> 4;
  CDC_write(d < 10 ? '0' + d : 'A' - 10 + d);
  d = value & 0x0F;
  CDC_write(d < 10 ? '0' + d : 'A' - 10 + d);
}

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  P_SW2 |= 0x80;                    // enable access to extended registers (MUST!)
  CDC_init();                       // init USB CDC
  ADC_init();                       // init ADC

  // Loop
  while(1) {
    ADC_input(PIN_ADC);             // set ADC input pin
    CDC_print("PIN: ");   CDC_printD(ADC_read());     CDC_print("\n");
    CDC_print("VDD: ");   CDC_printD(ADC_read_VDD()); CDC_print(" mV\n\n");
    CDC_flush();                    // flush transmit buffer
    DLY_ms(1000);                   // wait a second
  }
}
