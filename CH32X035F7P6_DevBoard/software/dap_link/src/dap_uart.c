// ===================================================================================
// CMSIS-DAP UART Communication Port for CH32X035                             * v1.0 *
// ===================================================================================
//
// Implementation of the DAP_UART_xxx commands for a bare-metal system without RTOS.
// The UART of the debug probe can either be used via the USB virtual COM port
// (transport 1, default, see usb_cdc.c) or via the DAP_UART_Transfer command
// (transport 2). Both use the same USART with DMA ring buffers (usart_dma.c).
//
// Based on: CMSIS-DAP UART.c, Copyright (c) 2021 ARM Limited,
// SPDX-License-Identifier: Apache-2.0
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "DAP_config.h"
#include "cmsis_dap/DAP.h"
#include "usart_dma.h"
#include "dap_uart.h"

#if (DAP_UART != 0)

// ===================================================================================
// Variables
// ===================================================================================
static uint8_t UART_rxBuffer[DAP_UART_RX_BUFFER_SIZE] __attribute__((aligned(4)));
static uint8_t UART_txBuffer[DAP_UART_TX_BUFFER_SIZE] __attribute__((aligned(4)));

USARTX_t UART_port = {
  .rxbuf  = UART_rxBuffer,
  .rxsize = DAP_UART_RX_BUFFER_SIZE,
  .txbuf  = UART_txBuffer,
  .txsize = DAP_UART_TX_BUFFER_SIZE
};

static uint8_t UART_transport  = DAP_UART_TRANSPORT_NONE;
static uint8_t UART_configured = 0;

// ===================================================================================
// Init Function (called once at startup)
// ===================================================================================
void UART_init(void) {
  USARTX_setup(&UART_port, UART_USART, UART_REMAP);
  DAP_pinHigh(PIN_UART_TX);
  DAP_pinConfig(PIN_UART_TX, DAP_CFG_AF_PP);
  DAP_pinInputPU(PIN_UART_RX);
  #if (DAP_UART_USB_COM_PORT != 0)
  if(USB_COM_PORT_Activate(1) == 0) UART_transport = DAP_UART_TRANSPORT_USB_COM_PORT;
  #endif
}

// ===================================================================================
// Helper Functions
// ===================================================================================

// Stop UART used by DAP commands
static void UART_stop(void) {
  USARTX_rxDisable(&UART_port);
  USARTX_txDisable(&UART_port);
  UART_configured = 0;
}

// Get status byte (error flags are cleared)
static uint8_t UART_getStatus(void) {
  uint8_t status = 0;
  uint8_t err = USARTX_errors(&UART_port);
  if(UART_port.rxenabled)          status |= DAP_UART_STATUS_RX_ENABLED;
  if(err & USARTX_ERR_LOST)        status |= DAP_UART_STATUS_RX_DATA_LOST;
  if(err & USARTX_ERR_FRAMING)     status |= DAP_UART_STATUS_FRAMING_ERROR;
  if(err & USARTX_ERR_PARITY)      status |= DAP_UART_STATUS_PARITY_ERROR;
  if(UART_port.txenabled)          status |= DAP_UART_STATUS_TX_ENABLED;
  return status;
}

// ===================================================================================
// DAP UART Commands
// ===================================================================================

// Process UART Transport command (0: none, 1: USB COM port, 2: DAP commands)
uint32_t UART_Transport(const uint8_t *request, uint8_t *response) {
  uint8_t transport = *request;
  uint8_t ret = DAP_ERROR;

  if(transport == UART_transport) ret = DAP_OK;
  else if(transport <= DAP_UART_TRANSPORT_DAP_COMMAND) {
    // Stop current transport
    #if (DAP_UART_USB_COM_PORT != 0)
    if(UART_transport == DAP_UART_TRANSPORT_USB_COM_PORT) USB_COM_PORT_Activate(0);
    #endif
    if(UART_transport == DAP_UART_TRANSPORT_DAP_COMMAND) UART_stop();
    UART_transport = DAP_UART_TRANSPORT_NONE;

    // Start new transport
    switch(transport) {
      case DAP_UART_TRANSPORT_NONE:
        ret = DAP_OK;
        break;
      #if (DAP_UART_USB_COM_PORT != 0)
      case DAP_UART_TRANSPORT_USB_COM_PORT:
        if(USB_COM_PORT_Activate(1) == 0) {
          UART_transport = DAP_UART_TRANSPORT_USB_COM_PORT;
          ret = DAP_OK;
        }
        break;
      #endif
      case DAP_UART_TRANSPORT_DAP_COMMAND:
        UART_configured = 0;
        UART_transport = DAP_UART_TRANSPORT_DAP_COMMAND;
        ret = DAP_OK;
        break;
      default:
        break;
    }
  }
  *response = ret;
  return ((1U << 16) | 1U);
}

