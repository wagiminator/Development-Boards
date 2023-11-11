// ===================================================================================
// Basic Serial Debug Functions for STM32L01x, STM32L021, STM32L031           * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include <stdarg.h>
#include "debug_serial.h"

#if DEBUG_ENABLE > 0

// Init UART
void DEBUG_init(void) {
  // Set GPIO pins
  #if DEBUG_TX == 0
    // Setup pin PA2 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(2<<1)))
                                     |  ((uint32_t)0b10<<(2<<1));
    GPIOA->OTYPER  &=                  ~((uint32_t)0b1 <<(2<<0));
    GPIOA->OSPEEDR |=                   ((uint32_t)0b11<<(2<<1));
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~((uint32_t)0xf <<(2<<2)))
                                     |  ((uint32_t)0x4 <<(2<<2));
  #elif DEBUG_TX == 1
    // Setup pin PA9 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(9<<1)))
                                     |  ((uint32_t)0b10<<(9<<1));
    GPIOA->OTYPER  &=                  ~((uint32_t)0b1 <<(9<<0));
    GPIOA->OSPEEDR |=                   ((uint32_t)0b11<<(9<<1));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~((uint32_t)0xf <<(1<<2)))
                                     |  ((uint32_t)0x4 <<(1<<2));
  #elif DEBUG_TX == 2
    // Setup pin PA14 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~((uint32_t)0b11<<(14<<1)))
                                     |  ((uint32_t)0b10<<(14<<1));
    GPIOA->OTYPER  &=                  ~((uint32_t)0b1 <<(14<<0));
    GPIOA->OSPEEDR |=                   ((uint32_t)0b11<<(14<<1));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~((uint32_t)0xf <<( 6<<2)))
                                     |  ((uint32_t)0x4 <<( 6<<2));
  #elif DEBUG_TX == 3
    // Setup pin PB6 (TX)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~((uint32_t)0b11<<(6<<1)))
                                     |  ((uint32_t)0b10<<(6<<1));
    GPIOB->OTYPER  &=                  ~((uint32_t)0b1 <<(6<<0));
    GPIOB->OSPEEDR |=                   ((uint32_t)0b11<<(6<<1));
    GPIOB->AFR[0]  &=                  ~((uint32_t)0xf <<(6<<2));
  #else
    #warning No automatic pin mapping for USART2
  #endif
	
  // Setup and start UART (8N1, TX, default BAUD rate)
  RCC->APB1ENR |= RCC_APB1ENR_USART2EN;
  USART2->BRR = ((2 * F_CPU / DEBUG_BAUD) + 1) / 2;
  USART2->CR1 = USART_CR1_TE | USART_CR1_UE;
}

// Send byte via UART
void DEBUG_write(const char c) {
  while(!(USART2->ISR & USART_ISR_TXE));
  USART2->TDR = c;
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
const uint32_t DIVIDER[] = {1, 10, 100, 1000, 10000, 100000, 1000000,
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
