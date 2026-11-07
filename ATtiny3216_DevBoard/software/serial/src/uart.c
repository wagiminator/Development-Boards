// ===================================================================================
// Basic USART Functions (no buffer, no interrupt) for tinyAVR                * v1.1 *
// ===================================================================================
// 2021 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"

// BAUD rate calculation (switch to double-speed mode if necessary)
#if ((F_CPU * 4) / UART_BAUD) < 64
  #define UART_CLK2X    1
  #define UART_BAUD_REG ((((F_CPU * 16) / UART_BAUD) + 1) / 2)
#else
  #define UART_CLK2X    0
  #define UART_BAUD_REG ((((F_CPU *  8) / UART_BAUD) + 1) / 2)
#endif
#if UART_BAUD_REG < 64
  #warning BAUD rate too high for F_CPU
#endif

#if UART_OSCCOMP == 3
  #define UART_BAUD_VAL (uint16_t)(UART_BAUD_REG + ((int32_t)UART_BAUD_REG * (int8_t)HSI_ERR_read())   / 1024)
#elif UART_OSCCOMP == 5
  #define UART_BAUD_VAL (uint16_t)(UART_BAUD_REG + ((int32_t)UART_BAUD_REG * (int8_t)HSI_ERR5V_read()) / 1024)
#else
  #define UART_BAUD_VAL UART_BAUD_REG
#endif

// Init UART
void UART_init(void) {
  // Set GPIO pins (TX output high to avoid a start bit glitch, pullup on RX)
  #if UART_MAP == 0
    #ifdef PORTB
    VPORTB.OUT |= (1 << 2); VPORTB.DIR |= (1 << 2); // PB2 (TX)
    PORTB.PIN3CTRL = PORT_PULLUPEN_bm;              // PB3 (RX)
    #else
    VPORTA.OUT |= (1 << 6); VPORTA.DIR |= (1 << 6); // PA6 (TX, 8-pin)
    PORTA.PIN7CTRL = PORT_PULLUPEN_bm;              // PA7 (RX, 8-pin)
    #endif
  #elif UART_MAP == 1
    VPORTA.OUT |= (1 << 1); VPORTA.DIR |= (1 << 1); // PA1 (TX)
    PORTA.PIN2CTRL = PORT_PULLUPEN_bm;              // PA2 (RX)
    PORTMUX.CTRLB |= PORTMUX_USART0_bm;
  #else
    #warning No automatic pin mapping for USART0
  #endif

  // Set BAUD rate
  USART0.BAUD  = UART_BAUD_VAL;
  #if UART_CLK2X > 0
  USART0.CTRLB = USART_RXMODE_CLK2X_gc;           // double-speed mode
  #endif

  // Start UART (8N1, RX/TX)
  USART0.CTRLB |= USART_TXEN_bm | USART_RXEN_bm;
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());
  return USART0.RXDATAL;
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  USART0.TXDATAL = c;
}