// Process UART Configure command
// Control: bit 3..0: data bits (0 = 8, 5..7), bit 5..4: parity (0 = none, 1 = even,
// 2 = odd), bit 7..6: stop bits (0 = 1, 1 = 2)
uint32_t UART_Configure(const uint8_t *request, uint8_t *response) {
  uint8_t  control = request[0];
  uint32_t baudrate = (uint32_t)request[1]        | ((uint32_t)request[2] << 8)
                   | ((uint32_t)request[3] << 16) | ((uint32_t)request[4] << 24);
  uint8_t  status = 0;
  uint8_t  databits, parity, stopbits, err;

  if(UART_transport != DAP_UART_TRANSPORT_DAP_COMMAND) {
    status = DAP_UART_CFG_ERROR_DATA_BITS | DAP_UART_CFG_ERROR_PARITY
           | DAP_UART_CFG_ERROR_STOP_BITS;
    baudrate = 0;
  }
  else {
    databits = control & 0x0F;
    if(!databits) databits = 8;
    parity   = (control >> 4) & 3;
    stopbits = ((control >> 6) & 3) + 1;
    err = USARTX_config(&UART_port, baudrate, databits, parity, stopbits);
    if(err == USARTX_OK) {
      UART_configured = 1;
      baudrate = UART_port.baudrate;
    }
    else {
      UART_configured = 0;
      if(err & USARTX_CFG_DATABITS) status |= DAP_UART_CFG_ERROR_DATA_BITS;
      if(err & USARTX_CFG_PARITY)   status |= DAP_UART_CFG_ERROR_PARITY;
      if(err & USARTX_CFG_STOPBITS) status |= DAP_UART_CFG_ERROR_STOP_BITS;
      baudrate = 0;
    }
  }
  response[0] = status;
  response[1] = (uint8_t)(baudrate >>  0);
  response[2] = (uint8_t)(baudrate >>  8);
  response[3] = (uint8_t)(baudrate >> 16);
  response[4] = (uint8_t)(baudrate >> 24);
  return ((5U << 16) | 5U);
}

// Process UART Control command (enable/disable/flush RX and TX)
uint32_t UART_Control(const uint8_t *request, uint8_t *response) {
  uint8_t control = *request;
  uint8_t ret = DAP_OK;

  if(UART_transport != DAP_UART_TRANSPORT_DAP_COMMAND) ret = DAP_ERROR;
  else {
    if(control & DAP_UART_CONTROL_RX_DISABLE) USARTX_rxDisable(&UART_port);
    else if(control & DAP_UART_CONTROL_RX_ENABLE) {
      if(!UART_configured) ret = DAP_ERROR;
      else if(!UART_port.rxenabled) USARTX_rxEnable(&UART_port);
    }
    if(control & DAP_UART_CONTROL_RX_BUF_FLUSH) USARTX_rxFlush(&UART_port);

    if(control & DAP_UART_CONTROL_TX_DISABLE) USARTX_txDisable(&UART_port);
    else if(control & DAP_UART_CONTROL_TX_ENABLE) {
      if(!UART_configured) ret = DAP_ERROR;
      else if(!UART_port.txenabled) {
        USARTX_txFlush(&UART_port);
        USARTX_txEnable(&UART_port);
      }
    }
    if(control & DAP_UART_CONTROL_TX_BUF_FLUSH) USARTX_txFlush(&UART_port);
  }
  *response = ret;
  return ((1U << 16) | 1U);
}

// Process UART Status command
uint32_t UART_Status(uint8_t *response) {
  uint32_t rx_cnt = 0, tx_cnt = 0;
  uint8_t  status = 0;
  if((UART_transport == DAP_UART_TRANSPORT_DAP_COMMAND) && UART_configured) {
    rx_cnt = USARTX_rxAvailable(&UART_port);
    tx_cnt = USARTX_txPending(&UART_port);
    status = UART_getStatus();
  }
  response[0] = status;
  response[1] = (uint8_t)(rx_cnt >>  0);
  response[2] = (uint8_t)(rx_cnt >>  8);
  response[3] = (uint8_t)(rx_cnt >> 16);
  response[4] = (uint8_t)(rx_cnt >> 24);
  response[5] = (uint8_t)(tx_cnt >>  0);
  response[6] = (uint8_t)(tx_cnt >>  8);
  response[7] = (uint8_t)(tx_cnt >> 16);
  response[8] = (uint8_t)(tx_cnt >> 24);
  return ((0U << 16) | 9U);
}

// Process UART Transfer command (read received data, write data to transmit)
uint32_t UART_Transfer(const uint8_t *request, uint8_t *response) {
  uint32_t rx_cnt = 0, tx_cnt = 0, tx_req;
  uint8_t  status = 0;

  tx_req = (uint32_t)request[2] | ((uint32_t)request[3] << 8);
  if(tx_req > (DAP_PACKET_SIZE - 5U)) tx_req = DAP_PACKET_SIZE - 5U;

  if(UART_transport == DAP_UART_TRANSPORT_DAP_COMMAND) {
    // Receive data
    rx_cnt = (uint32_t)request[0] | ((uint32_t)request[1] << 8);
    if(rx_cnt > (DAP_PACKET_SIZE - 6U)) rx_cnt = DAP_PACKET_SIZE - 6U;
    rx_cnt = USARTX_rxRead(&UART_port, &response[5], rx_cnt);

    // Transmit data
    if(UART_port.txenabled) tx_cnt = USARTX_txWrite(&UART_port, &request[4], tx_req);
    status = UART_getStatus();
  }
  response[0] = status;
  response[1] = (uint8_t)(tx_cnt >> 0);
  response[2] = (uint8_t)(tx_cnt >> 8);
  response[3] = (uint8_t)(rx_cnt >> 0);
  response[4] = (uint8_t)(rx_cnt >> 8);
  return (((4U + tx_req) << 16) | (5U + rx_cnt));
}

#else
void UART_init(void) {}
#endif // DAP_UART
