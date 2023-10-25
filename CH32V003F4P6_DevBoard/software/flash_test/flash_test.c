// ===================================================================================
// Project:   FLASH Demo for CH32V003
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
// FLASH, Option Bytes (OB) and Electronic Signature (ESIG) demo.
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
#include <flash.h>                            // FLASH functions
#include <debug_serial.h>                     // serial DEBUG functions

#define PIN_LED   PC0                         // define LED pin
#define PIN_RST   PD7                         // define RST pin

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DEBUG_init();                               // init serial debug

  // Perform some option bytes manipulations
  FLASH_OB_erase();                           // erase prior to rewrite
  FLASH_OB_RESET2GPIO();                      // make reset pin anormal GPIO pin
  FLASH_OB_DATA_write(0xCAFE);                // write value to user data

  // Write data to end of code flash area
  FLASH_unlock();                             // unlock flash
  FLASH_END_erase();                          // erase last code flash page (64 bytes)
  FLASH_END_write(0, 0xBEEF);                 // write data to end of code flash...
  FLASH_END_write(2, 0xFACE);                 // ...this data remains after reset
  FLASH_lock();                               // lock flash

  // Loop
  while(1) {
    DLY_ms(1000);

    DEBUG_print("FLASH PROTECTION:   ");
    if(FLASH_OB_protected())
      DEBUG_println("protected");
    else
      DEBUG_println("unprotected");

    DEBUG_print("OPTION BYTES FLAGS: ");
    DEBUG_printB(FLASH_OB_USER_read());       // print option bytes user flags
    DEBUG_newline();

    DEBUG_print("OPTION BYTES DATA:  ");      // print option bytes user data
    DEBUG_printH(FLASH_OB_DATA_read());
    DEBUG_newline();

    DEBUG_print("CODE FLASH END DATA: ");     // print "code flash end" data
    DEBUG_printH(FLASH_END_read(0));
    DEBUG_print(" ");
    DEBUG_printH(FLASH_END_read(2));
    DEBUG_newline();

    DEBUG_print("FLASH CAPACITY:      ");     // print flash capacity
    DEBUG_printD(ESIG->ESIG_FLACAP);
    DEBUG_println(" KB");

    DEBUG_print("UNIQUE DEVICE ID:    ");     // print unique device ID
    DEBUG_printW(ESIG->ESIG_UNIID3);
    DEBUG_printW(ESIG->ESIG_UNIID2);
    DEBUG_printW(ESIG->ESIG_UNIID1);
    DEBUG_newline();
    
    DEBUG_println("---------------------------------------------------");
  }
}
