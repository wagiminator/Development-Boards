// ===================================================================================
// USB Handler for STC8H8K64U                                                 * v1.0 *
// ===================================================================================
//
// All accesses to the USB peripheral (SIE registers via USBADR/USBDAT) are
// performed exclusively within this handler. Class drivers only use the
// endpoint functions USB_EP_init(), USB_EP_send(), USB_EP_receive() and the
// EP0 buffer / setup variables.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_handler.h"

// Functions in this file are called from the interrupt service routine and from
// the main program (with USB interrupt disabled). Their local variables must not
// be overlaid with those of other functions.
#pragma nooverlay

// ===================================================================================
// Variables
// ===================================================================================
volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config, USB_RX_len;
volatile uint16_t USB_SetupLen;
volatile __bit    USB_ENUM_OK;
__code uint8_t*   USB_pDescr;

// EP0 control transfer states
#define USB_EP0_IDLE      0                 // waiting for SETUP
#define USB_EP0_DATAIN    1                 // data stage IN  (device -> host)
#define USB_EP0_DATAOUT   2                 // data stage OUT (host -> device)

static volatile uint8_t USB_EP0_state;      // current EP0 state
static __bit            USB_EP0_zlp;        // zero-length packet required at the end

// Endpoint management (bit n = endpoint n)
static volatile uint8_t USB_EP_INmask;      // configured IN endpoints
static volatile uint8_t USB_EP_OUTmask;     // configured OUT endpoints
static volatile uint8_t USB_EP_INbusy;      // IN packet in FIFO, waiting for host
static volatile uint8_t USB_EP_OUTbusy;     // OUT packet delivered, waiting for receive
static __xdata uint8_t* __xdata USB_EP_INbuf[6];  // IN  buffer of each endpoint
static __xdata uint8_t* __xdata USB_EP_OUTbuf[6]; // OUT buffer of each endpoint

// USB interrupt enable/disable (EUSB = IE2.7, not bit-addressable)
#define USB_INT_save()    uint8_t ie2_save = IE2 & 0x80; IE2 &= 0x7f
#define USB_INT_restore() IE2 |= ie2_save

// ===================================================================================
// SIE Register Access (indirect via USBADR/USBDAT)
// ===================================================================================
static uint8_t USB_readReg(uint8_t addr) {
  while(USBADR & USB_BUSY);                 // wait for SIE ready
  USBADR = addr | USB_BUSY;                 // start read
  while(USBADR & USB_BUSY);                 // wait for data
  return USBDAT;                            // return data
}

#define USB_writeReg(addr, dat) {                                     \
  while(USBADR & USB_BUSY);                 /* wait for SIE ready */  \
  USBADR = (addr);                          /* set address        */  \
  USBDAT = (dat);                           /* write data         */  \
}

// ===================================================================================
// USB Init Function
// ===================================================================================
void USB_init(void) {
  P_SW2 |= 0x80;                            // enable access to extended registers
  P3M0  &= ~0x03;                           // P3.0 (D-), P3.1 (D+) as input
  P3M1  |=  0x03;                           // (high impedance)
  IRC48MCR = 0x80;                          // enable 48MHz internal oscillator
  while(!(IRC48MCR & 0x01));                // wait for oscillator to be stable

  USBCLK = 0x00;                            // USB clock from IRC48M
  USBCON = ENUSB | PUEN;                    // enable USB and D+ pull-up

  USB_writeReg(FADDR,     0x00);            // reset device address
  USB_writeReg(POWER,     USBRST);          // reset SIE
  USB_writeReg(INTRIN1E,  0x3f);            // enable IN interrupts EP0..EP5
  USB_writeReg(INTROUT1E, 0x3e);            // enable OUT interrupts EP1..EP5
  USB_writeReg(INTRUSBE,  RSTIF | SUSIF | RSUIF); // enable bus interrupts
  USB_writeReg(POWER,     0x00);            // release SIE reset

  USB_EP0_state = USB_EP0_IDLE;             // EP0 idle
  USB_Config    = 0;                        // not configured
  USB_ENUM_OK   = 0;                        // not enumerated
  #ifdef USB_INIT_endpoints
  USB_INIT_endpoints();                     // custom EP init handler
  #endif

  IE2 |= 0x80;                              // enable USB interrupt (EUSB)
  EA   = 1;                                 // enable global interrupts
}

