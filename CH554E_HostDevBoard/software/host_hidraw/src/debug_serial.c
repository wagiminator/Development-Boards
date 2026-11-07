// ===================================================================================
// Serial Debug Functions for CH551, CH552 and CH554                          * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "debug_serial.h"

// Send character
void DEBUG_write(char c) {
#if DEBUG_PORT == 0
  while(!TI);
  TI = 0;
  SBUF = c;
#else
  while(!U1TI);
  U1TI = 0;
  SBUF1 = c;
#endif
}

// Send character for printf
#if SDCC < 370
void putchar(char c) {
  DEBUG_write(c);
}
#else
int putchar(int c) {
  DEBUG_write(c);
  return c;
}
#endif

// Send string
void DEBUG_print(const char* str) {
  while(*str) DEBUG_write(*str++);
}

// Send string with newline
void DEBUG_println(const char* str) {
  DEBUG_print(str);
  DEBUG_write('\n');
}

// Send 16-bit decimal value
void DEBUG_printD(uint16_t value) {
  static __code const uint16_t div[] = {10000, 1000, 100, 10};
  uint8_t i, digit, lead = 0;
  for(i = 0; i < 4; i++) {
    digit = '0';
    while(value >= div[i]) {
      value -= div[i];
      digit++;
    }
    if(digit > '0') lead = 1;
    if(lead) DEBUG_write(digit);
  }
  DEBUG_write('0' + (uint8_t)value);
}

// Send 8-bit hex value
void DEBUG_printB(uint8_t value) {
  uint8_t n = value >> 4;
  DEBUG_write(n + (n < 10 ? '0' : 'A' - 10));
  n = value & 0x0f;
  DEBUG_write(n + (n < 10 ? '0' : 'A' - 10));
}

// Send 16-bit hex value
void DEBUG_printH(uint16_t value) {
  DEBUG_printB(value >> 8);
  DEBUG_printB(value);
}
