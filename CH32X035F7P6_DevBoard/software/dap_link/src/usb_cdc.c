// ===================================================================================
// USB CDC-ACM Virtual COM Port for CMSIS-DAP Debug Probe                     * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "DAP_config.h"
#include "cmsis_dap/DAP.h"
#include "usb_cdc.h"
#include "usb_handler.h"
#include "dap_uart.h"

#if (DAP_UART_USB_COM_PORT != 0)

// ===================================================================================
// Variables and Defines
// ===================================================================================
CDC_LINE_CODING_TYPE CDC_lineCoding = {
  .baudrate = 115200,                     // baudrate 115200
  .stopbits = 0,                          // 1 stopbit
  .parity   = 0,                          // no parity
  .databits = 8                           // 8 databits
};

volatile uint8_t CDC_controlLineState = 0;// control line state (DTR, RTS)
static volatile uint8_t CDC_rxLen     = 0;// number of bytes in OUT buffer
static volatile uint8_t CDC_txBusy    = 0;// IN transfer active
static volatile uint8_t CDC_configure = 0;// line coding changed
static uint8_t CDC_txZLP    = 0;          // last IN packet was full
static uint8_t CDC_active   = 0;          // COM port active (UART transport)

// CDC class requests
#define SET_LINE_CODING         0x20      // host configures line coding
#define GET_LINE_CODING         0x21      // host reads configured line coding
#define SET_CONTROL_LINE_STATE  0x22      // generates RS-232/V.24 style control signals
#define SEND_BREAK              0x23      // send break

// Endpoint buffers (EP5: OUT at offset 0, IN at offset 64)
#define CDC_OUT_buffer          (EP5_buffer)
#define CDC_IN_buffer           (EP5_buffer + 64)

// ===================================================================================
// UART Configuration
// ===================================================================================

// Apply line coding to UART
static void CDC_configUART(void) {
  uint8_t parity, stopbits, databits;
  switch(CDC_lineCoding.parity) {
    case 1:  parity = 2; break;           // odd
    case 2:  parity = 1; break;           // even
    default: parity = 0; break;           // none (mark/space not supported)
  }
  stopbits = CDC_lineCoding.stopbits ? 2 : 1;
  databits = CDC_lineCoding.databits;
  if((databits != 7) || !parity) databits = 8;
  USARTX_config(&UART_port, CDC_lineCoding.baudrate, databits, parity, stopbits);
}

// Activate/deactivate USB COM port (called by DAP_UART_Transport), returns 0 on success
uint8_t USB_COM_PORT_Activate(uint32_t cmd) {
  if(cmd) {
    CDC_configUART();
    USARTX_rxEnable(&UART_port);
    USARTX_txFlush(&UART_port);
    USARTX_txEnable(&UART_port);
    CDC_active = 1;
  }
  else {
    CDC_active = 0;
    USARTX_rxDisable(&UART_port);
    USARTX_txDisable(&UART_port);
  }
  return 0;
}

// ===================================================================================
// Main Loop Function
// ===================================================================================
void CDC_task(void) {
  uint8_t* ptr;
  uint32_t cnt, i;

  // Apply new line coding
  if(CDC_configure) {
    CDC_configure = 0;
    if(CDC_active) CDC_configUART();
  }

  // Data from host -> UART TX
  if(CDC_rxLen) {
    if(!CDC_active) {                     // COM port inactive: discard data
      CDC_rxLen = 0;
      USB_EP_receive(5);
    }
    else if(USARTX_txFree(&UART_port) >= CDC_rxLen) {
      USARTX_txWrite(&UART_port, CDC_OUT_buffer, CDC_rxLen);
      CDC_rxLen = 0;
      USB_EP_receive(5);
    }
  }

  // UART RX -> host
  if(CDC_active && !CDC_txBusy && USB_ENUM_OK) {
    cnt = USARTX_rxPeek(&UART_port, &ptr);
    if(cnt) {
      if(cnt > EP5_SIZE) cnt = EP5_SIZE;
      for(i=0; i<cnt; i++) CDC_IN_buffer[i] = ptr[i];
      USARTX_rxSkip(&UART_port, cnt);
      CDC_txZLP  = (cnt == EP5_SIZE);
      CDC_txBusy = 1;
      USB_EP_send(5, cnt);
    }
    else if(CDC_txZLP) {                  // terminate transfer with ZLP
      CDC_txZLP  = 0;
      CDC_txBusy = 1;
      USB_EP_send(5, 0);
    }
  }
}

// ===================================================================================
// CDC-Specific USB Handler Functions
// ===================================================================================

// Setup CDC endpoints
void CDC_EP_init(void) {
  USB_EP_init(3, USB_ENDP_TYPE_INTER, EP3_buffer, 0, 1);  // EP3: notification IN
  USB_EP_init(5, USB_ENDP_TYPE_BULK,  EP5_buffer, 1, 1);  // EP5: data OUT and IN
  CDC_rxLen  = 0;
  CDC_txBusy = 0;
  CDC_txZLP  = 0;
}

// Handle class setup requests
uint8_t CDC_control(void) {
  switch(USB_SetupReq) {
    case GET_LINE_CODING:
      USB_pDescr = (uint8_t*)&CDC_lineCoding;
      if(USB_SetupLen > sizeof(CDC_lineCoding)) USB_SetupLen = sizeof(CDC_lineCoding);
      USB_EP0_copyDescr(USB_SetupLen);
      return USB_SetupLen;
    case SET_CONTROL_LINE_STATE:
      CDC_controlLineState = EP0_buffer[2];
      return 0;
    case SET_LINE_CODING:                 // data follows in OUT stage
    case SEND_BREAK:                      // ignored
      return 0;
    default:
      return 0xff;                        // not supported -> STALL
  }
}

// Endpoint 0 class OUT handler (data stage of SET_LINE_CODING)
void CDC_EP0_OUT(void) {
  uint8_t i, len;
  if(USB_SetupReq == SET_LINE_CODING) {
    len = USB_RX_len;
    if(len > sizeof(CDC_lineCoding)) len = sizeof(CDC_lineCoding);
    for(i=0; i<len; i++) ((uint8_t*)&CDC_lineCoding)[i] = EP0_buffer[i];
    CDC_configure = 1;
  }
}

// Endpoint 5 IN handler (data sent to host)
void CDC_EP5_IN(void) {
  CDC_txBusy = 0;
}

// Endpoint 5 OUT handler (data received, endpoint stays at NAK until processed)
void CDC_EP5_OUT(void) {
  if(USB_RX_len) CDC_rxLen = USB_RX_len;
  else USB_EP_receive(5);                 // ZLP -> receive next packet
}

#else
void CDC_task(void) {}
#endif // DAP_UART_USB_COM_PORT