// ===================================================================================
// Endpoint Functions (EP1..EP5)
// ===================================================================================

// Setup/reset endpoint ep with buffer buf, out=1: OUT enabled, in=1: IN enabled.
// If both directions are used, the OUT data is at buf[0..63], the IN data at buf[64..127].
void USB_EP_init(uint8_t ep, __xdata uint8_t* buf, uint8_t out, uint8_t in) {
  uint8_t mask = 1 << ep;
  USB_INT_save();
  USB_writeReg(INDEX, ep);
  if(in) {
    USB_EP_INbuf[ep] = out ? buf + 64 : buf;
    USB_writeReg(INCSR2, INMODEIN);         // FIFO direction: IN
    USB_writeReg(INMAXP, 64 / 8);           // max packet size (units of 8 bytes)
    USB_writeReg(INCSR1, INCLRDT | INFLUSH);// reset data toggle, flush FIFO
    USB_EP_INmask |= mask;
  }
  if(out) {
    USB_EP_OUTbuf[ep] = buf;
    USB_writeReg(INCSR2, INMODEOUT);        // FIFO direction: OUT
    USB_writeReg(OUTMAXP, 64 / 8);          // max packet size (units of 8 bytes)
    USB_writeReg(OUTCSR1, OUTCLRDT | OUTFLUSH); // reset data toggle, flush FIFO
    USB_EP_OUTmask |= mask;
  }
  USB_EP_INbusy  &= ~mask;                  // nothing to send
  USB_EP_OUTbusy &= ~mask;                  // ready to receive
  USB_writeReg(INDEX, 0);
  USB_INT_restore();
}

// Transmit len bytes from IN buffer of endpoint ep
void USB_EP_send(uint8_t ep, uint8_t len) {
  __xdata uint8_t* ptr = USB_EP_INbuf[ep];
  uint8_t csr;
  USB_INT_save();
  USB_writeReg(INDEX, ep);
  while(len--) USB_writeReg(FIFO0 + ep, *ptr++);  // copy data to FIFO
  csr = USB_readReg(INCSR1) & INSDSTL;      // keep halt state
  USB_writeReg(INCSR1, csr | INIPRDY);      // packet ready to be sent
  USB_EP_INbusy |= 1 << ep;                 // waiting for completion
  USB_INT_restore();
}

// Release OUT buffer of endpoint ep, ready to receive next OUT packet
void USB_EP_receive(uint8_t ep) {
  uint8_t csr;
  USB_INT_save();
  USB_writeReg(INDEX, ep);
  csr = USB_readReg(OUTCSR1) & OUTSDSTL;    // keep halt state
  USB_writeReg(OUTCSR1, csr);               // clear OPRDY -> ACK next packet
  USB_EP_OUTbusy &= ~(1 << ep);
  USB_INT_restore();
}

// ===================================================================================
// Endpoint Callbacks (called within the interrupt)
// ===================================================================================
static void USB_EP_IN_callback(uint8_t ep) {
  switch(ep) {
    #ifdef EP1_IN_callback
    case 1: EP1_IN_callback(); break;
    #endif
    #ifdef EP2_IN_callback
    case 2: EP2_IN_callback(); break;
    #endif
    #ifdef EP3_IN_callback
    case 3: EP3_IN_callback(); break;
    #endif
    #ifdef EP4_IN_callback
    case 4: EP4_IN_callback(); break;
    #endif
    #ifdef EP5_IN_callback
    case 5: EP5_IN_callback(); break;
    #endif
    default: break;
  }
}

static void USB_EP_OUT_callback(uint8_t ep) {
  switch(ep) {
    #ifdef EP1_OUT_callback
    case 1: EP1_OUT_callback(); break;
    #endif
    #ifdef EP2_OUT_callback
    case 2: EP2_OUT_callback(); break;
    #endif
    #ifdef EP3_OUT_callback
    case 3: EP3_OUT_callback(); break;
    #endif
    #ifdef EP4_OUT_callback
    case 4: EP4_OUT_callback(); break;
    #endif
    #ifdef EP5_OUT_callback
    case 5: EP5_OUT_callback(); break;
    #endif
    default: USB_EP_receive(ep); break;     // no callback -> discard data
  }
}

