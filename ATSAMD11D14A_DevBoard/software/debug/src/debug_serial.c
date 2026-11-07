// ===================================================================================
// Basic Serial Debug Functions for SAMD11 (SERCOM USART, TX only)           * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "debug_serial.h"
#include "gpio.h"

#if DEBUG_ENABLE > 0

static uint8_t DEBUG_txflag = 0;               // something was transmitted

// Set BAUD rate (fractional BAUD rate generator, 16x oversampling)
void DEBUG_setBAUD(uint32_t baud) {
  uint32_t ctrla = DEBUG_SER->USART.CTRLA.reg;
  uint32_t div8  = (F_CPU + baud) / (baud << 1);  // F_CPU / (16 * baud) * 8
  if(ctrla & SERCOM_USART_CTRLA_ENABLE) {         // BAUD is enable-protected
    if(DEBUG_txflag) while(!(DEBUG_SER->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_TXC));
    DEBUG_SER->USART.CTRLA.reg = ctrla & ~SERCOM_USART_CTRLA_ENABLE;
    while(DEBUG_SER->USART.SYNCBUSY.reg);
  }
  DEBUG_SER->USART.BAUD.reg = SERCOM_USART_BAUD_FRAC_BAUD(div8 >> 3)
                            | SERCOM_USART_BAUD_FRAC_FP(div8 & 7);
  DEBUG_SER->USART.CTRLA.reg = ctrla;
  while(DEBUG_SER->USART.SYNCBUSY.reg);
}

// Init UART
void DEBUG_init(void) {
  // Set GPIO pin
  #ifdef DEBUG_PMUX
  PIN_alternate(DEBUG_TX_PIN, DEBUG_PMUX);
  #else
    #warning No automatic pin mapping for DEBUG
  #endif

  // Enable SERCOM clocks
  PM->APBCMASK.reg |= DEBUG_APBMASK;              // enable SERCOM module clock
  GCLK_setCH(DEBUG_GCLK_ID, GCLK_SYS);            // GCLK0 -> SERCOM

  // Setup and start UART (8N1, TX, default BAUD rate)
  DEBUG_SER->USART.CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
  while(DEBUG_SER->USART.CTRLA.reg & SERCOM_USART_CTRLA_SWRST);
  DEBUG_SER->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE_USART_INT_CLK  // internal clock
                             | SERCOM_USART_CTRLA_SAMPR(1)            // 16x fractional
                             | SERCOM_USART_CTRLA_TXPO(DEBUG_TXPO)    // TX pad
                             | SERCOM_USART_CTRLA_DORD;               // LSB first
  DEBUG_SER->USART.CTRLB.reg = SERCOM_USART_CTRLB_TXEN;               // enable TX
  while(DEBUG_SER->USART.SYNCBUSY.reg);
  DEBUG_setBAUD(DEBUG_BAUD);                                          // set BAUD rate
  DEBUG_SER->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;            // enable USART
  while(DEBUG_SER->USART.SYNCBUSY.reg);
}

// Send byte via UART
void DEBUG_write(const char c) {
  while(!(DEBUG_SER->USART.INTFLAG.reg & SERCOM_USART_INTFLAG_DRE));
  DEBUG_SER->USART.DATA.reg = c;
  DEBUG_txflag = 1;
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

// For BCD conversion
static const uint32_t DIVIDER[] = {1, 10, 100, 1000, 10000, 100000, 1000000,
                            10000000, 100000000, 1000000000};

// Print decimal value (BCD conversion by substraction method)
void DEBUG_printD(uint32_t value) {
  uint8_t digits   = 10;                          // print 10 digits
  uint8_t leadflag = 0;                           // flag for leading spaces
  while(digits--) {                               // for all digits digits
    uint8_t digitval = 0;                         // start with digit value 0
    uint32_t divider = DIVIDER[digits];           // read current divider
    while(value >= divider) {                     // if current divider fits into the value
      leadflag = 1;                               // end of leading spaces
      digitval++;                                 // increase digit value
      value -= divider;                           // decrease value by divider
    }
    if(!digits)  leadflag++;                      // least digit has to be printed
    if(leadflag) DEBUG_write(digitval + '0');     // print the digit
  }
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

// printf, supports %s, %c, %d, %u, %x, %b, %02d, %%
static void DEBUG_itoa(int32_t, int8_t, int32_t);
static void DEBUG_vfprintf(const char *format, va_list arg);

void DEBUG_printf(const char *format, ...) {
  va_list arg;
  va_start(arg, format);
  DEBUG_vfprintf(format, arg);
  va_end(arg);
}

static void DEBUG_vfprintf(const char* str,  va_list arp) {
  int32_t d, r, w, s;
  char *c;

  while((d = *str++) != 0) {
    if(d != '%') {
      DEBUG_write(d);
      continue;
    }
    d = *str++;
    w = r = s = 0;
    if(d == '%') {                               // "%%" -> print '%'
      DEBUG_write(d);
      continue;
    }
    if(d == '0') {
      d = *str++;
      s = 1;
    }
    while((d >= '0') && (d <= '9')) {
      w = w * 10 + (d - '0');
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
    else if(d == 'u') r = 10;
    else if(d == 'd') r = -10;
    else if(d == 'x') r = 16;
    else if(d == 'b') r = 2;
    else str--;
    if(r == 0) continue;
    if(r > 0) DEBUG_itoa((uint32_t)va_arg(arp, int32_t), r, w);
    else DEBUG_itoa((int32_t)va_arg(arp, int32_t), r, w);
  }
}

static void DEBUG_itoa(int32_t val, int8_t rad, int32_t len) {
  char c, sgn = 0, pad = ' ';
  char s[34];                                     // 32 binary digits + sign
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
  if(len > 32) len = 32;
  if(sgn && (pad == '0') && len) len--;           // sign is part of the field width
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
  do DEBUG_write(s[--i]);
  while(i);
}

#endif // DEBUG_ENABLE > 0
