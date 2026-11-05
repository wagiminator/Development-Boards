// ===================================================================================
// CMSIS-DAP SWO Trace (UART/NRZ Mode) for CH32X035                           * v1.0 *
// ===================================================================================
//
// Implementation of the DAP_SWO_xxx commands for a bare-metal system without RTOS.
// The SWO signal is received by a USART (RX only) with a circular DMA channel into
// the trace buffer (see usart_dma.c). Trace data is read by the host either with the
// DAP_SWO_Data command (transport 1) or streamed via the SWO bulk IN endpoint of the
// CMSIS-DAP v2 interface (transport 2, SWO_task() in the main loop).
//
// Based on: CMSIS-DAP SWO.c, Copyright (c) 2013-2022 ARM Limited,
// SPDX-License-Identifier: Apache-2.0
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "DAP_config.h"
#include "cmsis_dap/DAP.h"
#include "usart_dma.h"
#include "dap_swo.h"

#if (SWO_UART != 0)

// ===================================================================================
// Variables
// ===================================================================================
static uint8_t  SWO_buffer[SWO_BUFFER_SIZE] __attribute__((aligned(4)));
static USARTX_t SWO_port = {
  .rxbuf  = SWO_buffer,
  .rxsize = SWO_BUFFER_SIZE,
  .txbuf  = 0,
  .txsize = 0
};

static uint8_t  TraceTransport = 0;       // 0: none, 1: DAP_SWO_Data, 2: stream
static uint8_t  TraceMode      = 0;       // DAP_SWO_OFF or DAP_SWO_UART
static uint8_t  TraceStatus    = 0;       // DAP_SWO_CAPTURE_ACTIVE
static uint8_t  TraceError     = 0;       // error flags
static volatile uint8_t  TransferBusy = 0;// streaming transfer active
static volatile uint32_t TransferSize = 0;// size of streaming transfer

// ===================================================================================
// Helper Functions
// ===================================================================================

// Number of trace bytes in buffer
static uint32_t SWO_getCount(void) {
  uint32_t cnt = USARTX_rxAvailable(&SWO_port);
  if(USARTX_errors(&SWO_port) & USARTX_ERR_LOST) TraceError |= DAP_SWO_BUFFER_OVERRUN;
  if(TransferBusy) cnt -= TransferSize;   // data in streaming transfer
  return cnt;
}

// Trace status (active flag and error flags, errors are cleared)
static uint8_t SWO_getStatus(void) {
  uint8_t status = TraceStatus | TraceError;
  TraceError = 0;
  return status;
}

// Abort streaming transfer and clear trace buffer
static void SWO_clear(void) {
  if(TransferBusy) {
    SWO_AbortTransfer();
    TransferBusy = 0;
  }
  USARTX_rxFlush(&SWO_port);
  TraceError = 0;
}

// ===================================================================================
// Init Function (called once at startup)
// ===================================================================================
void SWO_init(void) {
  USARTX_setup(&SWO_port, SWO_USART, SWO_REMAP);
  if((PIN_SWO == PC18) || (PIN_SWO == PC19))       // pin of the SDI debug interface
    AFIO->PCFR1 = (AFIO->PCFR1 & ~AFIO_PCFR1_SWJ_CFG) | AFIO_PCFR1_SWJ_CFG_2;  // SDI off
  DAP_pinInputPU(PIN_SWO);
}

// ===================================================================================
// DAP SWO Commands
// ===================================================================================

// Process SWO Transport command (0: none, 1: DAP_SWO_Data, 2: streaming)
uint32_t SWO_Transport(const uint8_t *request, uint8_t *response) {
  uint8_t transport = *request;
  uint8_t result = 0;
  if(!(TraceStatus & DAP_SWO_CAPTURE_ACTIVE) && (transport <= 2)) {
    TraceTransport = transport;
    result = 1;
  }
  *response = result ? DAP_OK : DAP_ERROR;
  return ((1U << 16) | 1U);
}

// Process SWO Mode command (0: off, 1: UART)
uint32_t SWO_Mode(const uint8_t *request, uint8_t *response) {
  uint8_t mode = *request;
  uint8_t result = 0;

  // Stop current mode
  if(TraceMode == DAP_SWO_UART) USARTX_rxDisable(&SWO_port);
  TraceStatus = 0;

  switch(mode) {
    case DAP_SWO_OFF:  result = 1; break;
    case DAP_SWO_UART: result = 1; break;
    default:           result = 0; break;
  }
  TraceMode = result ? mode : DAP_SWO_OFF;
  *response = result ? DAP_OK : DAP_ERROR;
  return ((1U << 16) | 1U);
}