// IN endpoint interrupt: packet sent or STALL sent
static void USB_EP_IN_handler(uint8_t ep) {
  uint8_t csr, mask = 1 << ep;
  USB_writeReg(INDEX, ep);
  csr = USB_readReg(INCSR1);
  if(csr & INSTSTL) USB_writeReg(INCSR1, csr & INSDSTL);  // clear STSTL (FIFO flushed)
  if((USB_EP_INbusy & mask) && !(csr & INIPRDY)) {        // packet transmitted?
    USB_EP_INbusy &= ~mask;
    USB_EP_IN_callback(ep);
  }
}

// OUT endpoint interrupt: packet received or STALL sent
static void USB_EP_OUT_handler(uint8_t ep) {
  uint8_t csr, len, mask = 1 << ep;
  __xdata uint8_t* ptr;
  USB_writeReg(INDEX, ep);
  csr = USB_readReg(OUTCSR1);
  if(csr & OUTSTSTL) USB_writeReg(OUTCSR1, csr & (OUTSDSTL | OUTOPRDY)); // clear STSTL only
  if((csr & OUTOPRDY) && !(USB_EP_OUTbusy & mask) && (USB_EP_OUTmask & mask)) {
    len = USB_readReg(OUTCOUNT1);           // number of received bytes
    if(len > 64) len = 64;
    USB_RX_len = len;
    ptr = USB_EP_OUTbuf[ep];
    while(len--) *ptr++ = USB_readReg(FIFO0 + ep);  // copy FIFO to buffer
    USB_EP_OUTbusy |= mask;                 // NAK until USB_EP_receive()
    USB_EP_OUT_callback(ep);
  }
}

// ===================================================================================
// Fast Copy Function
// ===================================================================================
// Copy descriptor *USB_pDescr to EP0_buffer
void USB_EP0_copyDescr(uint8_t len) {
  __xdata uint8_t* ptr = EP0_buffer;
  while(len--) *ptr++ = *USB_pDescr++;
}

// ===================================================================================
// Endpoint 0 Functions
// ===================================================================================

// Write EP0_buffer to FIFO0 and set IN packet ready (last packet: data end)
static void USB_EP0_transmit(uint8_t len) {
  __xdata uint8_t* ptr = EP0_buffer;
  uint8_t last;
  USB_SetupLen -= len;
  last = (USB_SetupLen == 0) && ((len < EP0_SIZE) || !USB_EP0_zlp);
  while(len--) USB_writeReg(FIFO0, *ptr++);
  if(last) {
    USB_writeReg(CSR0, IPRDY | DATEND);     // last packet of data stage
    USB_EP0_state = USB_EP0_IDLE;
  }
  else {
    USB_writeReg(CSR0, IPRDY);              // more packets will follow
    USB_EP0_state = USB_EP0_DATAIN;
  }
}

// Get the halt (STALL) state of an endpoint; returns 0xff if not configured
static uint8_t USB_EP_getHalt(uint8_t epaddr) {
  uint8_t ep = epaddr & 0x0f, res = 0xff;
  if((ep == 0) && !(epaddr & 0x70)) return 0;
  if((ep > 5) || (epaddr & 0x70)) return 0xff;
  USB_writeReg(INDEX, ep);
  if(epaddr & 0x80) {
    if(USB_EP_INmask & (1 << ep))  res = (USB_readReg(INCSR1)  & INSDSTL)  ? 1 : 0;
  }
  else {
    if(USB_EP_OUTmask & (1 << ep)) res = (USB_readReg(OUTCSR1) & OUTSDSTL) ? 1 : 0;
  }
  USB_writeReg(INDEX, 0);
  return res;
}

