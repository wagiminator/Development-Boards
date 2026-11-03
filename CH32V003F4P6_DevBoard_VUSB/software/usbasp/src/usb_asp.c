// ===================================================================================
// USBasp Functions for CH32V003                                              * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_asp.h"

// Request states
#define ASP_IDLE        0               // nothing to do
#define ASP_SETUP       1               // setup received, execution pending
#define ASP_RECEIVE     2               // receiving data stage of a control write
#define ASP_WRITE       3               // data stage complete, execution pending
#define ASP_BUSY        4               // request is being executed
#define ASP_READY       5               // request executed, reply is ready

// Request handling (shared between USB interrupt and main loop)
static volatile uint8_t  ASP_state = ASP_IDLE;  // request state
static volatile uint8_t  ASP_seq;               // incremented on every new request
static volatile uint16_t ASP_len;               // received bytes / reply length
static uint16_t ASP_wLength;                    // length of the data stage
static uint8_t  ASP_setup[8];                   // copy of the setup packet
static uint8_t  ASP_buffer[ASP_BUF_SIZE] __attribute__((aligned(4)));

// Programming state
static uint8_t  prog_address_newmode = 0;
static uint8_t  prog_blockflags;
static uint16_t prog_pagesize;
static uint16_t prog_pagecounter;
static uint32_t prog_address;

// ===================================================================================
// USBasp Request Execution (called from main loop)
// ===================================================================================

// Get 16-bit address from setup packet
#define ASP_ADDR16()    ((uint16_t)ASP_setup[2] | ((uint16_t)ASP_setup[3] << 8))

// Execute control write (data stage complete, data in buffer)
static void ASP_handleWrite(void) {
  uint16_t i;
  uint16_t n = ASP_len;
  if(n > ASP_wLength) n = ASP_wLength;

  switch(ASP_setup[1]) {

    case USBASP_FUNC_WRITEFLASH:
      if(!prog_address_newmode) prog_address = ASP_ADDR16();
      prog_pagesize   = ASP_setup[4] | (((uint16_t)ASP_setup[5] & 0xF0) << 4);
      prog_blockflags = ASP_setup[5] & 0x0F;
      if(prog_blockflags & PROG_BLOCKFLAG_FIRST) prog_pagecounter = prog_pagesize;
      for(i=0; i<n; i++) {
        if(prog_pagesize == 0) ISP_writeFlash(prog_address, ASP_buffer[i], 1);
        else {
          ISP_writeFlash(prog_address, ASP_buffer[i], 0);
          if(--prog_pagecounter == 0) {
            ISP_flushPage(prog_address, ASP_buffer[i]);
            prog_pagecounter = prog_pagesize;
          }
        }
        prog_address++;
      }
      // Last block and page flush pending? -> flush it now
      if(n && (prog_blockflags & PROG_BLOCKFLAG_LAST) && (prog_pagecounter != prog_pagesize))
        ISP_flushPage(prog_address - 1, ASP_buffer[n - 1]);
      break;

    case USBASP_FUNC_WRITEEEPROM:
      if(!prog_address_newmode) prog_address = ASP_ADDR16();
      for(i=0; i<n; i++) ISP_writeEEPROM(prog_address++, ASP_buffer[i]);
      break;

    case USBASP_FUNC_TPI_WRITEBLOCK:
      prog_address = ASP_ADDR16();
      if(n > 255) n = 255;
      TPI_writeBlock(prog_address, ASP_buffer, n);
      prog_address += n;
      break;

    default:
      break;
  }
}

// Execute setup request, returns length of reply in buffer
static uint16_t ASP_handleSetup(void) {
  uint16_t i;
  uint16_t len = ASP_wLength;

  switch(ASP_setup[1]) {

    case USBASP_FUNC_CONNECT:
      prog_address_newmode = 0;               // set compatibility mode of address delivering
      ISP_connect();                          // connect ISP bus
      return 0;

    case USBASP_FUNC_DISCONNECT:
      ISP_disconnect();                       // disconnect ISP bus
      return 0;

    case USBASP_FUNC_TRANSMIT:
      for(i=0; i<4; i++) ASP_buffer[i] = ISP_transmit(ASP_setup[i + 2]);
      return 4;

    case USBASP_FUNC_READFLASH:
      if(!prog_address_newmode) prog_address = ASP_ADDR16();
      for(i=0; i<len; i++) ASP_buffer[i] = ISP_readFlash(prog_address++);
      return len;

    case USBASP_FUNC_READEEPROM:
      if(!prog_address_newmode) prog_address = ASP_ADDR16();
      for(i=0; i<len; i++) ASP_buffer[i] = ISP_readEEPROM(prog_address++);
      return len;

    case USBASP_FUNC_ENABLEPROG:
      ASP_buffer[0] = ISP_enterProgrammingMode();
      return 1;

    case USBASP_FUNC_WRITEFLASH:              // write requests without data stage
    case USBASP_FUNC_WRITEEEPROM:
    case USBASP_FUNC_TPI_WRITEBLOCK:
      ASP_len = 0;
      ASP_handleWrite();
      return 0;

    case USBASP_FUNC_SETLONGADDRESS:
      prog_address_newmode = 1;               // set new mode of address delivering
      prog_address = (uint32_t)ASP_setup[2]       | ((uint32_t)ASP_setup[3] << 8)
                   | ((uint32_t)ASP_setup[4] << 16) | ((uint32_t)ASP_setup[5] << 24);
      return 0;

    case USBASP_FUNC_SETISPSCK:
      ISP_sck = ASP_setup[2];                 // set SCK option
      ASP_buffer[0] = 0;
      return 1;

    case USBASP_FUNC_TPI_CONNECT:
      TPI_dly_cnt = ASP_ADDR16();             // set TPI clock delay
      TPI_connect();
      return 0;

    case USBASP_FUNC_TPI_DISCONNECT:
      TPI_disconnect();
      return 0;

    case USBASP_FUNC_TPI_RAWREAD:
      ASP_buffer[0] = TPI_readByte();
      return 1;

    case USBASP_FUNC_TPI_RAWWRITE:
      TPI_writeByte(ASP_setup[2]);
      return 0;

    case USBASP_FUNC_TPI_READBLOCK:
      prog_address = ASP_ADDR16();
      if(len > 255) len = 255;
      TPI_readBlock(prog_address, ASP_buffer, len);
      prog_address += len;
      return len;

    case USBASP_FUNC_GETCAPABILITIES:
      ASP_buffer[0] = USBASP_CAP_0_TPI;
      ASP_buffer[1] = 0;
      ASP_buffer[2] = 0;
      ASP_buffer[3] = USBASP_CAP_3_3MHZ;
      return 4;

    default:
      return 0;
  }
}

