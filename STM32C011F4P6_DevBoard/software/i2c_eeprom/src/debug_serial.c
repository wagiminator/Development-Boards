// ===================================================================================
// Basic Serial Debug Functions for STM32C0xx                                 * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "debug_serial.h"

#if DEBUG_ENABLE > 0

// Init UART
void DEBUG_init(void) {
  // Set GPIO pins
  #if DEBUG_TX == 0
    // Setup pin PA9 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(9<<1)))
                      |  ((uint32_t)0b10<<(9<<1));
    GPIOA->OTYPER  &= ~((uint32_t)1<<9);
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~((uint32_t)0xf<<(1<<2)))
                      |  ((uint32_t)0x1<<(1<<2));
  #elif DEBUG_TX == 1
    // Setup pin PA9 (TX), remapped to PA11 by SYSCFG
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(9<<1)))
                      |  ((uint32_t)0b10<<(9<<1));
    GPIOA->OTYPER  &= ~((uint32_t)1<<9);
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~((uint32_t)0xf<<(1<<2)))
                      |  ((uint32_t)0x1<<(1<<2));
    RCC->APBENR2   |= RCC_APBENR2_SYSCFGEN;
    SYSCFG->CFGR1  |= SYSCFG_CFGR1_PA11_RMP;
  #elif DEBUG_TX == 2
    // Setup pin PB6 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~((uint32_t)0b11<<(6<<1)))
                      |  ((uint32_t)0b10<<(6<<1));
    GPIOB->OTYPER  &= ~((uint32_t)1<<6);
    GPIOB->AFR[0]  &= ~((uint32_t)0xf<<(6<<2));
  #elif DEBUG_TX == 3
    // Setup pin PA0 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(0<<1)))
                      |  ((uint32_t)0b10<<(0<<1));
    GPIOA->OTYPER  &= ~((uint32_t)1<<0);
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~((uint32_t)0xf<<(0<<2)))
                      |  ((uint32_t)0x4<<(0<<2));
  #elif DEBUG_TX == 4
    // Setup pin PC14 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOCEN;
    GPIOC->MODER    = (GPIOC->MODER  & ~((uint32_t)0b11<<(14<<1)))
                      |  ((uint32_t)0b10<<(14<<1));
    GPIOC->OTYPER  &= ~((uint32_t)1<<14);
    GPIOC->AFR[1]  &= ~((uint32_t)0xf<<(6<<2));
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