// Set (halt=1) or clear (halt=0) endpoint halt; returns 0 if successful, 0xff if not
static uint8_t USB_EP_setHalt(uint8_t epaddr, uint8_t halt) {
  uint8_t ep = epaddr & 0x0f, mask = 1 << ep, res = 0xff;
  if((ep == 0) || (ep > 5) || (epaddr & 0x70)) return 0xff;
  USB_writeReg(INDEX, ep);
  if(epaddr & 0x80) {
    if(USB_EP_INmask & mask) {
      USB_writeReg(INCSR1, halt ? INSDSTL : INCLRDT);   // stall / reset toggle
      res = 0;
    }
  }
  else {
    if(USB_EP_OUTmask & mask) {
      USB_writeReg(OUTCSR1, halt ? OUTSDSTL : OUTCLRDT);// stall / reset toggle
      res = 0;
    }
  }
  USB_writeReg(INDEX, 0);
  return res;
}

// Endpoint 0 SETUP handler, returns length of data in EP0_buffer or 0xff (stall)
static uint8_t USB_EP0_SETUP(void) {
  uint8_t len = 0;                                // default is success and upload 0 length
  USB_SetupLen = ((uint16_t)USB_SetupBuf->wLengthH<<8) | (USB_SetupBuf->wLengthL);
  USB_SetupReq = USB_SetupBuf->bRequest;
  USB_SetupTyp = USB_SetupBuf->bRequestType;

  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD) {
    switch(USB_SetupReq) {                        // request type
      case USB_GET_DESCRIPTOR:
        switch(USB_SetupBuf->wValueH) {

          case USB_DESCR_TYP_DEVICE:              // Device Descriptor
            USB_pDescr = (uint8_t*)&DevDescr;     // put descriptor into out buffer
            len = sizeof(DevDescr);               // descriptor length
            break;

          case USB_DESCR_TYP_CONFIG:              // Configuration Descriptor
            USB_pDescr = (uint8_t*)&CfgDescr;     // put descriptor into out buffer
            len = sizeof(CfgDescr);               // descriptor length
            break;

          case USB_DESCR_TYP_STRING:
            switch(USB_SetupBuf->wValueL) {       // String Descriptor Index
              case 0:   USB_pDescr = USB_STR_DESCR_i0; break;
              case 1:   USB_pDescr = USB_STR_DESCR_i1; break;
              case 2:   USB_pDescr = USB_STR_DESCR_i2; break;
              case 3:   USB_pDescr = USB_STR_DESCR_i3; break;
              #ifdef USB_STR_DESCR_i4
              case 4:   USB_pDescr = USB_STR_DESCR_i4; break;
              #endif
              #ifdef USB_STR_DESCR_i5
              case 5:   USB_pDescr = USB_STR_DESCR_i5; break;
              #endif
              #ifdef USB_STR_DESCR_i6
              case 6:   USB_pDescr = USB_STR_DESCR_i6; break;
              #endif
              #ifdef USB_STR_DESCR_i7
              case 7:   USB_pDescr = USB_STR_DESCR_i7; break;
              #endif
              #ifdef USB_STR_DESCR_i8
              case 8:   USB_pDescr = USB_STR_DESCR_i8; break;
              #endif
              #ifdef USB_STR_DESCR_i9
              case 9:   USB_pDescr = USB_STR_DESCR_i9; break;
              #endif
              #ifdef USB_STR_DESCR_ixee
              case 0xee:  USB_pDescr = USB_STR_DESCR_ixee; break;
              #endif
              default:  USB_pDescr = USB_STR_DESCR_ix; break;
            }
            len = USB_pDescr[0];                  // descriptor length
            break;

          #ifdef USB_REPORT_DESCR
          case USB_DESCR_TYP_REPORT:
            if(USB_SetupBuf->wValueL == 0) {
              USB_pDescr = USB_REPORT_DESCR;
              len = USB_REPORT_DESCR_LEN;
            }
            else len = 0xff;
            break;
          #endif

          default:
            len = 0xff;                           // unsupported descriptors or error
            break;
        }

        if(len != 0xff) {
          if(USB_SetupLen > len) USB_SetupLen = len;    // limit length
          len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
          USB_EP0_copyDescr(len);                 // copy descriptor to EP0
        }
        break;

      case USB_SET_ADDRESS:
        USB_writeReg(FADDR, USB_SetupBuf->wValueL & 0x7f); // SIE updates after status
        break;

      case USB_GET_CONFIGURATION:
        EP0_buffer[0] = USB_Config;
        if(USB_SetupLen > 1) USB_SetupLen = 1;
        len = USB_SetupLen;
        break;

      case USB_SET_CONFIGURATION:
        if(USB_SetupBuf->wValueL > 1) {
          len = 0xff;                             // only configuration 1 available
          break;
        }
        USB_Config  = USB_SetupBuf->wValueL;
        USB_ENUM_OK = (USB_Config != 0);
        #ifdef USB_INIT_endpoints
        USB_INIT_endpoints();                     // reset endpoints and data toggles
        #endif
        break;

      case USB_GET_INTERFACE:
        EP0_buffer[0] = 0;                        // only alternate setting 0
        if(USB_SetupLen > 1) USB_SetupLen = 1;
        len = USB_SetupLen;
        break;

      case USB_SET_INTERFACE:
        if(USB_SetupBuf->wValueL) len = 0xff;     // only alternate setting 0
        break;

      case USB_GET_STATUS:
        EP0_buffer[0] = 0x00;
        EP0_buffer[1] = 0x00;
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {
          len = USB_EP_getHalt(USB_SetupBuf->wIndexL);
          if(len == 0xff) break;                  // invalid endpoint
          EP0_buffer[0] = len;                    // halt state
        }
        if(USB_SetupLen > 2) USB_SetupLen = 2;
        len = USB_SetupLen;
        break;

      case USB_CLEAR_FEATURE:
      case USB_SET_FEATURE:
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE) {
          if((USB_SetupBuf->wValueL != 0x01) || !(((uint8_t*)&CfgDescr)[7] & 0x20))
            len = 0xff;                           // remote wakeup not supported
        }
        else if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {
          if(USB_SetupBuf->wValueL == 0x00)       // ENDPOINT_HALT
            len = USB_EP_setHalt(USB_SetupBuf->wIndexL, USB_SetupReq == USB_SET_FEATURE);
          else len = 0xff;
        }
        else len = 0xff;                          // unsupported recipient
        break;

      default:
        len = 0xff;                               // failed
        break;
    }
  }

  #ifdef USB_CLASS_SETUP_handler
  else if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
    len = USB_CLASS_SETUP_handler();
  }
  #endif

  #ifdef USB_VENDOR_SETUP_handler
  else if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) {
    len = USB_VENDOR_SETUP_handler();
  }
  #endif

  else len = 0xff;
  return len;
}

