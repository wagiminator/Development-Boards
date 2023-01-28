// ===================================================================================
// Project:   Touchkey Raw Value Demo for CH551, CH552 and CH554
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
// Continuously transmits touchkey raw sample values via USB-CDC.
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
// - Clock: min. 6 MHz internal
// - Adjust the firmware parameters in include/config.h if necessary.
// - Make sure SDCC toolchain and Python3 with PyUSB is installed.
// - Press BOOT button on the board and keep it pressed while connecting it via USB
//   with your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor or a serial plotter and select the correct serial port
//   (BAUD rate doesn't matter).
// - The touchkey raw sample values are continuously transmitted via CDC.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include <config.h>                       // user configurations
#include <system.h>                       // system functions
#include <delay.h>                        // delay functions
#include <usb_cdc.h>                      // USB-CDC serial functions
#include <touch.h>                        // touchkey functions
#include <stdio.h>                        // for printf

// Prototypes for used interrupts
void USB_interrupt(void);
void DeviceUSBInterrupt(void) __interrupt (INT_NO_USB) {
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
  DLY_ms(5);                              // wait for clock to settle
  CDC_init();                             // init USB CDC
  PIN_input(PIN_TOUCH);                   // set touchkey pin to input

  // Loop
  while(1) {
    printf("Touchkey raw value: %u \n", TOUCH_sample(PIN_TOUCH));
    DLY_ms(250);
  }
}
