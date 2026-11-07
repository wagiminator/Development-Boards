// ===================================================================================
// USB HID Functions for CH32V203 (USBD)                                      * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_hid.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================
volatile uint8_t HID_writeBusyFlag;               // upload pointer busy flag
volatile uint8_t HID_status;                      // status byte returned from host
static   uint8_t HID_idle;                        // idle rate (not used)
static   uint8_t HID_protocol = 1;                // protocol (0: boot, 1: report)

// HID class requests
#define HID_GET_REPORT        0x01
#define HID_GET_IDLE          0x02
#define HID_GET_PROTOCOL      0x03
#define HID_SET_REPORT        0x09
#define HID_SET_IDLE          0x0A
#define HID_SET_PROTOCOL      0x0B

// ===================================================================================
// Front End Functions
// ===================================================================================

// Send HID report
void HID_sendReport(uint8_t* buf, uint8_t len) {
  uint8_t i;
  if(!USB_ENUM_OK) return;                        // not configured by host yet
  while(HID_writeBusyFlag);                       // wait for ready to write
  if(len > EP1_SIZE) len = EP1_SIZE;
  for(i=0; i<len; i++) EP1_buffer[i] = buf[i];    // copy report to EP1 buffer
  HID_writeBusyFlag = 1;                          // set busy flag
  USB_EP_send(1, len);                            // upload report to host
}

// ===================================================================================
// HID-Specific USB Handler Functions
// ===================================================================================

// Setup/reset HID endpoints
void HID_EP_init(void) {
  USB_EP_init(1, USB_ENDP_TYPE_INTER, EP1_buffer, 0, 1);  // EP1: report IN
  #ifdef EP2_SIZE
  USB_EP_init(2, USB_ENDP_TYPE_INTER, EP2_buffer, 1, 0);  // EP2: report OUT
  #endif
  HID_writeBusyFlag = 0;                          // reset write busy flag
}

// Handle HID class setup requests
uint8_t HID_control(void) {
  switch(USB_SetupReq) {
    case HID_SET_IDLE:
      HID_idle = USB_SetupBuf->wValueH;
      return 0;
    case HID_GET_IDLE:
      EP0_buffer[0] = HID_idle;
      if(USB_SetupLen > 1) USB_SetupLen = 1;
      return USB_SetupLen;
    case HID_SET_PROTOCOL:
      HID_protocol = USB_SetupBuf->wValueL;
      return 0;
    case HID_GET_PROTOCOL:
      EP0_buffer[0] = HID_protocol;
      if(USB_SetupLen > 1) USB_SetupLen = 1;
      return USB_SetupLen;
    case HID_SET_REPORT:                          // output report via control pipe
      return 0;                                   // data follows in OUT stage
    default:
      return 0xff;                                // not supported
  }
}

// Endpoint 0 CLASS OUT handler (SET_REPORT data stage, e.g. keyboard LEDs)
void HID_EP0_OUT(void) {
  if(USB_SetupReq != HID_SET_REPORT) return;
  #ifdef HID_LED_REPORT_ID
  if((USB_RX_len >= 2) && (EP0_buffer[0] == HID_LED_REPORT_ID))
    HID_status = EP0_buffer[1];                   // save status byte (with report ID)
  #else
  if(USB_RX_len) HID_status = EP0_buffer[0];      // save status byte
  #endif
}

// Endpoint 1 IN handler (HID report transfer to host completed)
void HID_EP1_IN(void) {
  HID_writeBusyFlag = 0;                          // clear busy flag
}

// Endpoint 2 OUT handler (HID status transfer from host completed)
#ifdef EP2_SIZE
void HID_EP2_OUT(void) {
  #ifdef HID_LED_REPORT_ID
  if((USB_RX_len >= 2) && (EP2_buffer[0] == HID_LED_REPORT_ID))
    HID_status = EP2_buffer[1];                   // save status byte (with report ID)
  #else
  if(USB_RX_len) HID_status = EP2_buffer[0];      // save status byte
  #endif
  USB_EP_receive(2);                              // ready for next report
}
#endif
