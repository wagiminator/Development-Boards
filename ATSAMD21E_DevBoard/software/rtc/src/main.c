// ===================================================================================
// Project:   Example for ATSAMD21E
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
// Real-time clock using the RTC in calendar mode clocked by the external
// 32.768kHz crystal. The time is set to the compile time and sent once a second
// via USB CDC (virtual serial port).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with tinyocd (via pip)
//   are installed. If necessary, grant access permissions for the SWD programmer.
// - Connect the SWD programmer (e.g. CMSIS-DAP) to the SWD pins of the board.
// - Run 'make flash'.
// - Alternatively, if the SAM-BA bootloader is installed, set LDSCRIPT and ISPTOOL
//   in the makefile accordingly (see readme) and upload the firmware via USB.
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
#include "gpio.h"                   // GPIO functions
#include "usb_cdc.h"                // USB CDC serial functions

#define PIN_LED   PA8               // define LED pin

// Convert two characters to number
#define C2N(s)    (((s)[0] == ' ' ? 0 : (s)[0] - '0') * 10 + (s)[1] - '0')

// Get month from compile date string
uint8_t getMonth(const char* s) {
  const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  for(uint8_t i=0; i<12; i++) {
    if((s[0] == months[i*3]) && (s[1] == months[i*3+1]) && (s[2] == months[i*3+2]))
      return i + 1;
  }
  return 1;
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  PIN_output(PIN_LED);              // set LED pin as output
  CDC_init();                       // init USB CDC
  RTC_init_XOSC32K();               // init RTC with external crystal

  // Set compile time and date
  const char* date = __DATE__;      // "Mmm dd yyyy"
  const char* time = __TIME__;      // "hh:mm:ss"
  RTC_setDate(C2N(date + 9) + 2000 - RTC_YEAR_REF, getMonth(date), C2N(date + 4));
  RTC_setTime(C2N(time), C2N(time + 3), C2N(time + 6));

  // Loop
  uint8_t last = 0xff;
  while(1) {
    uint32_t t = RTC_read();        // read RTC clock register
    if(RTC_getSeconds(t) != last) { // new second?
      last = RTC_getSeconds(t);
      PIN_toggle(PIN_LED);          // toggle LED
      CDC_printf("%02d:%02d:%02d - %02d.%02d.%d\n",
        RTC_getHours(t), RTC_getMinutes(t), RTC_getSeconds(t),
        RTC_getDay(t), RTC_getMonth(t), RTC_getYear(t));
      CDC_flush();
    }
  }
}
