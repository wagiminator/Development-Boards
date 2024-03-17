// ===================================================================================
// Basic Serial Debug Functions for STM32C0xx                                 * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "debug_serial.h"

#if DEBUG_ENABLE > 0

// Init UART
void DEBUG_init(void) {
  // Set GPIO pins
  #if DEBUG_TX < 2
    // Setup pin PA9 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(9<<1)))
                                     |  ((uint32_t)0b10<<(9<<1));
    GPIOA->OTYPER  &=                  ~((uint32_t)0b1 <<(9<<0));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~((uint32_t)0xf <<(1<<2)))
                                     |  ((uint32_t)0x1 <<(1<<2));
    #if DEBUG_TX == 1
      // Remap pins PA11 to PA9
      RCC->APBENR2  |= RCC_APBENR2_SYSCFGEN;
      SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_RMP;
    #endif
  #elif DEBUG_TX == 2
    // Setup pin PB6 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~((uint32_t)0b11<<(6<<1)))
                                     |  ((uint32_t)0b10<<(6<<1));
    GPIOB->OTYPER  &=                  ~((uint32_t)0b1 <<(6<<0));
    GPIOB->AFR[0]  &= ~((uint32_t)0xf <<(6<<2));
  #else
    #warning No automatic pin mapping for USART1
  #endif
	
  // Setup and start UART (8N1, TX, default BAUD rate)
  RCC->APBENR2 |= RCC_APBENR2_USART1EN;
  USART1->BRR = ((2 * F_CPU / DEBUG_BAUD) + 1) / 2;
  #if DEBUG_FIFO > 0
  USART1->CR1 = USART_CR1_FIFOEN | USART_CR1_TE | USART_CR1_UE;
  #else
  USART1->CR1 = USART_CR1_TE | USART_CR1_UE;
  #endif
}

// Send byte via UART
void DEBUG_write(const char c) {
  while(!(USART1->ISR & USART_ISR_TXE_TXFNF));
  USART1->TDR = c;
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

// Print decimal value (BCD conversion by substraction method)
void DEBUG_printD(uint32_t value) {
  uint8_t digitval;                               // current digit value
  uint8_t leadflag = 0;                           // flag for leading spaces
  uint32_t divider = 1000000000;                  // current divider
  while(divider) {                                // for all digits
    digitval = value / divider;                   // calculate digit value
    value    = value % divider;                   // set value to division remainder
    divider /= 10;                                // calculate next divider
    if(digitval) leadflag++;                      // end of leading spaces
    if(!divider) leadflag++;                      // least digit has to be printed
    if(leadflag) DEBUG_write(digitval + '0');     // print the digit
  }
}

// Convert 4-bit byte nibble into hex character and print it via UART
void DEBUG_printN(uint8_t nibble) {
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
void DEBUG_itoa(int32_t, int8_t, int8_t);
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

void DEBUG_itoa(int32_t val, int8_t rad, int8_t len) {
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
  do DEBUG_write(s[--i]);
  while(i);
}

#endif // DEBUG_ENABLE > 0
