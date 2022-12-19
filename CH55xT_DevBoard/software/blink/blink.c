// ===================================================================================
// Blink an LED connected to pin 3.3
// ===================================================================================

#include <ch554.h>            // CH55x header file
#include <clock.h>            // system clock functions
#include <delay.h>            // delay functions
#include <gpio.h>             // GPIO functions

#define PIN_LED   P33         // define LED pin

void main() {
  CLK_config();               // configure system clock
  pinOutput(PIN_LED);         // set LED pin as output

  while (1) {
    _delay_ms(100);           // wait 100ms
    pinToggle(PIN_LED);       // toggle LED on/off
  }
}
