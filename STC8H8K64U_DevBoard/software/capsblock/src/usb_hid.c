// ===================================================================================
// USB HID Functions for STC8H8K64U                                           * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_hid.h"
#pragma nooverlay                                 // functions are called by USB interrupt

// ===================================================================================
// Variables and Defines
// ===================================================================================
volatile __bit HID_writeBusyFlag;                 // upload busy flag
volatile uint8_t HID_idleRate = 0;                // idle rate (4ms units, 0: infinite)
volatile uint8_t HID_protocol = 1;                // protocol (0: boot, 1: report)

// ===================================================================================
// Front End Functions
// ===================================================================================

// Send HID report
void HID_sendReport(__xdata uint8_t* buf, uint8_t len) {
  uint8_t i;
  if(len > EP1_SIZE) len = EP1_SIZE;              // limit length
  while(HID_writeBusyFlag);                       // wait for ready to write
  for(i=0; i<len; i++) EP1_buffer[i] = buf[i];    // copy report to EP1 buffer
  HID_writeBusyFlag = 1;                          // set busy flag
  USB_EP_send(1, len);                            // upload report to host
}

// ===================================================================================
// HID-Specific USB Handler Functions
// ===================================================================================

// Setup/reset HID endpoints
void HID_EP_init(void) {
  USB_EP_init(1, EP1_buffer, 0, 1);               // EP1: report IN
  #ifdef EP2_SIZE
  USB_EP_init(2, EP2_buffer, 1, 0);               // EP2: report OUT
  #endif
  HID_writeBusyFlag = 0;                          // reset write busy flag
}

// Handle HID class SETUP requests
uint8_t HID_control(void) {
  switch(USB_SetupReq) {
    case HID_SET_IDLE:
      HID_idleRate = USB_SetupBuf->wValueH;
      return 0;
    case HID_GET_IDLE:
      EP0_buffer[0] = HID_idleRate;
      if(USB_SetupLen > 1) USB_SetupLen = 1;
      return USB_SetupLen;
    case HID_SET_PROTOCOL:
      HID_protocol = USB_SetupBuf->wValueL;
      return 0;
    case HID_GET_PROTOCOL:
      EP0_buffer[0] = HID_protocol;
      if(USB_SetupLen > 1) USB_SetupLen = 1;
      return USB_SetupLen;
    case HID_SET_REPORT:                          // accept (data stage is ignored)
      return 0;
    default:
      return 0xff;                                // not supported -> STALL
  }
}

// Endpoint 1 IN handler (HID report transfer to host completed)
void HID_EP1_IN(void) {
  HID_writeBusyFlag = 0;                          // clear busy flag
}

#ifdef EP2_SIZE
// Endpoint 2 OUT handler (HID report transfer from host completed)
// The current report can be read from the EP2 buffer (HID_IN_buffer).
void HID_EP2_OUT(void) {
  USB_EP_receive(2);                              // ready for next report
}
#endif