// Endpoint 0 interrupt handler
static void USB_EP0_handler(void) {
  uint8_t csr, len, i;
  uint16_t wlen;

  USB_writeReg(INDEX, 0);
  csr = USB_readReg(CSR0);

  if(csr & STSTL) {                         // STALL has been sent
    USB_writeReg(CSR0, 0);                  // clear STSTL
    USB_EP0_state = USB_EP0_IDLE;
  }
  if(csr & SUEND) {                         // control transfer ended prematurely
    USB_writeReg(CSR0, SSUEND);             // clear SUEND
    USB_EP0_state = USB_EP0_IDLE;
  }

  switch(USB_EP0_state) {

    case USB_EP0_IDLE:                      // SETUP stage
      if(!(csr & OPRDY)) break;
      len = USB_readReg(COUNT0);
      if(len != 8) {                        // not a valid SETUP packet
        for(i=len; i; i--) USB_readReg(FIFO0);
        USB_writeReg(CSR0, SOPRDY | SDSTL);
        break;
      }
      for(i=0; i<8; i++) EP0_buffer[i] = USB_readReg(FIFO0);
      wlen = ((uint16_t)USB_SetupBuf->wLengthH<<8) | (USB_SetupBuf->wLengthL);
      len  = USB_EP0_SETUP();
      USB_writeReg(INDEX, 0);               // handlers may have changed INDEX

      if(len == 0xff) {                     // request not supported -> STALL
        USB_SetupReq = 0xff;
        USB_writeReg(CSR0, SOPRDY | SDSTL);
      }
      else if((USB_SetupTyp & USB_REQ_TYP_IN) && wlen) {  // data stage IN
        USB_writeReg(CSR0, SOPRDY);
        if(len < EP0_SIZE) USB_SetupLen = len;  // short packet ends data stage
        USB_EP0_zlp = (USB_SetupLen < wlen);// shorter than requested -> may need ZLP
        USB_EP0_transmit(len);
      }
      else if(USB_SetupLen) {               // data stage OUT
        USB_writeReg(CSR0, SOPRDY);
        USB_EP0_state = USB_EP0_DATAOUT;
      }
      else {                                // no data stage
        USB_writeReg(CSR0, SOPRDY | DATEND);
      }
      break;

    case USB_EP0_DATAIN:                    // previous IN packet has been sent
      if(csr & IPRDY) break;
      len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
      #ifdef USB_CLASS_IN_handler
      if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) USB_CLASS_IN_handler();
      else
      #endif
      #ifdef USB_VENDOR_IN_handler
      if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) USB_VENDOR_IN_handler();
      else
      #endif
      USB_EP0_copyDescr(len);               // copy next part of descriptor
      USB_writeReg(INDEX, 0);
      USB_EP0_transmit(len);
      break;

    case USB_EP0_DATAOUT:                   // OUT packet received
      if(!(csr & OPRDY)) break;
      len = USB_readReg(COUNT0);
      if(len > EP0_SIZE) len = EP0_SIZE;
      USB_RX_len = len;
      for(i=0; i<len; i++) EP0_buffer[i] = USB_readReg(FIFO0);
      USB_SetupLen = (USB_SetupLen > len) ? (USB_SetupLen - len) : 0;
      #ifdef USB_CLASS_OUT_handler
      if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) USB_CLASS_OUT_handler();
      #endif
      #ifdef USB_VENDOR_OUT_handler
      if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) USB_VENDOR_OUT_handler();
      #endif
      USB_writeReg(INDEX, 0);
      if((USB_SetupLen == 0) || (len < EP0_SIZE)) {
        USB_writeReg(CSR0, SOPRDY | DATEND);// last packet of data stage
        USB_EP0_state = USB_EP0_IDLE;
      }
      else USB_writeReg(CSR0, SOPRDY);      // more packets will follow
      break;

    default:
      USB_EP0_state = USB_EP0_IDLE;
      break;
  }
}

