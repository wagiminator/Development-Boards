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
// Increments a boot counter in the EEPROM (IAP area) at each start and sends a hex
// dump of the first 128 bytes of the EEPROM via USB CDC (virtual serial port).
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
#include "eeprom.h"                 // EEPROM functions
#include "usb_cdc.h"                // USB CDC serial functions

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
  uint8_t i, j, cnt;
  uint16_t addr;
  P_SW2 |= 0x80;                    // enable access to extended registers (MUST!)
  cnt = EEPROM_read(0) + 1;         // read and increment boot counter
  EEPROM_erase(0);                  // erase first EEPROM sector
  EEPROM_write(0, cnt);             // write new boot counter
  CDC_init();                       // init USB CDC

  // Loop
  while(1) {
    CDC_print("\nBoot counter: "); CDC_printD(EEPROM_read(0));
    CDC_print("\nEEPROM hex dump:\n");
    addr = 0;
    for(j=0; j<8; j++) {
      CDC_printB(addr); CDC_print(": ");
      for(i=0; i<16; i++) {
        CDC_printB(EEPROM_read(addr++));
        CDC_write(' ');
      }
      CDC_write('\n');
    }
    CDC_flush();                    // flush transmit buffer
    DLY_ms(3000);                   // wait 3 seconds
  }
}
