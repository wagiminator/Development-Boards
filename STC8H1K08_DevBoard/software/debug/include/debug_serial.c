// ===================================================================================
// Basic Serial Debug Functions for STC8H Microcontrollers                    * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "debug_serial.h"

#if DEBUG_ENABLE > 0

#if DEBUG_PORT == 1
void DEBUG_write(const char c) {
  while(!TI);
  TI = 0;
  SBUF = c;
}
#elif DEBUG_PORT == 2
void DEBUG_write(const char c) {
  while(!(S2CON & 0x02));
  S2CON &= ~0x02;
  S2BUF = c;
}
#endif  // DEBUG_PORT

// printf, supports %s, %c, %d, %u, %x, %b, %02d, %%
void DEBUG_itoa(int16_t, int8_t, int8_t);
static void DEBUG_vfprintf(const char *format, va_list arg);

void DEBUG_printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  DEBUG_vfprintf(format, arg);
  va_end(arg);
}

static void DEBUG_vfprintf(const char* str,  va_list arp) {
  int16_t d, r, w, s;
  char *c;

  while((d = *str++) != 0) {
    if(d != '%') {
      DEBUG_write(d);
      continue;
    }
    d = *str++;
    w = r = s = 0;
    if(d == '%') {
      DEBUG_write(d);
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
      while(*c) DEBUG_write(*(c++));
      continue;
    }
    if(d == 'c') {
      DEBUG_write((char)va_arg(arp, int));
      continue;
    }
    if(d =='\0') break;
    else if(d == 'u') r =  10;
    else if(d == 'd') r = -10;
    else if(d == 'x') r =  16;
    else if(d == 'b') r =  2;
    else str--;
    if(r == 0) continue;
    if(r > 0) DEBUG_itoa((uint16_t)va_arg(arp, int16_t), r, w);
    else DEBUG_itoa((int16_t)va_arg(arp, int16_t), r, w);
  }
}

void DEBUG_itoa(int16_t val, int8_t rad, int8_t len) {
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
    c = (char)((uint16_t)val % rad);
    if (c >= 10) c += ('A' - 10);
    else c += '0';
    s[i++] = c;
    val = (uint16_t)val / rad;
  } while(val);
  if((sgn != 0) && (pad != '0')) s[i++] = sgn;
  while(i < (uint8_t)len) s[i++] = pad;
  if((sgn != 0) && (pad == '0')) s[i++] = sgn;
  do DEBUG_write(s[--i]);
  while(i);
}

#endif // DEBUG_ENABLE > 0
