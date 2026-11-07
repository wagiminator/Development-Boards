// ===================================================================================
// Basic USART Functions (no buffer, no interrupt) for megaAVR                * v1.1 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart.h"

// USART pins
#if   UART_NUM == 0
  #define UART_VPORT    VPORTA
  #define UART_PORT     PORTA
#elif UART_NUM == 1
  #define UART_VPORT    VPORTC
  #define UART_PORT     PORTC
#elif UART_NUM == 2
  #define UART_VPORT    VPORTF
  #define UART_PORT     PORTF
#elif UART_NUM == 3
  #define UART_VPORT    VPORTB
  #define UART_PORT     PORTB
#else
  #error Wrong UART_NUM
#endif
#define UART_TX         (UART_MAP * 4)            // TX pin number, RX pin is TX + 1

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
  #if UART_MAP < 2
  UART_VPORT.OUT |= (1 << UART_TX);               // TX pin output high
  UART_VPORT.DIR |= (1 << UART_TX);
  (&UART_PORT.PIN0CTRL)[UART_TX + 1] = PORT_PULLUPEN_bm; // RX pin pullup
  PORTMUX.USARTROUTEA = (PORTMUX.USARTROUTEA & ~(3 << (UART_NUM * 2)))
                      | (UART_MAP << (UART_NUM * 2)); // set pin mapping
  #else
    #warning No automatic pin mapping for USART
  #endif

  // Set BAUD rate
  UART_USART.BAUD  = UART_BAUD_VAL;
  #if UART_CLK2X > 0
  UART_USART.CTRLB = USART_RXMODE_CLK2X_gc;           // double-speed mode
  #endif

  // Start UART (8N1, RX/TX)
  UART_USART.CTRLB |= USART_TXEN_bm | USART_RXEN_bm;
}

// Read byte via UART
char UART_read(void) {
  while(!UART_available());
  return UART_USART.RXDATAL;
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  UART_USART.TXDATAL = c;
}
