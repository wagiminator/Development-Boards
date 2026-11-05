// ===================================================================================
// CMSIS-DAP v2 USB Transport for CH32X035                                    * v1.0 *
// ===================================================================================
//
// Bulk endpoint transport of the CMSIS-DAP v2 interface for a bare-metal system
// (port of the ARM example USBD_User_CustomClass_0.c without RTOS):
// - EP1 OUT: DAP requests, EP1 IN: DAP responses, EP2 IN: SWO trace stream.
// - Requests and responses are buffered in DAP_PACKET_COUNT packets of
//   DAP_PACKET_SIZE bytes each. The USB interrupt only moves data, the commands are
//   executed in the main loop by DAP_task().
// - DAP packets larger than a USB packet are transferred as multiple USB packets.
//   The end of a request is detected by a short packet, by the full packet size or
//   by the request length of the DAP command (hosts do not send zero length packets).
//   A response with a length that is a multiple of 64 is terminated by a zero
//   length packet.
// - DAP_TransferAbort is executed immediately in the USB interrupt.
// - DAP_QueueCommands packets are executed when the following non-queued packet
//   has been received.
//
// Based on: CMSIS-DAP USBD_User_CustomClass_0.c, Copyright (c) 2021 ARM Limited,
// SPDX-License-Identifier: Apache-2.0
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "DAP_config.h"
#include "cmsis_dap/DAP.h"
#include "usb_dap.h"
#include "usb_handler.h"

// ===================================================================================
// Variables
// ===================================================================================
#define DAP_OUT_buffer  (EP1_buffer)
#define DAP_IN_buffer   (EP1_buffer + 64)
#define SWO_IN_buffer   (EP2_buffer)

static uint8_t  DAP_request [DAP_PACKET_COUNT][DAP_PACKET_SIZE] __attribute__((aligned(4)));
static uint8_t  DAP_response[DAP_PACKET_COUNT][DAP_PACKET_SIZE] __attribute__((aligned(4)));
static uint16_t DAP_respSize[DAP_PACKET_COUNT];

static volatile uint8_t  DAP_reqCountI;   // received requests (USB interrupt)
static volatile uint8_t  DAP_reqCountO;   // executed requests (main loop)
static volatile uint8_t  DAP_reqIdle;     // request endpoint is not armed (buffer full)
static volatile uint16_t DAP_reqPos;      // receive position in current request

static volatile uint8_t  DAP_respCountI;  // prepared responses (main loop)
static volatile uint8_t  DAP_respCountO;  // completely sent responses (USB interrupt)
static volatile uint8_t  DAP_respBusy;    // response transfer active
static volatile uint16_t DAP_respPos;     // send position in current response
static volatile uint8_t  DAP_respZLP;     // zero length packet pending
static volatile uint8_t  DAP_usbGen;      // incremented on USB reset

volatile uint8_t DAP_LED_connected = 0;   // connected LED state (DAP_config.h)
volatile uint8_t DAP_LED_running   = 0;   // running LED state (DAP_config.h)
volatile uint8_t DAP_activity      = 0;   // set when a command was executed

#define IDX(c)  ((c) & (DAP_PACKET_COUNT - 1))

