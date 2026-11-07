// ===================================================================================
// Basic USB CDC Functions for STC8H8K64U                         * v1.5 *
// ===================================================================================

#include "usb_cdc.h"

// ===================================================================================
// Variables and Defines
// ===================================================================================

// Initialize line coding
__xdata CDC_LINE_CODING_TYPE CDC_lineCoding = {
  .baudrate = 115200,       // baudrate 115200
  .stopbits = 0,            // 1 stopbit
  .parity   = 0,            // no parity
  .databits = 8             // 8 databits
};

// Variables
volatile __xdata uint8_t CDC_controlLineState = 0;  // control line state
volatile __xdata uint8_t CDC_readByteCount = 0;     // number of data bytes in IN buffer
volatile __xdata uint8_t CDC_readPointer   = 0;     // data pointer for fetching
volatile __xdata uint8_t CDC_writePointer  = 0;     // data pointer for writing
volatile __bit CDC_writeBusyFlag = 0;               // flag of whether upload pointer is busy

// CDC class requests
#define SET_LINE_CODING         0x20  // host configures line coding
#define GET_LINE_CODING         0x21  // host reads configured line coding
#define SET_CONTROL_LINE_STATE  0x22  // generates RS-232/V.24 style control signals
#define SEND_BREAK              0x23  // send break

// Endpoint buffers (EP2: OUT at offset 0, IN at offset 64)
#define CDC_OUT_buffer          (EP2_buffer)
#define CDC_IN_buffer           (EP2_buffer + 64)

// ===================================================================================
// Front End Functions
// ===================================================================================

// Flush the OUT buffer (upload to host)
void CDC_flush(void) {
  if(!CDC_writeBusyFlag && CDC_writePointer) {    // not busy and buffer not empty?
    CDC_writeBusyFlag = 1;                        // busy for now
    USB_EP_send(2, CDC_writePointer);             // upload data to host
    CDC_writePointer = 0;                         // reset write pointer
  }
}

// Write single character to OUT buffer
void CDC_write(char c) {
  while(CDC_writeBusyFlag);                       // wait for ready to write
  CDC_IN_buffer[CDC_writePointer++] = c;          // write character to buffer
  if(CDC_writePointer == EP2_SIZE) CDC_flush();   // flush if buffer full
}

// Write string to OUT buffer
void CDC_print(char* str) {
  while(*str) CDC_write(*str++);                  // write each char of string
}

// Write string with newline to OUT buffer and flush
void CDC_println(char* str) {
  CDC_print(str);                                 // write string
  CDC_write('\n');                                // write new line
  CDC_flush();                                    // flush OUT buffer
}

// Read single character from IN buffer
char CDC_read(void) {
  char data;
  while(!CDC_readByteCount);                      // wait for data
  data = CDC_OUT_buffer[CDC_readPointer++];       // get character
  if(--CDC_readByteCount == 0) USB_EP_receive(2); // request new data if empty
  return data;
}

// ===================================================================================
// CDC-Specific USB Handler Functions
// ===================================================================================
#pragma save
#pragma nooverlay                                 // functions are called by USB interrupt

// Setup/reset CDC endpoints
void CDC_EP_init(void) {
  USB_EP_init(1, EP1_buffer, 0, 1);               // EP1: notification IN
  USB_EP_init(2, EP2_buffer, 1, 1);               // EP2: data OUT and IN
  CDC_readByteCount = 0;                          // reset received bytes counter
  CDC_writePointer  = 0;                          // reset write pointer
  CDC_writeBusyFlag = 0;                          // reset write busy flag
}

// Handle CLASS SETUP requests
uint8_t CDC_control(void) {
  uint8_t i;
  switch(USB_SetupReq) {
    case GET_LINE_CODING:                         // 0x21  currently configured
      for(i=0; i<sizeof(CDC_lineCoding); i++)
        EP0_buffer[i] = ((uint8_t*)&CDC_lineCoding)[i]; // transmit line coding to host
      if(USB_SetupLen > sizeof(CDC_lineCoding)) USB_SetupLen = sizeof(CDC_lineCoding);
      return USB_SetupLen;
    case SET_CONTROL_LINE_STATE:                  // 0x22  generates RS-232/V.24 style control signals
      CDC_controlLineState = EP0_buffer[2];       // read control line state
      return 0;
    case SET_LINE_CODING:                         // 0x20  configure (data follows in OUT stage)
    case SEND_BREAK:                              // 0x23  ignore break
      return 0;
    default:
      return 0xff;                                // command not supported
  }
}

// Endpoint 0 CLASS OUT handler (data stage)
void CDC_EP0_OUT(void) {
  uint8_t i, len;
  if(USB_SetupReq == SET_LINE_CODING) {           // set line coding
    len = USB_RX_len;
    if(len > sizeof(CDC_lineCoding)) len = sizeof(CDC_lineCoding);
    for(i=0; i<len; i++)
      ((uint8_t*)&CDC_lineCoding)[i] = EP0_buffer[i];  // receive line coding from host
  }
}

// Endpoint 1 IN handler
// No handling is actually necessary here, nothing is sent via EP1.

// Endpoint 2 IN handler (bulk data transfer to host completed)
void CDC_EP2_IN(void) {
  CDC_writeBusyFlag = 0;                          // clear busy flag
}

// Endpoint 2 OUT handler (bulk data transfer from host completed)
void CDC_EP2_OUT(void) {
  if(USB_RX_len) {                                // received data? (endpoint stays at NAK)
    CDC_readPointer   = 0;                        // reset read pointer for fetching
    CDC_readByteCount = USB_RX_len;               // set number of received data bytes
  }
  else USB_EP_receive(2);                         // ZLP -> ready for next packet
}

#pragma restore
