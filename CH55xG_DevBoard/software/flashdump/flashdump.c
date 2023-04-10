// ===================================================================================
// Project:   Data Flash DUMP via USB-CDC for CH551, CH552 and CH554
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
// Transfers content of data flash via USB-CDC.
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
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't
//   matter).
// - The data flash content is transmitted once a second via CDC.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================

// Libraries
#include "src/system.h"                   // system functions
#include "src/delay.h"                    // delay functions
#include "src/flash.h"                    // data flash functions
#include "src/usb_cdc.h"                  // USB-CDC serial functions

// Prototypes for used interrupts
void USB_interrupt(void);
void USB_ISR(void) __interrupt(INT_NO_USB) {
  USB_interrupt();
}

// ===================================================================================
// Print HEX Values
// ===================================================================================

// Convert byte nibble into hex character and print it via CDC
void printNibble(uint8_t nibble) {
  (nibble<=9) ? (nibble+='0') : (nibble+='A'-10);
  CDC_write(nibble);
}

// Convert byte into hex characters and print it via CDC
void printByte(uint8_t value) {
  printNibble(value >> 4);
  printNibble(value & 0x0f);
}

// ===================================================================================
// Main Function
// ===================================================================================
void main(void) {
  // Setup
  CLK_config();                           // configure system clock
  DLY_ms(10);                             // wait for clock to settle
  CDC_init();                             // init USB CDC

  // Loop
  while(1) {
    uint8_t i,j;
    uint8_t addr = 0;
    CDC_println("Data Flash Hex Dump:");
    for(j=8; j; j--) {
      printByte(0); printByte(addr); CDC_print(": ");
      for(i=16; i; i--) {
        printByte(FLASH_read(addr++)); CDC_write(' ');
      }
      CDC_println("");
    }
    CDC_println("");
    DLY_ms(1000);                         // wait a second
  }
}
