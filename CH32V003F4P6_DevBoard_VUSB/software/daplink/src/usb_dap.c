// ===================================================================================
// USB HID Transport for CMSIS-DAP v1 on CH32V003                             * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_dap.h"

// HID request types (bmRequestType | bRequest << 8)
#define HID_REQ_SET_REPORT    0x0921      // class request to interface, OUT

// Receive states
#define RX_IDLE               0           // waiting for request
#define RX_RECEIVING          1           // receiving output report
#define RX_COMPLETE           2           // request complete, execution pending

// Receive buffer (HID output report, written by USB interrupt)
static uint8_t  HID_rx_buf[HID_REPORT_SIZE] __attribute__((aligned(4)));
static volatile uint8_t HID_rx_state = RX_IDLE;
static volatile uint8_t HID_rx_len;
static uint8_t  HID_rx_wLength;

// Transmit buffer (HID input report, read by USB interrupt)
static uint8_t  HID_tx_buf[HID_REPORT_SIZE] __attribute__((aligned(4)));
static volatile uint8_t HID_tx_busy;      // 1: response is being sent
static uint8_t  HID_tx_len;               // number of bytes to send
static uint8_t  HID_tx_pkts;              // number of packets to send (incl. ZLP)

// DAP request buffer (copy of the received report)
static uint8_t  DAP_request[HID_REPORT_SIZE] __attribute__((aligned(4)));

// Endpoint 1 state of the USB stack
#define EP1   (&rv003usb_internal_data.eps[1])

// Compiler memory barrier: the buffers are shared with the USB interrupt
#define MEM_BARRIER()   __asm__ volatile("" ::: "memory")

// ===================================================================================
// DAP Request Handling (main loop)
// ===================================================================================

// Handle pending DAP requests (call in main loop)
void DAP_USB_poll(void) {
  uint8_t i, len;

  // Previous response completely sent?
  if(HID_tx_busy) {
    if(*(volatile __typeof__(EP1->count)*)&EP1->count < HID_tx_pkts) return;  // all packets ACKed?
    HID_tx_busy = 0;
    MEM_BARRIER();
  }

  // New request received?
  if(HID_rx_state != RX_COMPLETE) return;
  MEM_BARRIER();                          // read receive buffer after state check
  len = HID_rx_len;
  for(i=0; i<HID_REPORT_SIZE; i++) DAP_request[i] = (i < len) ? HID_rx_buf[i] : 0;
  MEM_BARRIER();                          // copy complete before releasing buffer
  HID_rx_state = RX_IDLE;                 // receive buffer is free again

  // Process request
  for(i=0; i<HID_REPORT_SIZE; i++) HID_tx_buf[i] = 0;
  len = DAP_process(DAP_request, HID_tx_buf);

  // Send response
  #if DAP_SHORT_REPORTS > 0
  HID_tx_len  = len;                      // only the used part
  HID_tx_pkts = ((len & 7) || (len >= HID_REPORT_SIZE)) ? ((len + 7) >> 3) : ((len >> 3) + 1);
  #else
  (void)len;
  HID_tx_len  = HID_REPORT_SIZE;          // complete report
  HID_tx_pkts = HID_REPORT_SIZE / 8;
  #endif
  EP1->count  = 0;
  MEM_BARRIER();                          // response written before releasing it
  HID_tx_busy = 1;
}

// Init DAP and USB
void DAP_USB_init(void) {
  DAP_init();
  usb_setup();
}

// ===================================================================================
// USB Handler Functions (called from USB interrupt)
// ===================================================================================

// Setup packet of a control transfer which is not handled by the USB stack
void usb_handle_other_control_message(struct usb_endpoint *e, struct usb_urb *s, struct rv003usb_internal *ist) {
  if(s->wRequestTypeLSBRequestMSB == HID_REQ_SET_REPORT) {
    // HID output report (DAP request) follows in the data stage
    HID_rx_wLength = (s->wLength < HID_REPORT_SIZE) ? s->wLength : HID_REPORT_SIZE;
    HID_rx_len     = 0;
    HID_rx_state   = HID_rx_wLength ? RX_RECEIVING : RX_IDLE;
  }
  // All other requests (e.g. SET_IDLE) get an empty reply
}

// Data stage of a control write: collect DAP request
void usb_handle_user_data(struct usb_endpoint *e, int current_endpoint, uint8_t *data, int len, struct rv003usb_internal *ist) {
  uint8_t pos;
  if(current_endpoint || (HID_rx_state != RX_RECEIVING)) return;
  pos = HID_rx_len;
  while((len-- > 0) && (pos < HID_rx_wLength)) HID_rx_buf[pos++] = *data++;
  HID_rx_len = pos;
  if(pos >= HID_rx_wLength) {
    if(HID_rx_buf[0] == ID_DAP_TransferAbort) {
      DAP_abort    = 1;                   // abort current transfer immediately
      HID_rx_state = RX_IDLE;             // no response for this command
    }
    else HID_rx_state = RX_COMPLETE;      // execute request in main loop
  }
}

// IN request on endpoint 1: send response, or NAK if there is none
void usb_handle_user_in_request(struct usb_endpoint *e, uint8_t *scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal *ist) {
  int offset, len;
  if((endp == 1) && HID_tx_busy && (e->count < HID_tx_pkts)) {
    offset = e->count << 3;
    len = (int)HID_tx_len - offset;
    if(len > 8) len = 8;
    if(len > 0) usb_send_data(HID_tx_buf + offset, len, 0, sendtok);
    else usb_send_empty(sendtok);         // zero-length packet terminates short report
    return;
  }
  usb_send_data(0, 0, 2, 0x5A);           // send NAK
}
