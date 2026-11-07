// ===================================================================================
// Project:   Example for STM32C011
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
// Writes a test pattern to a 24Cxx I2C EEPROM, reads it back and sends a hex dump
// via the USB-to-serial converter (115200 BAUD, 8N1). Connect the EEPROM to
// PA12 (SDA) and PA11 (SCL).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with stm32isp (via pip)
//   are installed. If necessary, a driver for the USB-to-serial converter must be
//   installed.
// - Connect the board via USB to your PC.
// - Put the MCU into boot mode (press and hold BOOT, press and release RESET, then
//   release BOOT).
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "debug_serial.h"           // serial debug functions
#include "eeprom_24c.h"             // I2C EEPROM functions

#define PIN_LED   PB6               // define LED pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  uint16_t addr;
  PIN_output(PIN_LED);              // set LED pin as output
  DEBUG_init();                     // init serial debug
  EE_init();                        // init I2C and EEPROM

  // Write test pattern to the first 64 bytes
  DEBUG_println("Writing test pattern ...");
  for(addr=0; addr<64; addr++) EE_update(addr, addr ^ 0x5A);

  // Loop
  while(1) {
    PIN_low(PIN_LED);               // LED on (active low)
    DEBUG_println("\nEEPROM hex dump:");
    for(addr=0; addr<64; addr+=16) {
      DEBUG_printf("%04x: ", addr);
      for(uint8_t i=0; i<16; i++) DEBUG_printf("%02x ", EE_read(addr + i));
      DEBUG_newline();
    }
    PIN_high(PIN_LED);              // LED off
    DLY_ms(2000);                   // wait two seconds
  }
}