// Process SWO Baudrate command, returns actual baudrate (0: not configured)
uint32_t SWO_Baudrate(const uint8_t *request, uint8_t *response) {
  uint32_t baudrate = (uint32_t)request[0]        | ((uint32_t)request[1] << 8)
                   | ((uint32_t)request[2] << 16) | ((uint32_t)request[3] << 24);
  if((TraceMode == DAP_SWO_UART) && (baudrate <= SWO_UART_MAX_BAUDRATE)
     && (USARTX_config(&SWO_port, baudrate, 8, 0, 1) == USARTX_OK)) {
    baudrate = SWO_port.baudrate;
  }
  else {
    baudrate = 0;
    if(TraceStatus) USARTX_rxDisable(&SWO_port);
    TraceStatus = 0;
  }
  response[0] = (uint8_t)(baudrate >>  0);
  response[1] = (uint8_t)(baudrate >>  8);
  response[2] = (uint8_t)(baudrate >> 16);
  response[3] = (uint8_t)(baudrate >> 24);
  return ((4U << 16) | 4U);
}

// Process SWO Control command (start/stop capture)
uint32_t SWO_Control(const uint8_t *request, uint8_t *response) {
  uint8_t active = *request & DAP_SWO_CAPTURE_ACTIVE;
  uint8_t result = 1;
  if(active != (TraceStatus & DAP_SWO_CAPTURE_ACTIVE)) {
    if(TraceMode != DAP_SWO_UART) result = 0;
    else if(active) {
      if(!SWO_port.baudrate) result = 0;
      else {
        SWO_clear();
        USARTX_rxEnable(&SWO_port);
      }
    }
    else USARTX_rxDisable(&SWO_port);
    if(result) TraceStatus = active;
  }
  *response = result ? DAP_OK : DAP_ERROR;
  return ((1U << 16) | 1U);
}

// Process SWO Status command
uint32_t SWO_Status(uint8_t *response) {
  uint32_t count  = SWO_getCount();
  uint8_t  status = SWO_getStatus();
  response[0] = status;
  response[1] = (uint8_t)(count >>  0);
  response[2] = (uint8_t)(count >>  8);
  response[3] = (uint8_t)(count >> 16);
  response[4] = (uint8_t)(count >> 24);
  return 5U;
}

// Process SWO Extended Status command
uint32_t SWO_ExtendedStatus(const uint8_t *request, uint8_t *response) {
  uint8_t  cmd = *request;
  uint32_t num = 0, count, index, tick;
  if(cmd & 0x01) {
    count = SWO_getCount();
    response[num++] = SWO_getStatus();
  }
  if(cmd & 0x02) {
    count = SWO_getCount();
    response[num++] = (uint8_t)(count >>  0);
    response[num++] = (uint8_t)(count >>  8);
    response[num++] = (uint8_t)(count >> 16);
    response[num++] = (uint8_t)(count >> 24);
  }
  if(cmd & 0x04) {
    index = USARTX_rxHead(&SWO_port);
    tick  = TIMESTAMP_GET();
    response[num++] = (uint8_t)(index >>  0);
    response[num++] = (uint8_t)(index >>  8);
    response[num++] = (uint8_t)(index >> 16);
    response[num++] = (uint8_t)(index >> 24);
    response[num++] = (uint8_t)(tick  >>  0);
    response[num++] = (uint8_t)(tick  >>  8);
    response[num++] = (uint8_t)(tick  >> 16);
    response[num++] = (uint8_t)(tick  >> 24);
  }
  (void)count;
  return ((1U << 16) | num);
}

// Process SWO Data command (read trace data, transport 1 only)
uint32_t SWO_Data(const uint8_t *request, uint8_t *response) {
  uint32_t count  = SWO_getCount();
  uint8_t  status = SWO_getStatus();
  uint32_t n;
  if(TraceTransport == 1) {
    n = (uint32_t)request[0] | ((uint32_t)request[1] << 8);
    if(n > (DAP_PACKET_SIZE - 4U)) n = DAP_PACKET_SIZE - 4U;
    if(count > n) count = n;
    count = USARTX_rxRead(&SWO_port, &response[3], count);
  }
  else count = 0;
  response[0] = status;
  response[1] = (uint8_t)(count >> 0);
  response[2] = (uint8_t)(count >> 8);
  return ((2U << 16) | (3U + count));
}

// ===================================================================================
// SWO Streaming (Transport 2)
// ===================================================================================

// Streaming transfer completed (called from USB interrupt)
void SWO_TransferComplete(void) {
  USARTX_rxSkip(&SWO_port, TransferSize);
  TransferSize = 0;
  TransferBusy = 0;
}

// Send trace data via the SWO endpoint (called from main loop)
void SWO_task(void) {
  uint8_t* ptr;
  uint32_t cnt;
  if((TraceTransport != 2) || TransferBusy) return;
  cnt = USARTX_rxPeek(&SWO_port, &ptr);
  if(USARTX_errors(&SWO_port) & USARTX_ERR_LOST) TraceError |= DAP_SWO_BUFFER_OVERRUN;
  if(!cnt) return;
  if(cnt > SWO_STREAM_PACKET) cnt = SWO_STREAM_PACKET;
  TransferSize = cnt;
  TransferBusy = 1;
  SWO_QueueTransfer(ptr, cnt);
}

#else
void SWO_init(void) {}
void SWO_task(void) {}
void SWO_TransferComplete(void) {}
#endif // SWO_UART