// ===================================================================================
// DAP Request Length Parser
// ===================================================================================
// Returns the total length of the DAP command at buf (len bytes available),
// 0 if more bytes are needed to determine the length, -1 if unknown.
static int32_t DAP_cmdLength(const uint8_t* buf, uint32_t len, uint8_t depth) {
  uint32_t n, i, cnt, pos;
  uint8_t  req;
  int32_t  sub;
  if(len < 1) return 0;

  switch(buf[0]) {
    case ID_DAP_Info:               return 2;
    case ID_DAP_HostStatus:         return 3;
    case ID_DAP_Connect:            return 2;
    case ID_DAP_Disconnect:         return 1;
    case ID_DAP_TransferConfigure:  return 6;
    case ID_DAP_TransferAbort:      return 1;
    case ID_DAP_WriteABORT:         return 6;
    case ID_DAP_Delay:              return 3;
    case ID_DAP_ResetTarget:        return 1;
    case ID_DAP_SWJ_Pins:           return 7;
    case ID_DAP_SWJ_Clock:          return 5;
    case ID_DAP_SWD_Configure:      return 2;
    case ID_DAP_JTAG_IDCODE:        return 2;
    case ID_DAP_SWO_Transport:      return 2;
    case ID_DAP_SWO_Mode:           return 2;
    case ID_DAP_SWO_Baudrate:       return 5;
    case ID_DAP_SWO_Control:        return 2;
    case ID_DAP_SWO_Status:         return 1;
    case ID_DAP_SWO_ExtendedStatus: return 2;
    case ID_DAP_SWO_Data:           return 3;
    case ID_DAP_UART_Transport:     return 2;
    case ID_DAP_UART_Configure:     return 6;
    case ID_DAP_UART_Control:       return 2;
    case ID_DAP_UART_Status:        return 1;

    case ID_DAP_UART_Transfer:      // rx count (2), tx count (2), tx data
      if(len < 5) return 0;
      return 5 + ((uint32_t)buf[3] | ((uint32_t)buf[4] << 8));

    case ID_DAP_SWJ_Sequence:       // count (1, 0 = 256), data
      if(len < 2) return 0;
      cnt = buf[1] ? buf[1] : 256;
      return 2 + ((cnt + 7) >> 3);

    case ID_DAP_JTAG_Configure:     // count, IR lengths
      if(len < 2) return 0;
      return 2 + buf[1];

    case ID_DAP_SWD_Sequence:       // count, {info, [data if output]}
    case ID_DAP_JTAG_Sequence:      // count, {info, tdi data}
      if(len < 2) return 0;
      pos = 2;
      for(i=0; i<buf[1]; i++) {
        if(pos >= len) return 0;
        n = buf[pos] & 0x3F;
        if(!n) n = 64;
        pos++;
        if((buf[0] == ID_DAP_JTAG_Sequence) || !(buf[pos-1] & 0x80)) pos += (n + 7) >> 3;
      }
      return pos;

    case ID_DAP_Transfer:           // index, count, {request, [data]}
      if(len < 3) return 0;
      pos = 3;
      for(i=0; i<buf[2]; i++) {
        if(pos >= len) return 0;
        req = buf[pos++];
        if(!(req & DAP_TRANSFER_RnW) || (req & DAP_TRANSFER_MATCH_VALUE)) pos += 4;
      }
      return pos;

    case ID_DAP_TransferBlock:      // index, count (2), request, [data]
      if(len < 5) return 0;
      cnt = (uint32_t)buf[2] | ((uint32_t)buf[3] << 8);
      return 5 + ((buf[4] & DAP_TRANSFER_RnW) ? 0 : (cnt << 2));

    case ID_DAP_ExecuteCommands:    // count, commands
    case ID_DAP_QueueCommands:
      if(depth) return -1;
      if(len < 2) return 0;
      pos = 2;
      for(i=0; i<buf[1]; i++) {
        if(pos >= len) return 0;
        sub = DAP_cmdLength(buf + pos, len - pos, 1);
        if(sub <= 0) return sub;
        pos += sub;
      }
      return pos;

    default:                        // vendor and unknown commands
      return -1;
  }
}

// ===================================================================================
// USB Endpoint Handlers (called from USB interrupt)
// ===================================================================================

// Setup endpoints (USB reset)
void DAP_EP_init(void) {
  DAP_usbGen++;                                           // discard running command
  USB_EP_init(1, USB_ENDP_TYPE_BULK, EP1_buffer, 1, 1);   // EP1: DAP OUT and IN
  USB_EP_init(2, USB_ENDP_TYPE_BULK, EP2_buffer, 0, 1);   // EP2: SWO IN
  DAP_reqCountI  = 0;
  DAP_reqCountO  = 0;
  DAP_reqIdle    = 0;
  DAP_reqPos     = 0;
  DAP_respCountI = 0;
  DAP_respCountO = 0;
  DAP_respBusy   = 0;
  DAP_respPos    = 0;
  DAP_respZLP    = 0;
  #if (SWO_STREAM != 0)
  SWO_TransferComplete();                                 // release pending transfer
  #endif
  #if (DAP_UART_USB_COM_PORT != 0)
  CDC_EP_init();
  #endif
}

// Send next USB packet of current response
static void DAP_sendPacket(void) {
  uint8_t  idx = IDX(DAP_respCountO);
  uint16_t len = DAP_respSize[idx] - DAP_respPos;
  uint16_t i;
  if(len > EP1_SIZE) len = EP1_SIZE;
  for(i=0; i<len; i++) DAP_IN_buffer[i] = DAP_response[idx][DAP_respPos + i];
  DAP_respPos += len;
  DAP_respZLP  = (len == EP1_SIZE) && (DAP_respPos == DAP_respSize[idx]);
  USB_EP_send(1, len);
}

// Endpoint 1 IN handler (USB packet of response sent)
void DAP_EP1_IN(void) {
  uint8_t idx = IDX(DAP_respCountO);
  if(DAP_respPos < DAP_respSize[idx]) {                   // more packets
    DAP_sendPacket();
    return;
  }
  if(DAP_respZLP) {                                       // terminate with ZLP
    DAP_respZLP = 0;
    USB_EP_send(1, 0);
    return;
  }
  DAP_respCountO++;                                       // response completed
  DAP_respPos = 0;
  if(DAP_respCountO != DAP_respCountI) DAP_sendPacket();  // next response
  else DAP_respBusy = 0;
}

