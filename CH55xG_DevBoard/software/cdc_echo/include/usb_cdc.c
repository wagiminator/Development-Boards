// ===================================================================================
// USB CDC Functions for CH551, CH552 and CH554
// ===================================================================================

#include "ch554.h"
#include "usb.h"
#include "usb_cdc.h"
#include "usb_descr.h"
#include "usb_handler.h"
#include "delay.h"

// Initialize line coding
__xdata CDC_LINE_CODING_TYPE CDC_LineCoding = {
  .baudrate = 57600,        // baudrate 57600
  .stopbits = 0,            // 1 stopbit
  .parity   = 0,            // no parity
  .databits = 8             // 8 databits
};

volatile __xdata uint8_t CDC_controlLineState  = 0; // control line state
volatile __xdata uint8_t USB_EP2_readByteCount = 0; // bytes of received data on USB endpoint
volatile __xdata uint8_t USB_EP2_readPointer   = 0; // data pointer for fetching
volatile __bit USB_EP2_writeBusyFlag = 0;           // flag of whether upload pointer is busy
__xdata uint8_t USB_EP2_writePointer = 0;           // data pointer for writing

#define CDC_DTR_flag  (CDC_controlLineState & 1)
#define CDC_RTS_flag  ((CDC_controlLineState >> 1) & 1)

// Setup USB-CDC
void CDC_init(void) {
  USB_init();
  UEP0_T_LEN = 0;
  UEP1_T_LEN = 0;
  UEP2_T_LEN = 0;
}

// Check number of bytes in the IN buffer
uint8_t CDC_available(void) {
  return USB_EP2_readByteCount;
}

// Check if OUT buffer is ready to be written
__bit CDC_ready(void) {
  return(CDC_DTR_flag && !USB_EP2_writeBusyFlag);
}

// Flush the OUT buffer
void CDC_flush(void) {
  if(!USB_EP2_writeBusyFlag && USB_EP2_writePointer > 0) {
    UEP2_T_LEN = USB_EP2_writePointer;                                                   
    UEP2_CTRL  = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_ACK; // respond ACK
    USB_EP2_writeBusyFlag = 1;
    USB_EP2_writePointer  = 0;
  }
}

// Write single character to OUT buffer
void CDC_write(char c) {
  while(!CDC_ready());
  Ep2Buffer[EP2_SIZE + USB_EP2_writePointer++] = c;
  if(USB_EP2_writePointer == EP2_SIZE) CDC_flush();
}

// Write string to OUT buffer
void CDC_print(char* str) {
  while(*str) CDC_write(*str++);
}

// Write string with newline to OUT buffer and flush
void CDC_println(char* str) {
  CDC_print(str);
  CDC_write('\n');
  CDC_flush();
}

// Read single character from IN buffer
char CDC_read(void) {
  char data;
  while(!CDC_available());
  data = Ep2Buffer[USB_EP2_readPointer++];
  if(--USB_EP2_readByteCount == 0)
    UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_ACK;
  return data;
}

// Get DTR flag
__bit CDC_getDTR(void) {
  return CDC_DTR_flag;
}

// Get RTS flag
__bit CDC_getRTS(void) {
  return CDC_RTS_flag;
}

// Reset CDC parameters
void CDC_reset(void) {
  USB_EP2_readByteCount = 0;                          // bytes of received data on USB endpoint
  USB_EP2_writeBusyFlag = 0;
}

// Set line coding handler
void CDC_setLineCoding(void) {
  uint8_t i;
  for(i=0; i<((sizeof(CDC_LineCoding)<=USB_RX_LEN)?sizeof(CDC_LineCoding):USB_RX_LEN); i++) {
    ((uint8_t*)&CDC_LineCoding)[i] = Ep0Buffer[i];
  }
}

// Get line coding handler
uint8_t CDC_getLineCoding(void) {
  uint8_t i;
  for(i=0; i<sizeof(CDC_LineCoding); i++) {
    Ep0Buffer[i] = ((uint8_t*)&CDC_LineCoding)[i];
  }
  return sizeof(CDC_LineCoding);
}

// Set control line state handler
void CDC_setControlLineState(void) {
  CDC_controlLineState = Ep0Buffer[2];
}

void USB_EP2_IN(void) {
  UEP2_T_LEN = 0;                                           // no data to send anymore
  UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_T_RES | UEP_T_RES_NAK;  // respond NAK by default
  USB_EP2_writeBusyFlag = 0;                                // clear busy flag
}

void USB_EP2_OUT(void) {
  if(U_TOG_OK) {                                            // discard unsynchronized packets
    USB_EP2_readByteCount = USB_RX_LEN;
    USB_EP2_readPointer = 0;                                // reset Data pointer for fetching
    if(USB_EP2_readByteCount) 
      UEP2_CTRL = UEP2_CTRL & ~MASK_UEP_R_RES | UEP_R_RES_NAK; // respond NAK after a packet. Let main code change response after handling.
  }
}
