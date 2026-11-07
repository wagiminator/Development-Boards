// ===================================================================================
// Project:   Example for ATmega4808
// Version:   v1.1
// Year:      2020
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Real-time clock using the RTC with the external 32.768kHz crystal. With a
// prescaler of 32768 the RTC counts every second. Every five seconds the current
// time and date are sent via the USB-to-serial converter (115200 BAUD, 8N1). The
// time is set to the compile time at startup.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-avr, avr-libc) and Python3 with megaupdi (via pip)
//   are installed. If necessary, a driver for the USB-to-serial converter used by
//   the SerialUPDI programmer must be installed.
// - Connect the SerialUPDI programmer to the UPDI header of the board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "debug_serial.h"           // serial debug functions

#define PIN_LED   PA7               // define LED pin
#define UPDATE_PERIOD   5           // update period in seconds

// ===================================================================================
// Time Keeping
// ===================================================================================
typedef struct {
  uint8_t  second, minute, hour, date, month;
  uint16_t year;
} time_t;

time_t t;
volatile uint8_t tick = 0;

// Convert two characters to number
#define C2N(s)    (((s)[0] == ' ' ? 0 : (s)[0] - '0') * 10 + (s)[1] - '0')

// Set time to compile time
void TIME_init(void) {
  const char months[] = "JanFebMarAprMayJunJulAugSepOctNovDec";
  const char* d = __DATE__;         // "Mmm dd yyyy"
  const char* s = __TIME__;         // "hh:mm:ss"
  t.month = 1;
  for(uint8_t i=0; i<12; i++)
    if((d[0] == months[i*3]) && (d[1] == months[i*3+1]) && (d[2] == months[i*3+2])) t.month = i + 1;
  t.date   = C2N(d + 4);
  t.year   = C2N(d + 7) * 100 + C2N(d + 9);
  t.hour   = C2N(s);
  t.minute = C2N(s + 3);
  t.second = C2N(s + 6);
}

// Return 1 if year is a leap year
uint8_t TIME_isLeapYear(void) {
  return ((!(t.year % 4) && (t.year % 100)) || !(t.year % 400));
}

// Update time (refer to AN2543)
void TIME_update(void) {
  const uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  t.second += UPDATE_PERIOD;
  if(t.second < 60) return;
  t.second -= 60;
  if(++t.minute < 60) return;
  t.minute = 0;
  if(++t.hour < 24) return;
  t.hour = 0;
  uint8_t dim = days[t.month - 1];
  if((t.month == 2) && TIME_isLeapYear()) dim++;
  if(++t.date <= dim) return;
  t.date = 1;
  if(++t.month <= 12) return;
  t.month = 1;
  t.year++;
}

// ===================================================================================
// RTC Overflow Interrupt Service Routine
// ===================================================================================
ISR(RTC_CNT_vect) {
  RTC_OVF_FLAG_clear();             // clear interrupt flag
  tick = 1;                         // raise tick flag
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CLK_init();                       // set system clock frequency
  PIN_output(PIN_LED);              // set LED pin to output
  DEBUG_init();                     // init DEBUG (TX: PA0, BAUD: 115200, 8N1)
  TIME_init();                      // set time to compile time
  LSE_enable();                     // enable external 32.768kHz crystal
  RTC_setLSE();                     // set crystal as RTC clock source
  RTC_PER_write(UPDATE_PERIOD - 1); // set RTC period
  RTC_OVF_INT_enable();             // enable overflow interrupt
  RTC_enable(32768);                // enable RTC with prescaler 32768 (1Hz)
  INT_enable();                     // enable global interrupts

  // Loop
  while(1) {
    if(tick) {                      // if tick flag is set
      PIN_high(PIN_LED);            // LED on
      TIME_update();                // update time
      DEBUG_printf("Hello World ! - %02d:%02d:%02d - %02d.%02d.%d\n",
        t.hour, t.minute, t.second, t.date, t.month, t.year);
      tick = 0;                     // clear flag
      PIN_low(PIN_LED);             // LED off
    }
  }
}
