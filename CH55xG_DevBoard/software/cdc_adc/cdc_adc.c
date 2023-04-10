// ===================================================================================
// Project:   ADC Transmitter Demo for CH551, CH552 and CH554
// Version:   v1.1
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Continuously transmits ADC sample values (pin P1.4) via USB-CDC.
//
// References:
// -----------
// - Blinkinlabs: https://github.com/Blinkinlabs/ch554_sdcc
// - Deqing Sun: https://github.com/DeqingSun/ch55xduino
// - Ralph Doncaster: https://github.com/nerdralph/ch554_sdcc
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Chip:  CH551, CH552 or CH554
// - Clock: 16 MHz internal
// - Adjust the firmware parameters in src/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash' immediatly afterwards.
// - To compile the firmware using the Arduino IDE, follow the instructions in the 
//   .ino file.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor or a serial plotter and select the correct serial port
//   (BAUD rate doesn't matter).
// - The ADC sample values are continuously transmitted via CDC.
// - Use a variable resistor (5V - P1.4 - GND) to change values.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/config.h"                   // user configurations
#include "src/gpio.h"                     // GPIO functions
#include "src/system.h"                   // system functions
#include "src/delay.h"                    // delay functions
#include "src/usb_cdc.h"                  // USB-CDC serial functions
#include <stdio.h>                        // for printf

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

// ===================================================================================
// printf
// ===================================================================================
#define printf printf_tiny
#define ALWAYS_PRINT_UNSIGNED

#if SDCC < 370
void putchar(char c) {
  CDC_write(c);
  if(c == '\n') CDC_flush();
}
#else
int putchar(int c) {
  CDC_write(c & 0xFF);
  if(c == '\n') CDC_flush();
  return c;
}
#endif

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                           // configure system clock
  DLY_ms(10);                             // wait for clock to settle
  CDC_init();                             // init USB CDC
  PIN_input(PIN_ADC);                     // set ADC pin to INPUT
  ADC_input(PIN_ADC);                     // set ADC pin as ADC input
  ADC_enable();                           // enable ADC

  // Loop
  while(1) {
    printf("ADC value: %u \n", ADC_read());
    DLY_ms(250);
  }
}
