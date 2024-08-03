// ===================================================================================
// USB HID Functions for CH32X035/X034/X033                                   * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_hid.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================
volatile uint8_t HID_writeBusyFlag;               // upload pointer busy flag

#ifdef EP2_SIZE
volatile uint8_t HID_status;                      // status byte returned from host
#endif

// ===================================================================================
// Front End Functions
// ===================================================================================

// Send HID report
void HID_sendReport(uint8_t* buf, uint8_t len) {
  uint8_t i;
  while(HID_writeBusyFlag);                       // wait for ready to write
  for(i=0; i<len; i++) EP1_buffer[i] = buf[i];    // copy report to EP1 buffer
  HID_writeBusyFlag   = 1;                        // set busy flag
  USBFSD->UEP1_TX_LEN = len;                      // set length and upload report to host
  USBFSD->UEP1_CTRL_H = (USBFSD->UEP1_CTRL_H & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;
}

// ===================================================================================
// HID-Specific USB Handler Functions
// ===================================================================================

// Setup/reset HID endpoints
void HID_EP_init(void) {
  USBFSD->UEP1_DMA    = (uint32_t)EP1_buffer;     // EP1 data transfer buffer address
  USBFSD->UEP4_1_MOD  = USBFS_UEP1_TX_EN;         // EP1 TX enable
  USBFSD->UEP1_CTRL_H = USBFS_UEP_AUTO_TOG        // EP1 Auto flip sync flag
                      | USBFS_UEP_T_RES_NAK;      // EP1 IN transaction returns NAK
  USBFSD->UEP1_TX_LEN = 0;                        // Nothing to send

  #ifdef EP2_SIZE
  USBFSD->UEP2_DMA    = (uint32_t)EP2_buffer;     // EP2 data transfer buffer address
  USBFSD->UEP2_3_MOD  = USBFS_UEP2_RX_EN;         // EP2 RX enable
  USBFSD->UEP2_CTRL_H = USBFS_UEP_AUTO_TOG        // EP2 Auto flip sync flag
                      | USBFS_UEP_R_RES_ACK;      // EP2 OUT transaction returns ACK
  #endif

  HID_writeBusyFlag   = 0;                        // reset write busy flag
}

// Endpoint 1 IN handler (HID report transfer to host completed)
void HID_EP1_IN(void) {
  USBFSD->UEP1_CTRL_H = (USBFSD->UEP1_CTRL_H & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NAK;
  HID_writeBusyFlag = 0;                          // clear busy flag
}

// Endpoint 2 OUT handler (HID status transfer from host completed)
#ifdef EP2_SIZE
void HID_EP2_OUT(void) {
  HID_status = EP2_buffer[0];                     // save status byte
}
#endif