// ===================================================================================
// USB Bus Reset Handler
// ===================================================================================
static void USB_reset(void) {
  USB_writeReg(FADDR, 0x00);                // reset device address
  USB_EP0_state = USB_EP0_IDLE;             // EP0 idle
  USB_Config    = 0;                        // not configured
  USB_ENUM_OK   = 0;                        // not enumerated
  #ifdef USB_RESET_handler
  USB_RESET_handler();                      // custom reset handler
  #endif
  #ifdef USB_INIT_endpoints
  USB_INIT_endpoints();                     // reset endpoints
  #endif
  USB_writeReg(INDEX, 0);
}

// ===================================================================================
// USB Interrupt Service Routine
// ===================================================================================
// Must be called by the USB interrupt service routine in main.c:
// void USB_ISR(void) __interrupt(INT_NO_USB) { USB_interrupt(); }
void USB_interrupt(void) {
  uint8_t intrusb, intrin, introut, ep, mask;

  intrusb = USB_readReg(INTRUSB);           // reading clears the flags
  intrin  = USB_readReg(INTRIN1);
  introut = USB_readReg(INTROUT1);

  // USB bus reset event
  if(intrusb & RSTIF) USB_reset();

  // Endpoint 0 (control transfers)
  if(intrin & EP0IF) USB_EP0_handler();

  // Endpoints 1..5
  for(ep=1, mask=0x02; ep<6; ep++, mask<<=1) {
    if(intrin  & mask) USB_EP_IN_handler(ep);
    if(introut & mask) USB_EP_OUT_handler(ep);
  }

  // USB bus suspend event
  #ifdef USB_SUSPEND_handler
  if(intrusb & SUSIF) USB_SUSPEND_handler();
  #endif

  // USB bus resume event
  #ifdef USB_RESUME_handler
  if(intrusb & RSUIF) USB_RESUME_handler();
  #endif
}
