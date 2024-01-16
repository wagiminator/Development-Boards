// ===================================================================================
// Basic USB CDC Functions for CH32X035/X034/X033                             * v1.0 *
// ===================================================================================

#include "usb_cdc.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================

// Initialize line coding
CDC_LINE_CODING_TYPE CDC_lineCoding = {
  .baudrate = 115200,       // baudrate 115200
  .stopbits = 0,            // 1 stopbit
  .parity   = 0,            // no parity
  .databits = 8             // 8 databits
};

// Variables
volatile uint8_t CDC_controlLineState = 0;  // control line state
volatile uint8_t CDC_readByteCount = 0;     // number of data bytes in IN buffer
volatile uint8_t CDC_readPointer   = 0;     // data pointer for fetching
volatile uint8_t CDC_writePointer  = 0;     // data pointer for writing
volatile uint8_t CDC_writeBusyFlag = 0;     // flag of whether upload pointer is busy

// CDC class requests
#define SET_LINE_CODING           0x20      // host configures line coding
#define GET_LINE_CODING           0x21      // host reads configured line coding
#define SET_CONTROL_LINE_STATE    0x22      // generates RS-232/V.24 style control signals

// ===================================================================================
// Front End Functions
// ===================================================================================

// Setup USB-CDC
void CDC_init(void) {
  USB_init();
}

// Check number of bytes in the IN buffer
uint8_t CDC_available(void) {
  return CDC_readByteCount;
}

// Check if OUT buffer is ready to be written
uint8_t CDC_ready(void) {
  return(!CDC_writeBusyFlag);
}

// Flush the OUT buffer
void CDC_flush(void) {
  if(!CDC_writeBusyFlag && CDC_writePointer > 0) {      // not busy and buffer not empty?
    USBFSD->UEP2_TX_LEN = CDC_writePointer;             // number of bytes in OUT buffer
    USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;
    CDC_writeBusyFlag = 1;                              // busy for now
    CDC_writePointer  = 0;                              // reset write pointer
  }
}

// Write single character to OUT buffer
void CDC_write(char c) {
  while(CDC_writeBusyFlag);                             // wait for ready to write
  EP2_buffer[64 + CDC_writePointer++] = c;              // write character
  if(CDC_writePointer == EP2_SIZE) CDC_flush();         // flush if buffer full
}

// Read single character from IN buffer
char CDC_read(void) {
  char data;
  while(!CDC_readByteCount);                            // wait for data
  data = EP2_buffer[CDC_readPointer++];                 // get character
  if(--CDC_readByteCount == 0)                          // dec number of bytes in buffer
    USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_ACK;
  return data;
}

// ===================================================================================
// CDC-Specific USB Handler Functions
// ===================================================================================

// Setup CDC endpoints
void CDC_EP_init(void) {
  USBFSD->UEP1_DMA    = (uint32_t)EP1_buffer;   // EP1 data transfer buffer address
  USBFSD->UEP2_DMA    = (uint32_t)EP2_buffer;   // EP2 data transfer buffer address
  USBFSD->UEP4_1_MOD  = USBFS_UEP1_TX_EN;       // EP1 TX enable
  USBFSD->UEP2_3_MOD  = USBFS_UEP2_RX_EN        // EP2 RX enable
                      | USBFS_UEP2_TX_EN;       // EP2 TX enable
  USBFSD->UEP1_CTRL_H = USBFS_UEP_AUTO_TOG      // EP1 Auto flip sync flag
                      | USBFS_UEP_T_RES_NAK;    // EP1 IN transaction returns NAK
  USBFSD->UEP2_CTRL_H = USBFS_UEP_AUTO_TOG      // EP2 Auto flip sync flag
                      | USBFS_UEP_R_RES_ACK     // EP2 OUT transaction returns ACK
                      | USBFS_UEP_T_RES_NAK;    // EP2 IN transaction returns NAK
  USBFSD->UEP1_TX_LEN = 0;                      // Nothing to send
  USBFSD->UEP2_TX_LEN = 0;                      // Nothing to send

  CDC_readByteCount   = 0;                      // reset received bytes counter
  CDC_writeBusyFlag   = 0;                      // reset write busy flag
}

// Handle class setup requests
uint8_t CDC_control(void) {
  uint8_t i;
  switch(USB_SetupReq) {
    case GET_LINE_CODING:                   // 0x21  currently configured 
      for(i=0; i<sizeof(CDC_lineCoding); i++)
        EP0_buffer[i] = ((uint8_t*)&CDC_lineCoding)[i]; // transmit line coding to host
      if(USB_SetupLen > sizeof(CDC_lineCoding)) USB_SetupLen = sizeof(CDC_lineCoding);
      return USB_SetupLen;
    case SET_CONTROL_LINE_STATE:            // 0x22  generates RS-232/V.24 style control signals
      CDC_controlLineState = EP0_buffer[2]; // read control line state
      return 0;
    case SET_LINE_CODING:                   // 0x20  Configure
      return 0;            
    default:
      return 0xff;                          // command not supported
  }
}

// Endpoint 0 CLASS OUT handler
void CDC_EP0_OUT(void) {
  uint8_t i, len;
  if(USB_SetupReq == SET_LINE_CODING) {                 // set line coding
    len = USBFSD->RX_LEN;
    for(i=0; i<((sizeof(CDC_lineCoding)<=len)?sizeof(CDC_lineCoding):len); i++)
      ((uint8_t*)&CDC_lineCoding)[i] = EP0_buffer[i];   // receive line coding from host
    USB_SetupLen = 0;
  }
  USBFSD->UEP0_CTRL_H = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK | USBFS_UEP_R_RES_ACK;
}

// Endpoint 1 IN handler
// No handling is actually necessary here, the auto-NAK is sufficient.

// Endpoint 2 IN handler (bulk data transfer to host)
void CDC_EP2_IN(void) {
  CDC_writeBusyFlag = 0;                                // clear busy flag
  USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_NAK;
}

// Endpoint 2 OUT handler (bulk data transfer from host)
void CDC_EP2_OUT(void) {
  if((USBFSD->INT_FG & USBFS_U_TOG_OK) && USBFSD->RX_LEN) {
    // respond NAK after a packet. Let main code change response after handling.
    USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_NAK;
    CDC_readByteCount   = USBFSD->RX_LEN;             // set number of received data bytes
    CDC_readPointer     = 0;                          // reset read pointer for fetching
  }
}