// Endpoint 1 OUT handler (USB packet of request received)
void DAP_EP1_OUT(void) {
  uint8_t* req = DAP_request[IDX(DAP_reqCountI)];
  uint16_t len = USB_RX_len;
  uint16_t i;
  int32_t  cmdlen;

  // Copy packet into request buffer
  if(len > DAP_PACKET_SIZE - DAP_reqPos) len = DAP_PACKET_SIZE - DAP_reqPos;
  for(i=0; i<len; i++) req[DAP_reqPos + i] = DAP_OUT_buffer[i];
  DAP_reqPos += len;

  // Check if request is complete
  if((USB_RX_len == EP1_SIZE) && (DAP_reqPos < DAP_PACKET_SIZE)) {
    cmdlen = DAP_cmdLength(req, DAP_reqPos, 0);
    if((cmdlen == 0) || (cmdlen > (int32_t)DAP_reqPos)) { // more packets to come
      USB_EP_receive(1);
      return;
    }
  }
  if(!DAP_reqPos) {                                       // ZLP: ignore
    USB_EP_receive(1);
    return;
  }

  // Request complete
  DAP_reqPos = 0;
  if(req[0] == ID_DAP_TransferAbort) {                    // abort: no response
    DAP_TransferAbort = 1;
  }
  else {
    DAP_reqCountI++;
    if((uint8_t)(DAP_reqCountI - DAP_reqCountO) == DAP_PACKET_COUNT) {
      DAP_reqIdle = 1;                                    // buffer full: NAK
      return;
    }
  }
  USB_EP_receive(1);
}

// Endpoint 2 IN handler (SWO trace packet sent)
void DAP_EP2_IN(void) {
  #if (SWO_STREAM != 0)
  SWO_TransferComplete();
  #endif
}

// Vendor setup requests (Microsoft OS 2.0 descriptor)
uint8_t DAP_vendorSetup(void) {
  #if USB_WINUSB > 0
  uint8_t len;
  if((USB_SetupReq == USB_MS_VENDOR_CODE) && (USB_SetupTyp & USB_REQ_TYP_IN)
     && (USB_SetupBuf->wIndexL == 0x07) && (USB_SetupBuf->wIndexH == 0x00)) {
    USB_pDescr = MSOS20Descr;
    if(USB_SetupLen > USB_MSOS20_SET_LEN) USB_SetupLen = USB_MSOS20_SET_LEN;
    len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
    USB_EP0_copyDescr(len);
    return len;
  }
  #endif
  return 0xff;                                            // not supported -> STALL
}

// ===================================================================================
// SWO Streaming Transfer (called by dap_swo.c)
// ===================================================================================
#if (SWO_STREAM != 0)
void SWO_QueueTransfer(uint8_t *buf, uint32_t num) {
  uint32_t i;
  if(num > EP2_SIZE) num = EP2_SIZE;
  for(i=0; i<num; i++) SWO_IN_buffer[i] = buf[i];
  USB_EP_send(2, num);
}

void SWO_AbortTransfer(void) {
  USB_EP_abort(2);
}
#endif

// ===================================================================================
// Main Loop Functions
// ===================================================================================

// Init DAP and USB
void DAP_init(void) {
  DAP_Setup();
  USB_initSerial();
  USB_init();
}

// Check if USB is configured
uint8_t DAP_usbReady(void) {
  return USB_ENUM_OK;
}

// Process received DAP requests (call in main loop)
void DAP_task(void) {
  uint8_t  idx, cnt, n, found, gen;
  uint8_t* req;

  // Check for request and free response buffer
  if(DAP_reqCountI == DAP_reqCountO) return;
  if((uint8_t)(DAP_respCountI - DAP_respCountO) == DAP_PACKET_COUNT) return;

  // Queued commands are executed when the following packet has been received
  req = DAP_request[IDX(DAP_reqCountO)];
  if(req[0] == ID_DAP_QueueCommands) {
    cnt = DAP_reqCountI - DAP_reqCountO;
    found = 0;
    for(n=0; n<cnt; n++) {
      if(DAP_request[IDX(DAP_reqCountO + n)][0] != ID_DAP_QueueCommands) {
        found = 1;
        break;
      }
    }
    if(!found && (cnt < DAP_PACKET_COUNT)) return;        // wait for more packets
    for(n=0; n<cnt; n++) {
      uint8_t* r = DAP_request[IDX(DAP_reqCountO + n)];
      if(r[0] != ID_DAP_QueueCommands) break;
      r[0] = ID_DAP_ExecuteCommands;
    }
  }

  // Execute command
  gen = DAP_usbGen;
  idx = IDX(DAP_respCountI);
  DAP_respSize[idx] = (uint16_t)DAP_ExecuteCommand(req, DAP_response[idx]);
  DAP_activity = 1;

  // Release request buffer
  __disable_irq();
  if(gen != DAP_usbGen) {                                 // USB reset during execution
    __enable_irq();
    return;
  }
  DAP_reqCountO++;
  if(DAP_reqIdle) {
    DAP_reqIdle = 0;
    USB_EP_receive(1);
  }

  // Send response
  DAP_respCountI++;
  if(!DAP_respBusy) {
    DAP_respBusy = 1;
    DAP_respPos  = 0;
    DAP_sendPacket();
  }
  __enable_irq();
}
