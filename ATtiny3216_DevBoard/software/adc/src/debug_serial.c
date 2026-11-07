// ===================================================================================
// Basic Serial Debug Functions for tinyAVR 0-Series and 1-Series             * v1.1 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "debug_serial.h"

#if DEBUG_ENABLE > 0

// BAUD rate calculation (switch to double-speed mode if necessary)
#if ((F_CPU * 4) / DEBUG_BAUD) < 64
  #define DEBUG_CLK2X     1
  #define DEBUG_BAUD_REG  ((((F_CPU * 16) / DEBUG_BAUD) + 1) / 2)
#else
  #define DEBUG_CLK2X     0
  #define DEBUG_BAUD_REG  ((((F_CPU *  8) / DEBUG_BAUD) + 1) / 2)
#endif

// Init debug interface (USART0 TX)
void DEBUG_init(void) {
  // Setup TX pin (output high)
  #if DEBUG_TX == 0
    #ifdef PORTB
    VPORTB.OUT |= (1 << 2); VPORTB.DIR |= (1 << 2);   // PB2
    #else
    VPORTA.OUT |= (1 << 6); VPORTA.DIR |= (1 << 6);   // PA6 (8-pin)
    #endif
  #elif DEBUG_TX == 1
    VPORTA.OUT |= (1 << 1); VPORTA.DIR |= (1 << 1);   // PA1
    PORTMUX.CTRLB |= PORTMUX_USART0_bm;
  #else
    #warning Wrong DEBUG_TX pin mapping
  #endif

  // Setup and start UART (8N1, TX, default BAUD rate)
  USART0.BAUD  = DEBUG_BAUD_REG;
  #if DEBUG_CLK2X > 0
  USART0.CTRLB = USART_TXEN_bm | USART_RXMODE_CLK2X_gc;
  #else
  USART0.CTRLB = USART_TXEN_bm;
  #endif
}

// Send byte via UART
void DEBUG_write(const char c) {
  while(!(USART0.STATUS & USART_DREIF_bm));
  USART0.TXDATAL = c;
}

// Send string via UART
void DEBUG_print(const char* str) {
  while(*str) DEBUG_write(*str++);
}

// Send string via UART with newline
void DEBUG_println(const char* str) {
  DEBUG_print(str);
  DEBUG_write('\n');
}

// Print decimal value (BCD conversion by subtraction method)
void DEBUG_printD(uint16_t value) {
  static const uint16_t divider[] = {10000, 1000, 100, 10};
  uint8_t leadflag = 0;                     // flag for leading zeros
  for(uint8_t i=0; i<4; i++) {              // for all digits except the last
    uint8_t digitval = '0';                 // start with digit value 0
    while(value >= divider[i]) {            // if current divider fits into the value
      digitval++;                           // increase digit value
      value -= divider[i];                  // decrease value by divider
    }
    if(digitval > '0') leadflag = 1;        // end of leading zeros
    if(leadflag) DEBUG_write(digitval);          // print the digit
  }
  DEBUG_write((uint8_t)value + '0');             // least digit has to be printed
}

// Convert 4-bit byte nibble into hex character and print it via UART
static void DEBUG_printN(uint8_t nibble) {
  DEBUG_write((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert 8-bit byte into hex characters and print it via UART
void DEBUG_printB(uint8_t value) {
  DEBUG_printN(value >> 4);
  DEBUG_printN(value & 0x0f);
}

// Convert 16-bit half-word into hex characters and print it via UART
void DEBUG_printH(uint16_t value) {
  DEBUG_printB(value >> 8);
  DEBUG_printB(value);
}

// Convert 32-bit word into hex characters and print it via UART
void DEBUG_printW(uint32_t value) {
  DEBUG_printH(value >> 16);
  DEBUG_printH(value);
}

// Convert value to string and print it via UART
// rad: radix (negative: signed), len: field width (negative: zero padding)
static void DEBUG_printI(uint32_t val, int8_t rad, int8_t len) {
  char c, sgn = 0, pad = ' ';
  char s[34];                               // 32 binary digits + sign + spare
  uint8_t i = 0;

  if(rad < 0) {
    rad = -rad;
    if((int32_t)val < 0) {
      val = -(int32_t)val;
      sgn = '-';
    }
  }
  if(len < 0) {
    len = -len;
    pad = '0';
  }
  if(len > 32) len = 32;
  if(sgn && (pad == '0') && len) len--;     // sign is part of the field width
  do {
    c = (char)(val % (uint8_t)rad);
    c += (c >= 10) ? ('A' - 10) : '0';
    s[i++] = c;
    val /= (uint8_t)rad;
  } while(val);
  if(sgn && (pad != '0')) s[i++] = sgn;
  while(i < len) s[i++] = pad;
  if(sgn && (pad == '0')) s[i++] = sgn;
  do DEBUG_write(s[--i]);
  while(i);
}

// printf, supports %s, %c, %d, %u, %x, %b, %02d, %%, and l for 32-bit (e.g. %ld)
void DEBUG_printf(const char *str, ...) {
  va_list arp;
  int8_t r, w;
  uint8_t s, l;
  char d;
  char *c;
  uint32_t v;

  va_start(arp, str);
  while((d = *str++) != 0) {
    if(d != '%') {
      DEBUG_write(d);
      continue;
    }
    d = *str++;
    if(d == '%') {                          // "%%" -> print '%'
      DEBUG_write(d);
      continue;
    }
    w = r = s = l = 0;
    if(d == '0') {                          // zero padding
      d = *str++;
      s = 1;
    }
    while((d >= '0') && (d <= '9')) {       // field width
      w = w * 10 + (d - '0');
      d = *str++;
    }
    if(s) w = -w;
    if(d == 'l') {                          // 32-bit value
      l = 1;
      d = *str++;
    }
    if(d == 's') {
      c = va_arg(arp, char*);
      while(*c) DEBUG_write(*(c++));
      continue;
    }
    if(d == 'c') {
      DEBUG_write((char)va_arg(arp, int));
      continue;
    }
    if(d == '\0') break;
    else if(d == 'u') r = 10;
    else if(d == 'd') r = -10;
    else if(d == 'x' || d == 'X') r = 16;
    else if(d == 'b') r = 2;
    else {                                  // unknown conversion -> print it
      DEBUG_write(d);
      continue;
    }
    if(l)          v = va_arg(arp, uint32_t);
    else if(r < 0) v = (int32_t)(int16_t)va_arg(arp, int);
    else           v = (uint16_t)va_arg(arp, unsigned int);
    DEBUG_printI(v, r, w);
  }
  va_end(arp);
}

#endif // DEBUG_ENABLE > 0
