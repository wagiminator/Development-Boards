// ===================================================================================
// Basic PRINT Functions                                                      * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "print.h"

// Print decimal value (BCD conversion by subtraction method)
void printD(void (*putchr) (char c), uint16_t value) {
  static const uint16_t divider[] = {10000, 1000, 100, 10};
  uint8_t leadflag = 0;                     // flag for leading zeros
  for(uint8_t i=0; i<4; i++) {              // for all digits except the last
    uint8_t digitval = '0';                 // start with digit value 0
    while(value >= divider[i]) {            // if current divider fits into the value
      digitval++;                           // increase digit value
      value -= divider[i];                  // decrease value by divider
    }
    if(digitval > '0') leadflag = 1;        // end of leading zeros
    if(leadflag) putchr(digitval);          // print the digit
  }
  putchr((uint8_t)value + '0');             // least digit has to be printed
}

// Convert 4-bit byte nibble into hex character and print it via putchar
static void printN(void (*putchr) (char c), uint8_t nibble) {
  putchr((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert 8-bit byte into hex characters and print it via putchar
void printB(void (*putchr) (char c), uint8_t value) {
  printN(putchr, value >> 4);
  printN(putchr, value & 0x0f);
}

// Convert 16-bit half-word into hex characters and print it via putchar
void printH(void (*putchr) (char c), uint16_t value) {
  printB(putchr, value >> 8);
  printB(putchr, value);
}

// Convert 32-bit word into hex characters and print it via putchar
void printW(void (*putchr) (char c), uint32_t value) {
  printH(putchr, value >> 16);
  printH(putchr, value);
}

// Print string via putchar
void printS(void (*putchr) (char c), const char* str) {
  while(*str) putchr(*str++);
}

// Print string with newline via putchar
void println(void (*putchr) (char c), const char* str) {
  printS(putchr, str);
  putchr('\n');
}

// Convert value to string and print it via putchar
// rad: radix (negative: signed), len: field width (negative: zero padding)
static void printI(void (*putchr) (char c), uint32_t val, int8_t rad, int8_t len) {
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
  do putchr(s[--i]);
  while(i);
}

// printf, supports %s, %c, %d, %u, %x, %b, %02d, %%, and l for 32-bit (e.g. %ld)
void printF(void (*putchr) (char c), const char *str, ...) {
  va_list arp;
  int8_t r, w;
  uint8_t s, l;
  char d;
  char *c;
  uint32_t v;

  va_start(arp, str);
  while((d = *str++) != 0) {
    if(d != '%') {
      putchr(d);
      continue;
    }
    d = *str++;
    if(d == '%') {                          // "%%" -> print '%'
      putchr(d);
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
      while(*c) putchr(*(c++));
      continue;
    }
    if(d == 'c') {
      putchr((char)va_arg(arp, int));
      continue;
    }
    if(d == '\0') break;
    else if(d == 'u') r = 10;
    else if(d == 'd') r = -10;
    else if(d == 'x' || d == 'X') r = 16;
    else if(d == 'b') r = 2;
    else {                                  // unknown conversion -> print it
      putchr(d);
      continue;
    }
    if(l)          v = va_arg(arp, uint32_t);
    else if(r < 0) v = (int32_t)(int16_t)va_arg(arp, int);
    else           v = (uint16_t)va_arg(arp, unsigned int);
    printI(putchr, v, r, w);
  }
  va_end(arp);
}
