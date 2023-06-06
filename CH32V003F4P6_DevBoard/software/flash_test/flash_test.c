// ===================================================================================
// Project:   FLASH Demo for CH32V003
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
// Read/write option bytes demo.
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <system.h>                           // system functions
#include <gpio.h>                             // GPIO functions
#include <debug.h>                            // serial DEBUG functions
#include <flash.h>                            // FLASH functions

#define PIN_LED   PC0                         // define LED pin
#define PIN_RST   PD7                         // define RST pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DEBUG_init();                               // init serial debug
  FLASH_OB_erase();                           // erase option bytes and enable read-out
  FLASH_OB_RESET2GPIO();                      // make reset pin anormal GPIO pin
  FLASH_OB_DATA_write(0x1234);                // write value to user data

  // Loop
  while(1) {
    DLY_ms(1000);
    DEBUG_print("Option Bytes FLAGS: ");
    DEBUG_printB(FLASH_OB_USER_read());       // print option bytes user flags
    DEBUG_newline();
    DEBUG_print("Option Bytes DATA:  ");      // print option bytes user data
    DEBUG_printW(FLASH_OB_DATA_read());
    DEBUG_newline();
  }
}
