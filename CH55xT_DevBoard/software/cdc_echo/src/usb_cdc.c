// ===================================================================================
// Basic USB CDC Functions for CH551, CH552 and CH554                         * v1.5 *
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

// ===================================================================================
// Front End Functions
// ===================================================================================

// Flush the OUT buffer (upload to host)
void CDC_flush(void) {
  if(!CDC_writeBusyFlag && CDC_writePointer) {    // not busy and buffer not empty?
    CDC_writeBusyFlag = 1;                        // busy for now
    UEP2_T_LEN = CDC_writePointer;                // number of bytes to upload
    UEP2_CTRL  = (UEP2_CTRL & ~MASK_UEP_T_RES)
               | UEP_T_RES_ACK;                   // upload data to host
    CDC_writePointer = 0;                         // reset write pointer
  }
}

// Write single character to OUT buffer
void CDC_write(char c) {
  while(CDC_writeBusyFlag);                       // wait for ready to write
  EP2_buffer[64 + CDC_writePointer++] = c;        // write character to buffer
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
  data = EP2_buffer[CDC_readPointer++];           // get character
  if(--CDC_readByteCount == 0)                    // dec number of bytes in buffer
    UEP2_CTRL = (UEP2_CTRL & ~MASK_UEP_R_RES)
              | UEP_R_RES_ACK;                    // request new data if empty
  return data;
}

// ===================================================================================
// CDC-Specific USB Handler Functions
// ===================================================================================

// Setup/reset CDC endpoints
void CDC_EP_init(void) {
  UEP1_DMA    = (uint16_t)EP1_buffer;             // EP1 data transfer address
  UEP2_DMA    = (uint16_t)EP2_buffer;             // EP2 data transfer address
  UEP1_CTRL   = bUEP_AUTO_TOG                     // EP1 Auto flip sync flag
              | UEP_T_RES_NAK;                    // EP1 IN transaction returns NAK
  UEP2_CTRL   = bUEP_AUTO_TOG                     // EP2 Auto flip sync flag
              | UEP_T_RES_NAK                     // EP2 IN transaction returns NAK
              | UEP_R_RES_ACK;                    // EP2 OUT transaction returns ACK
  UEP2_3_MOD  = bUEP2_RX_EN | bUEP2_TX_EN;        // EP2 double buffer (0x0C)
  UEP4_1_MOD  = bUEP1_TX_EN;                      // EP1 TX enable (0x40)
  UEP1_T_LEN  = 0;                                // EP1 nothing to send
  UEP2_T_LEN  = 0;                                // EP2 nothing to send
  CDC_readByteCount = 0;                          // reset received bytes counter
  CDC_writeBusyFlag = 0;                          // reset write busy flag
}

// Handle CLASS SETUP requests
uint8_t CDC_control(void) {
  uint8_t i;
  switch(USB_SetupReq) {
    case GET_LINE_CODING:                         // 0x21  currently configured
      for(i=0; i<sizeof(CDC_lineCoding); i++)
        EP0_buffer[i] = ((uint8_t*)&CDC_lineCoding)[i]; // transmit line coding to host
      return sizeof(CDC_lineCoding);
    case SET_CONTROL_LINE_STATE:                  // 0x22  generates RS-232/V.24 style control signals
      CDC_controlLineState = EP0_buffer[2];       // read control line state
      return 0;
    case SET_LINE_CODING:                         // 0x20  Configure
      return 0;            
    default:
      return 0xff;                                // command not supported
  }
}

// Endpoint 0 CLASS OUT handler
void CDC_EP0_OUT(void) {
  uint8_t i;
  if(USB_SetupReq == SET_LINE_CODING) {           // set line coding
    for(i=0; i<((sizeof(CDC_lineCoding)<=USB_RX_LEN)?sizeof(CDC_lineCoding):USB_RX_LEN); i++)
      ((uint8_t*)&CDC_lineCoding)[i] = EP0_buffer[i];      // receive line coding from host
  }
  UEP0_CTRL = bUEP_T_TOG | UEP_T_RES_ACK | UEP_R_RES_ACK;
}

// Endpoint 1 IN handler
// No handling is actually necessary here, the auto-NAK is sufficient.

// Endpoint 2 IN handler (bulk data transfer to host completed)
void CDC_EP2_IN(void) {
  UEP2_CTRL  = (UEP2_CTRL & ~MASK_UEP_T_RES)
             | UEP_T_RES_NAK;                     // -> respond NAK for now
  CDC_writeBusyFlag = 0;                          // clear busy flag
}

// Endpoint 2 OUT handler (bulk data transfer from host completed)
void CDC_EP2_OUT(void) {
  if(U_TOG_OK && USB_RX_LEN) {                    // received synchronized packet?
    UEP2_CTRL = (UEP2_CTRL & ~MASK_UEP_R_RES)
              | UEP_R_RES_NAK;                    // not ready to receive more for now
    CDC_readByteCount = USB_RX_LEN;               // set number of received data bytes
    CDC_readPointer   = 0;                        // reset read pointer for fetching
  }
}
