// ===================================================================================
// Basic PRINT Functions                                                      * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "print.h"

// Print decimal value
void printD(void (*putchar) (char c), uint32_t value) {
  uint8_t digitval;                               // current digit value
  uint8_t leadflag = 0;                           // flag for leading spaces
  uint32_t divider = 1000000000;                  // current divider
  while(divider) {                                // for all digits
    digitval = value / divider;                   // calculate digit value
    value    = value % divider;                   // set value to division remainder
    divider /= 10;                                // calculate next divider
    if(digitval) leadflag++;                      // end of leading spaces
    if(!divider) leadflag++;                      // least digit has to be printed
    if(leadflag) putchar(digitval + '0');         // print the digit
  }
}

// Convert 4-bit byte nibble into hex character and print it via putchar
void printN(void (*putchar) (char c), uint8_t nibble) {
  putchar((nibble <= 9) ? ('0' + nibble) : ('A' - 10 + nibble));
}

// Convert 8-bit byte into hex characters and print it via putchar
void printB(void (*putchar) (char c), uint8_t value) {
  printN(putchar, value >> 4);
  printN(putchar, value & 0x0f);
}

// Convert 16-bit half-word into hex characters and print it via putchar
void printH(void (*putchar) (char c), uint16_t value) {
  printB(putchar, value >> 8);
  printB(putchar, value);
}

// Convert 32-bit word into hex characters and print it via putchar
void printW(void (*putchar) (char c), uint32_t value) {
  printH(putchar, value >> 16);
  printH(putchar, value);
}

// Print string via putchar
void printS(void (*putchar) (char c), const char* str) {
  while(*str) putchar(*str++);
}

// Print string with newline via putchar
void println(void (*putchar) (char c), const char* str) {
  while(*str) putchar(*str++);
  putchar('\n');
}

// printf, supports %s, %c, %d, %u, %x, %b, %02d, %%
void _itoa(void (*putchar) (char c), int32_t, int8_t, int8_t);
static void _vfprintf(void (*putchar) (char c), const char *format, va_list arg);

void printF(void (*putchar) (char c), const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  _vfprintf(putchar, format, arg);
  va_end(arg);
}

static void _vfprintf(void (*putchar) (char c), const char* str,  va_list arp) {
  int32_t d, r, w, s;
  char *c;

  while((d = *str++) != 0) {
    if(d != '%') {
      putchar(d);
      continue;
    }
    d = *str++;
    w = r = s = 0;
    if(d == '%') {
      putchar(d);
      d = *str++;
    }
    if(d == '0') {
      d = *str++;
      s = 1;
    }
    while((d >= '0') && (d <= '9')) {
      w += w * 10 + (d - '0');
      d = *str++;
    }
    if(s) w = -w;
    if(d == 's') {
      c = va_arg(arp, char*);
      while(*c) putchar(*(c++));
      continue;
    }
    if(d == 'c') {
      putchar((char)va_arg(arp, int));
      continue;
    }
    if(d =='\0') break;
    else if(d == 'u') r = 10;
    else if(d == 'd') r = -10;
    else if(d == 'x') r = 16;
    else if(d == 'b') r = 2;
    else str--;
    if(r == 0) continue;
    if(r > 0) _itoa(putchar, (uint32_t)va_arg(arp, int32_t), r, w);
    else _itoa(putchar, (int32_t)va_arg(arp, int32_t), r, w);
  }
}

void _itoa(void (*putchar) (char c), int32_t val, int8_t rad, int8_t len) {
  char c, sgn = 0, pad = ' ';
  char s[20];
  uint8_t i = 0;

  if(rad < 0) {
    rad = -rad;
    if(val < 0) {
      val = -val;
      sgn = '-';
    }
  }
  if(len < 0) {
    len = -len;
    pad = '0';
  }
  if(len > 20) return;
  do {
    c = (char)((uint32_t)val % rad);
    if (c >= 10) c += ('A' - 10);
    else c += '0';
    s[i++] = c;
    val = (uint32_t)val / rad;
  } while(val);
  if((sgn != 0) && (pad != '0')) s[i++] = sgn;
  while(i < len) s[i++] = pad;
  if((sgn != 0) && (pad == '0')) s[i++] = sgn;
  do putchar(s[--i]);
  while(i);
}