// Handle pending USBasp requests (call in main loop)
void ASP_poll(void) {
  uint8_t  state = ASP_state;
  uint8_t  seq;
  uint16_t len = 0;

  if((state != ASP_SETUP) && (state != ASP_WRITE)) return;
  seq = ASP_seq;
  ASP_state = ASP_BUSY;
  if(state == ASP_SETUP) len = ASP_handleSetup();
  else ASP_handleWrite();
  ASP_len = len;
  if(seq == ASP_seq) ASP_state = ASP_READY;   // reply is ready (if no new request came in)
}

// Init USBasp
void ASP_init(void) {
  ISP_LED_init();                             // init status LEDs
  ASP_LED_USB_init();
  ISP_disconnect();                           // all ISP pins to high impedance
  usb_setup();                                // setup USB
}

// ===================================================================================
// USB Handler Functions (called from USB interrupt)
// ===================================================================================

// Setup packet of a control transfer which is not handled by the USB stack
void usb_handle_other_control_message(struct usb_endpoint *e, struct usb_urb *s, struct rv003usb_internal *ist) {
  uint8_t i;
  uint8_t type = s->wRequestTypeLSBRequestMSB;

  // Only vendor requests are handled (others get an empty reply)
  if((type & USB_REQ_TYP_MASK) != USB_REQ_TYP_VENDOR) return;

  #ifdef WCID_VENDOR_CODE
  // Microsoft WCID feature request
  if((s->wRequestTypeLSBRequestMSB >> 8) == WCID_VENDOR_CODE) {
    if((s->lValueLSBIndexMSB >> 16) == 0x0004) {
      e->opaque  = (uint8_t*)WCID_FEATURE_DESCR;
      e->max_len = (s->wLength < sizeof(WCID_FEATURE_DESCR)) ? s->wLength : sizeof(WCID_FEATURE_DESCR);
    }
    return;
  }
  #endif

  // USBasp request: copy setup packet, execution in main loop
  for(i=0; i<8; i++) ASP_setup[i] = ((uint8_t*)s)[i];
  ASP_wLength = (s->wLength < ASP_BUF_SIZE) ? s->wLength : ASP_BUF_SIZE;
  ASP_len     = 0;
  ASP_seq++;
  e->custom   = 1;                            // IN transfers are handled by user function
  if(!(type & USB_REQ_TYP_IN) && ASP_wLength) ASP_state = ASP_RECEIVE;
  else ASP_state = ASP_SETUP;
}

// Data stage of a control write: collect data in buffer
void usb_handle_user_data(struct usb_endpoint *e, int current_endpoint, uint8_t *data, int len, struct rv003usb_internal *ist) {
  uint16_t pos;
  if(ASP_state != ASP_RECEIVE) return;
  pos = ASP_len;
  while((len-- > 0) && (pos < ASP_wLength)) ASP_buffer[pos++] = *data++;
  ASP_len = pos;
  if(pos >= ASP_wLength) ASP_state = ASP_WRITE;
}

// IN request: send reply data, or NAK if request is not yet executed
void usb_handle_user_in_request(struct usb_endpoint *e, uint8_t *scratchpad, int endp, uint32_t sendtok, struct rv003usb_internal *ist) {
  int offset, len;
  if(ASP_state != ASP_READY) {
    usb_send_data(0, 0, 2, 0x5A);             // send NAK, host will try again
    return;
  }
  offset = e->count << 3;
  len = (int)ASP_len - offset;
  if(len > 8) len = 8;
  if(len <= 0) usb_send_empty(sendtok);
  else usb_send_data(ASP_buffer + offset, len, 0, sendtok);
}
