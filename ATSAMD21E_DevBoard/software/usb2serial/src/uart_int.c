// ===================================================================================
// UART (SERCOM USART) with RX Interrupt and Ring Buffer for SAMD21          * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "uart_int.h"
#include "gpio.h"

static uint8_t UART_txflag = 0;           // something was transmitted

// UART RX ring buffer
volatile uint8_t UART_buffer[UART_RX_BUF_SIZE];   // UART RX ring buffer
volatile uint8_t UART_rptr = 0;                   // UART RX buffer read pointer
volatile uint8_t UART_wptr = 0;                   // UART RX buffer write pointer

// Disable USART for configuration (wait for ongoing transmission), return CTRLA
static uint32_t UART_stop(void) {
  uint32_t ctrla = UART_SER->USART.CTRLA.reg;
  if(ctrla & SERCOM_USART_CTRLA_ENABLE) {
    if(UART_txflag) while(!UART_completed());       // wait for TX completed
    UART_SER->USART.CTRLA.reg = ctrla & ~SERCOM_USART_CTRLA_ENABLE;
    UART_sync();
  }
  return ctrla;
}

// Re-enable USART after configuration if it was enabled before
static void UART_restart(uint32_t ctrla) {
  if(ctrla & SERCOM_USART_CTRLA_ENABLE) {
    UART_SER->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;
    UART_sync();
  }
}

// Set BAUD rate (fractional BAUD rate generator, 16x oversampling)
void UART_setBAUD(uint32_t baud) {
  uint32_t ctrla = UART_stop();
  uint32_t div8  = (F_CPU + baud) / (baud << 1);    // F_CPU / (16 * baud) * 8
  UART_SER->USART.BAUD.reg = SERCOM_USART_BAUD_FRAC_BAUD(div8 >> 3)
                           | SERCOM_USART_BAUD_FRAC_FP(div8 & 7);
  UART_restart(ctrla);
}

// Change CTRLA register (enable-protected bits)
void UART_setCTRLA(uint32_t mask, uint32_t val) {
  uint32_t ctrla = UART_stop();
  UART_SER->USART.CTRLA.reg = (UART_SER->USART.CTRLA.reg & ~mask) | val;
  UART_restart(ctrla);
}

// Change CTRLB register (enable-protected bits)
void UART_setCTRLB(uint32_t mask, uint32_t val) {
  uint32_t ctrla = UART_stop();
  UART_SER->USART.CTRLB.reg = (UART_SER->USART.CTRLB.reg & ~mask) | val;
  UART_sync();
  UART_restart(ctrla);
}

// Init UART
void UART_init(void) {
  // Set GPIO pins (RX with pullup)
  #ifdef UART_TX_PMUX
  PIN_alternate(UART_TX_PIN, UART_TX_PMUX);
  PIN_input_PU(UART_RX_PIN);
  PIN_alternate(UART_RX_PIN, UART_RX_PMUX);
  #else
    #warning No automatic pin mapping for UART
  #endif

  // Enable SERCOM clocks
  PM->APBCMASK.reg |= UART_APBMASK;                 // enable SERCOM module clock
  GCLK_setCH(UART_GCLK_ID, GCLK_SYS);               // GCLK0 -> SERCOM

  // Setup and start UART (8N1, RX/TX, default BAUD rate)
  UART_SER->USART.CTRLA.reg = SERCOM_USART_CTRLA_SWRST;
  while(UART_SER->USART.CTRLA.reg & SERCOM_USART_CTRLA_SWRST);
  UART_SER->USART.CTRLA.reg = SERCOM_USART_CTRLA_MODE_USART_INT_CLK // internal clock
                            | SERCOM_USART_CTRLA_SAMPR(1)           // 16x fractional
                            | SERCOM_USART_CTRLA_TXPO(UART_TXPO)    // TX pad
                            | SERCOM_USART_CTRLA_RXPO(UART_RXPO)    // RX pad
                            | SERCOM_USART_CTRLA_DORD;              // LSB first
  UART_SER->USART.CTRLB.reg = SERCOM_USART_CTRLB_TXEN               // enable TX
                            | SERCOM_USART_CTRLB_RXEN;              // enable RX
  UART_sync();
  UART_setBAUD(UART_BAUD);                                          // set BAUD rate
  UART_SER->USART.INTENSET.reg = SERCOM_USART_INTENSET_RXC;         // RX interrupt
  NVIC_EnableIRQ(UART_IRQn);                                        // enable interrupt
  UART_SER->USART.CTRLA.reg |= SERCOM_USART_CTRLA_ENABLE;           // enable USART
  UART_sync();
}

// Read byte via UART
char UART_read(void) {
  char result;
  while(!UART_available());                         // wait for incoming data byte
  result = UART_buffer[UART_rptr];                  // get data byte from buffer
  UART_rptr = (UART_rptr + 1) & (UART_RX_BUF_SIZE - 1); // increase read pointer
  return result;                                    // return the received byte
}

// Send byte via UART
void UART_write(const char c) {
  while(!UART_ready());
  UART_SER->USART.DATA.reg = c;
  UART_txflag = 1;
}

// UART RX interrupt service routine
void UART_IRQHandler(void) {
  uint8_t data = UART_SER->USART.DATA.reg;          // read received byte (clears RXC)
  uint8_t next = (UART_wptr + 1) & (UART_RX_BUF_SIZE - 1);
  if(next != UART_rptr) {                           // buffer not full?
    UART_buffer[UART_wptr] = data;                  // push received byte to buffer
    UART_wptr = next;                               // increase write pointer
  }
}
