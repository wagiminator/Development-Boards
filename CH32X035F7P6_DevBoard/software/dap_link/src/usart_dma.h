// ===================================================================================
// USART Driver with DMA Ring Buffers for CH32X035                            * v1.0 *
// ===================================================================================
//
// USART driver for the SWO trace input and the UART of the debug probe without
// byte interrupts. Received data is written by a circular DMA channel into a ring buffer,
// transmit data is sent from a ring buffer by a second DMA channel. Positions are
// handled as free-running 32-bit counters (head/tail), the buffer index is the
// counter modulo the buffer size (2^n). The DMA transfer complete interrupts count
// the RX buffer cycles and restart the TX transfers, so no data is lost while the
// main loop is busy with long debug commands.
//
// Functions available:
// --------------------
// USARTX_setup(u, n, remap)      setup instance u for USART n (1..4) with pin remap
// USARTX_config(u, baud, data, parity, stop)
//                                configure format (data: 7/8, parity: 0=none,
//                                1=even, 2=odd, stop: 1/2), returns USARTX_OK or
//                                error flags, actual baudrate in u->baudrate
// USARTX_rxEnable(u)             start receiving into ring buffer
// USARTX_rxDisable(u)            stop receiving
// USARTX_rxFlush(u)              discard received data
// USARTX_rxHead(u)               absolute number of received bytes
// USARTX_rxAvailable(u)          number of bytes in RX buffer (overrun is handled)
// USARTX_rxRead(u, buf, n)       read up to n bytes, returns number of bytes read
// USARTX_rxPeek(u, &ptr)         get pointer to contiguous RX data, returns length
// USARTX_rxSkip(u, n)            remove n bytes from RX buffer
// USARTX_txEnable(u)             enable transmitter
// USARTX_txDisable(u)            disable transmitter (pending data is discarded)
// USARTX_txFlush(u)              discard pending TX data
// USARTX_txFree(u)               free space in TX buffer
// USARTX_txPending(u)            number of bytes not yet transmitted
// USARTX_txWrite(u, buf, n)      write up to n bytes, returns number of bytes written
// USARTX_errors(u)               get and clear error flags (USARTX_ERR_xxx)
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "system.h"

// Configuration result flags
#define USARTX_OK               0x00
#define USARTX_CFG_DATABITS     0x01      // data bits not supported
#define USARTX_CFG_PARITY       0x02      // parity not supported
#define USARTX_CFG_STOPBITS     0x04      // stop bits not supported
#define USARTX_CFG_BAUDRATE     0x08      // baudrate not supported

// Error flags
#define USARTX_ERR_LOST         0x01      // receive buffer overrun (data lost)
#define USARTX_ERR_FRAMING      0x02      // framing error
#define USARTX_ERR_PARITY       0x04      // parity error

// USART instance
typedef struct {
  USART_TypeDef*        usart;            // USART peripheral
  DMA_Channel_TypeDef*  rxdma;            // RX DMA channel
  DMA_Channel_TypeDef*  txdma;            // TX DMA channel
  uint8_t               rxch, txch;       // DMA channel numbers (1..8)
  uint8_t*              rxbuf;            // RX ring buffer
  uint8_t*              txbuf;            // TX ring buffer (NULL: RX only)
  uint16_t              rxsize, txsize;   // buffer sizes (2^n)
  uint32_t              baudrate;         // actual baudrate
  volatile uint32_t     rxwraps;          // completed RX buffer cycles
  uint32_t              rxtail;           // RX read position
  uint32_t              rxstop;           // RX write position when stopped
  volatile uint32_t     txhead;           // TX write position
  volatile uint32_t     txtail;           // TX read position
  volatile uint16_t     txnum;            // bytes of active TX DMA transfer
  volatile uint8_t      txactive;         // TX DMA transfer active
  uint8_t               rxenabled;        // receiver enabled
  uint8_t               txenabled;        // transmitter enabled
  uint8_t               errors;           // accumulated error flags
} USARTX_t;

void     USARTX_setup(USARTX_t* u, uint8_t n, uint8_t remap);
uint8_t  USARTX_config(USARTX_t* u, uint32_t baud, uint8_t databits, uint8_t parity, uint8_t stopbits);
void     USARTX_rxEnable(USARTX_t* u);
void     USARTX_rxDisable(USARTX_t* u);
void     USARTX_rxFlush(USARTX_t* u);
uint32_t USARTX_rxHead(USARTX_t* u);
uint32_t USARTX_rxAvailable(USARTX_t* u);
uint32_t USARTX_rxRead(USARTX_t* u, uint8_t* buf, uint32_t n);
uint32_t USARTX_rxPeek(USARTX_t* u, uint8_t** ptr);
void     USARTX_rxSkip(USARTX_t* u, uint32_t n);
void     USARTX_txEnable(USARTX_t* u);
void     USARTX_txDisable(USARTX_t* u);
void     USARTX_txFlush(USARTX_t* u);
uint32_t USARTX_txFree(USARTX_t* u);
uint32_t USARTX_txPending(USARTX_t* u);
uint32_t USARTX_txWrite(USARTX_t* u, const uint8_t* buf, uint32_t n);
uint8_t  USARTX_errors(USARTX_t* u);

#ifdef __cplusplus
}
#endif
