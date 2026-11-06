// ===================================================================================
// USB Handler for CH32V203 USBD (USB Full-Speed Device)                      * v1.0 *
// ===================================================================================
//
// USB full-speed device using the USBD peripheral (pins PA11 = D-, PA12 = D+) with
// internal 1.5k pull-up resistor on D+. The USBD peripheral is available on all
// CH32V203 except CH32V203F8.
//
// The USBD uses a 512 byte packet memory (PMA, 16-bit words at 32-bit addresses).
// PMA layout: buffer description table (EP0..3) 0x000, EP0 OUT/IN 0x020/0x040
// (32 bytes each), EP1..3 OUT/IN 0x060/0x0A0, 0x0E0/0x120, 0x160/0x1A0 (64 bytes
// each). Only endpoints 0..3 are supported. If CAN is used at the same time, only
// endpoints 0..2 can be used (USBD must stay within the lower 384 bytes of the PMA).
// The data is copied between PMA and the endpoint buffers in RAM, the buffer
// layout is the same as for the USBFS (IN buffer at offset 64, if OUT is used).
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_handler.h"

// ===================================================================================
// Variables
// ===================================================================================
volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config, USB_Addr, USB_ENUM_OK;
volatile uint16_t USB_SetupLen;
volatile uint8_t  USB_RX_len;
const uint8_t*    USB_pDescr;

static uint8_t*   USB_EP_OUTbuf[4];             // endpoint OUT buffers in RAM
static uint8_t*   USB_EP_INbuf[4];              // endpoint IN  buffers in RAM

// ===================================================================================
// Register and Packet Memory Access
// ===================================================================================
#define USB_EPR(n)          (*(__IO uint16_t*)(USBD_BASE + 4*(n)))
#define USB_PMA_ADDR_TX(n)  (*(__IO uint16_t*)(USBD_PMA_BASE + 16*(n) + 0))
#define USB_PMA_COUNT_TX(n) (*(__IO uint16_t*)(USBD_PMA_BASE + 16*(n) + 4))
#define USB_PMA_ADDR_RX(n)  (*(__IO uint16_t*)(USBD_PMA_BASE + 16*(n) + 8))
#define USB_PMA_COUNT_RX(n) (*(__IO uint16_t*)(USBD_PMA_BASE + 16*(n) + 12))
#define USB_PMA_RX(n)       ((uint16_t)((n) ? 0x060 + 0x80*((n)-1) : 0x020))  // OUT buffer
#define USB_PMA_TX(n)       ((uint16_t)((n) ? 0x0A0 + 0x80*((n)-1) : 0x040))  // IN buffer
#define USB_PMA_RX_SIZE32   ((uint16_t)0x8000)              // BL_SIZE=1, NUM_BLOCK=0
#define USB_PMA_RX_SIZE64   ((uint16_t)0x8400)              // BL_SIZE=1, NUM_BLOCK=1
#define USB_EP_MAX          3                               // highest endpoint number

#define USB_EP_TOGGLE_MASK  (USBD_EP_DTOG_RX | USBD_EPRX_STAT | USBD_EP_DTOG_TX | USBD_EPTX_STAT)

// Set TX status of endpoint (status: USBD_EP_TX_DIS/STALL/NAK/ACK)
static inline void USB_EP_setTX(uint8_t ep, uint16_t status) {
  uint16_t reg = USB_EPR(ep);
  USB_EPR(ep) = ((reg & (USBD_EPREG_MASK | USBD_EPTX_STAT)) ^ status)
              | USBD_EP_CTR_RX | USBD_EP_CTR_TX;
}

// Set RX status of endpoint (status: USBD_EP_RX_DIS/STALL/NAK/ACK)
static inline void USB_EP_setRX(uint8_t ep, uint16_t status) {
  uint16_t reg = USB_EPR(ep);
  USB_EPR(ep) = ((reg & (USBD_EPREG_MASK | USBD_EPRX_STAT)) ^ status)
              | USBD_EP_CTR_RX | USBD_EP_CTR_TX;
}

// Clear CTR_RX/CTR_TX flags
static inline void USB_EP_clearRX(uint8_t ep) {
  USB_EPR(ep) = USB_EPR(ep) & (USBD_EPREG_MASK & ~USBD_EP_CTR_RX);
}
static inline void USB_EP_clearTX(uint8_t ep) {
  USB_EPR(ep) = USB_EPR(ep) & (USBD_EPREG_MASK & ~USBD_EP_CTR_TX);
}

// Configure endpoint register (type, address, status), resets data toggles
static void USB_EP_config(uint8_t ep, uint16_t type, uint16_t rxstat, uint16_t txstat) {
  uint16_t reg = USB_EPR(ep);
  USB_EPR(ep) = type | ep | ((reg ^ (rxstat | txstat)) & USB_EP_TOGGLE_MASK);
}

// Copy data from RAM to packet memory
static void USB_PMA_write(uint16_t pma, const uint8_t* buf, uint8_t len) {
  __IO uint16_t* dst = (__IO uint16_t*)(USBD_PMA_BASE + 2*pma);
  while(len >= 2) {
    *dst = (uint16_t)buf[0] | ((uint16_t)buf[1] << 8);
    dst += 2; buf += 2; len -= 2;                 // 16-bit words at 32-bit addresses
  }
  if(len) *dst = buf[0];
}

// Copy data from packet memory to RAM
static void USB_PMA_read(uint16_t pma, uint8_t* buf, uint8_t len) {
  __IO uint16_t* src = (__IO uint16_t*)(USBD_PMA_BASE + 2*pma);
  while(len >= 2) {
    uint16_t w = *src;
    src += 2;                                     // 16-bit words at 32-bit addresses
    *buf++ = w; *buf++ = w >> 8;
    len -= 2;
  }
  if(len) *buf = *src;
}

// ===================================================================================
// Endpoint Functions (also used by the class drivers)
// ===================================================================================

// Setup endpoint (ep: 1..3, buf: endpoint buffer, out/in: enable direction)
void USB_EP_init(uint8_t ep, uint8_t type, uint8_t* buf, uint8_t out, uint8_t in) {
  uint16_t eptype;
  if(!ep || (ep > USB_EP_MAX)) return;
  switch(type) {
    case USB_ENDP_TYPE_INTER: eptype = USBD_EP_INTERRUPT;   break;
    case USB_ENDP_TYPE_ISOCH: eptype = USBD_EP_ISOCHRONOUS; break;
    default:                  eptype = USBD_EP_BULK;        break;
  }
  USB_EP_OUTbuf[ep]    = buf;
  USB_EP_INbuf[ep]     = out ? buf + 64 : buf;
  USB_PMA_ADDR_TX(ep)  = USB_PMA_TX(ep);
  USB_PMA_COUNT_TX(ep) = 0;
  USB_PMA_ADDR_RX(ep)  = USB_PMA_RX(ep);
  USB_PMA_COUNT_RX(ep) = USB_PMA_RX_SIZE64;
  USB_EP_config(ep, eptype, out ? USBD_EP_RX_ACK : USBD_EP_RX_DIS,
                             in ? USBD_EP_TX_NAK : USBD_EP_TX_DIS);
}

// Transmit len bytes of endpoint IN buffer to host
void USB_EP_send(uint8_t ep, uint8_t len) {
  USB_PMA_write(USB_PMA_TX(ep), USB_EP_INbuf[ep], len);
  USB_PMA_COUNT_TX(ep) = len;
  USB_EP_setTX(ep, USBD_EP_TX_ACK);
}

// Ready to receive next OUT packet (ACK)
void USB_EP_receive(uint8_t ep) {
  USB_EP_setRX(ep, USBD_EP_RX_ACK);
}

// Do not receive OUT packets (NAK)
void USB_EP_hold(uint8_t ep) {
  USB_EP_setRX(ep, USBD_EP_RX_NAK);
}

// Set (stall=1) or clear (stall=0) endpoint halt feature (addr: endpoint address)
static uint8_t USB_EP_halt(uint8_t addr, uint8_t stall) {
  uint8_t ep = addr & 0x0f;
  uint16_t reg;
  if(!ep || (ep > USB_EP_MAX)) return 0;
  if(addr & 0x80) {
    if(stall) USB_EP_setTX(ep, USBD_EP_TX_STALL);
    else {
      reg = USB_EPR(ep);                        // reset data toggle to DATA0
      if(reg & USBD_EP_DTOG_TX)
        USB_EPR(ep) = (reg & USBD_EPREG_MASK) | USBD_EP_DTOG_TX | USBD_EP_CTR_RX | USBD_EP_CTR_TX;
      USB_EP_setTX(ep, USBD_EP_TX_NAK);
    }
  }
  else {
    if(stall) USB_EP_setRX(ep, USBD_EP_RX_STALL);
    else {
      reg = USB_EPR(ep);                        // reset data toggle to DATA0
      if(reg & USBD_EP_DTOG_RX)
        USB_EPR(ep) = (reg & USBD_EPREG_MASK) | USBD_EP_DTOG_RX | USBD_EP_CTR_RX | USBD_EP_CTR_TX;
      USB_EP_setRX(ep, USBD_EP_RX_ACK);
    }
  }
  return 1;
}

// Check if endpoint is halted
static uint8_t USB_EP_isHalted(uint8_t addr) {
  uint8_t ep = addr & 0x0f;
  if(ep > USB_EP_MAX) return 0;
  if(addr & 0x80) return (USB_EPR(ep) & USBD_EPTX_STAT) == USBD_EP_TX_STALL;
  return (USB_EPR(ep) & USBD_EPRX_STAT) == USBD_EP_RX_STALL;
}

// ===================================================================================
// Setup/Reset Endpoints
// ===================================================================================
static void USB_EP_reset(void) {
  uint8_t i;
  USBD->BTABLE = 0;                         // buffer description table at PMA 0x00
  USB_PMA_ADDR_TX(0)  = USB_PMA_TX(0);
  USB_PMA_COUNT_TX(0) = 0;
  USB_PMA_ADDR_RX(0)  = USB_PMA_RX(0);
  USB_PMA_COUNT_RX(0) = USB_PMA_RX_SIZE32;
  USB_EP_config(0, USBD_EP_CONTROL, USBD_EP_RX_ACK, USBD_EP_TX_NAK);
  for(i=1; i<8; i++) USB_EP_config(i, 0, USBD_EP_RX_DIS, USBD_EP_TX_DIS);
  for(i=1; i<=USB_EP_MAX; i++) USB_EP_OUTbuf[i] = USB_EP_INbuf[i] = 0;

  #ifdef USB_INIT_endpoints
  USB_INIT_endpoints();                     // custom EP init handler
  #endif

  USBD->DADDR = USBD_DADDR_EF;              // enable function, address 0
  USB_ENUM_OK = 0;
  USB_Config  = 0;
  USB_Addr    = 0;
}

// ===================================================================================
// USB Init Function
// ===================================================================================
void USB_init(void) {
  // Set pins PA11 (D-) and PA12 (D+) to floating input
  RCC->APB2PCENR |= RCC_IOPAEN;
  GPIOA->CFGHR    = (GPIOA->CFGHR & ~(((uint32_t)0b1111<<(3<<2)) | ((uint32_t)0b1111<<(4<<2))))
                                  |  (((uint32_t)0b0100<<(3<<2)) | ((uint32_t)0b0100<<(4<<2)));
  GPIOA->BCR      = ((uint32_t)1<<11) | ((uint32_t)1<<12);

  // Set USB clock to 48MHz (PLL / 1, 2 or 3) and enable USBD module
  RCC->CFGR0 = (RCC->CFGR0 & ~USB_RCC_USBPRE_MASK) | USB_RCC_USBPRE;
  RCC->APB1PCENR |= RCC_USBEN;

  // Power up and reset USBD
  USBD->CNTR = USBD_CNTR_FRES;              // power up (PDWN = 0), keep reset
  DLY_us(10);                               // wait for analog circuit to start
  USBD->CNTR = 0;                           // release reset
  USBD->ISTR = 0;                           // clear interrupt flags
  USB_EP_reset();
  USBD->CNTR = USBD_CNTR_CTRM | USBD_CNTR_RESETM | USBD_CNTR_SUSPM | USBD_CNTR_WKUPM;

  // Connect: full-speed, internal pull-up on D+
  EXTEN->EXTEN_CTR = (EXTEN->EXTEN_CTR & ~EXTEN_USBD_LS) | EXTEN_USBD_PU_EN;
  NVIC_EnableIRQ(USB_LP_CAN1_RX0_IRQn);
}

// ===================================================================================
// Endpoint Copy Function
// ===================================================================================
// Copy descriptor *USB_pDescr to Ep0
void USB_EP0_copyDescr(uint8_t len) {
  uint8_t* tgt = EP0_buffer;
  while(len--) *tgt++ = *USB_pDescr++;
}

// Transmit EP0 buffer
static void USB_EP0_send(uint8_t len) {
  USB_PMA_write(USB_PMA_TX(0), EP0_buffer, len);
  USB_PMA_COUNT_TX(0) = len;
  USB_EP_setTX(0, USBD_EP_TX_ACK);
}

// ===================================================================================
// Endpoint EP0 Handlers
// ===================================================================================

// Endpoint 0 SETUP handler
static void USB_EP0_SETUP(void) {
  uint8_t len = 0;
  USB_SetupLen = ((uint16_t)USB_SetupBuf->wLengthH<<8) | (USB_SetupBuf->wLengthL);
  USB_SetupReq = USB_SetupBuf->bRequest;
  USB_SetupTyp = USB_SetupBuf->bRequestType;

  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_STANDARD) {
    switch(USB_SetupReq) {
      case USB_GET_DESCRIPTOR:
        switch(USB_SetupBuf->wValueH) {

          case USB_DESCR_TYP_DEVICE:
            USB_pDescr = (uint8_t*)&DevDescr;
            len = sizeof(DevDescr);
            break;

          case USB_DESCR_TYP_CONFIG:
            USB_pDescr = (uint8_t*)&CfgDescr;
            len = sizeof(CfgDescr);
            break;

          case USB_DESCR_TYP_STRING:
            switch(USB_SetupBuf->wValueL) {
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
              default:  USB_pDescr = USB_STR_DESCR_ix; break;
            }
            len = USB_pDescr[0];
            break;

          #ifdef USB_REPORT_DESCR
          case USB_DESCR_TYP_REPORT:
            if(USB_SetupBuf->wIndexL == 0) {
              USB_pDescr = USB_REPORT_DESCR;
              len = USB_REPORT_DESCR_LEN;
            }
            else len = 0xff;
            break;
          #endif

          #ifdef USB_HID_DESCR_PTR
          case USB_DESCR_TYP_HID:
            if(USB_SetupBuf->wIndexL == 0) {
              USB_pDescr = USB_HID_DESCR_PTR;
              len = USB_HID_DESCR_PTR[0];
            }
            else len = 0xff;
            break;
          #endif

          default:
            len = 0xff;
            break;
        }

        if(len != 0xff) {
          if(USB_SetupLen > len) USB_SetupLen = len;
          len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
          USB_EP0_copyDescr(len);
        }
        break;

      case USB_SET_ADDRESS:
        USB_Addr = USB_SetupBuf->wValueL;
        break;

      case USB_GET_CONFIGURATION:
        EP0_buffer[0] = USB_Config;
        if(USB_SetupLen > 1) USB_SetupLen = 1;
        len = USB_SetupLen;
        break;

      case USB_SET_CONFIGURATION:
        USB_Config  = USB_SetupBuf->wValueL;
        USB_ENUM_OK = 1;
        break;

      case USB_GET_INTERFACE:
        EP0_buffer[0] = 0x00;                 // only alternate setting 0
        if(USB_SetupLen > 1) USB_SetupLen = 1;
        len = USB_SetupLen;
        break;

      case USB_SET_INTERFACE:
        break;

      case USB_GET_STATUS:
        EP0_buffer[0] = 0x00;
        EP0_buffer[1] = 0x00;
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
          EP0_buffer[0] = USB_EP_isHalted(USB_SetupBuf->wIndexL);
        if(USB_SetupLen > 2) USB_SetupLen = 2;
        len = USB_SetupLen;
        break;

      case USB_CLEAR_FEATURE:
      case USB_SET_FEATURE:
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE) {
          if((USB_SetupBuf->wValueL != 0x01) || !(((uint8_t*)&CfgDescr)[7] & 0x20))
            len = 0xff;                       // only remote wakeup, if supported
        }
        else if(((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
             && (USB_SetupBuf->wValueL == 0x00)) {
          if(!USB_EP_halt(USB_SetupBuf->wIndexL, USB_SetupReq == USB_SET_FEATURE))
            len = 0xff;
        }
        else len = 0xff;
        break;

      default:
        len = 0xff;
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

  if(len == 0xff) {                           // request not supported -> STALL
    USB_SetupReq = 0xff;
    USB_EP_setTX(0, USBD_EP_TX_STALL);
    USB_EP_setRX(0, USBD_EP_RX_STALL);
  }
  else if(USB_SetupTyp & USB_REQ_TYP_IN) {    // data stage IN
    USB_SetupLen -= len;
    USB_EP0_send(len);
    USB_EP_setRX(0, USBD_EP_RX_ACK);          // for status stage
  }
  else if(USB_SetupLen) {                     // data stage OUT
    USB_EP_setRX(0, USBD_EP_RX_ACK);
  }
  else {                                      // no data stage: status IN (ZLP)
    USB_EP0_send(0);
    USB_EP_setRX(0, USBD_EP_RX_ACK);
  }
}

// Endpoint 0 IN handler
static void USB_EP0_IN(void) {
  uint8_t len;

  #ifdef USB_CLASS_IN_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
    USB_CLASS_IN_handler();
    return;
  }
  #endif

  if((USB_SetupTyp & USB_REQ_TYP_IN) && (USB_SetupReq == USB_GET_DESCRIPTOR)) {
    len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
    USB_EP0_copyDescr(len);                   // next packet of descriptor
    USB_SetupLen -= len;
    USB_EP0_send(len);
    return;
  }

  if(USB_SetupReq == USB_SET_ADDRESS)         // status stage of SET_ADDRESS completed
    USBD->DADDR = USBD_DADDR_EF | USB_Addr;
}

// Endpoint 0 OUT handler
static void USB_EP0_OUT(void) {
  if(USB_SetupTyp & USB_REQ_TYP_IN) {         // status stage of IN transfer completed
    USB_EP_setRX(0, USBD_EP_RX_ACK);
    return;
  }

  #ifdef USB_CLASS_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) USB_CLASS_OUT_handler();
  #endif

  #ifdef USB_VENDOR_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) USB_VENDOR_OUT_handler();
  #endif

  USB_SetupLen = 0;                           // only single packet data stages
  USB_EP0_send(0);                            // status stage: send ZLP
  USB_EP_setRX(0, USBD_EP_RX_ACK);
}

// ===================================================================================
// USB Interrupt Service Routine
// ===================================================================================
void USB_LP_CAN1_RX0_IRQHandler(void) __attribute__((interrupt));
void USB_LP_CAN1_RX0_IRQHandler(void) {
  uint16_t istr;

  // USB transfer completed interrupt(s)
  while((istr = USBD->ISTR) & USBD_ISTR_CTR) {
    uint8_t  ep  = istr & USBD_ISTR_EP_ID;
    uint16_t epr = USB_EPR(ep);

    // OUT or SETUP transaction completed
    if(epr & USBD_EP_CTR_RX) {
      USB_RX_len = USB_PMA_COUNT_RX(ep) & 0x03ff;
      if(USB_RX_len > 64) USB_RX_len = 64;
      if(!ep) {
        if(USB_RX_len > EP0_BUF_SIZE) USB_RX_len = EP0_BUF_SIZE;
        USB_PMA_read(USB_PMA_RX(0), EP0_buffer, USB_RX_len);
        USB_EP_clearRX(0);
        if(epr & USBD_EP_SETUP) {
          if(epr & USBD_EP_CTR_TX) {          // discard pending IN completion
            USB_EP_clearTX(0);
            epr &= ~USBD_EP_CTR_TX;
          }
          USB_EP0_SETUP();
        }
        else USB_EP0_OUT();
      }
      else if(ep > USB_EP_MAX) USB_EP_clearRX(ep);
      else {
        if(USB_EP_OUTbuf[ep]) USB_PMA_read(USB_PMA_RX(ep), USB_EP_OUTbuf[ep], USB_RX_len);
        USB_EP_clearRX(ep);                   // STAT_RX is NAK now
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
          default: USB_EP_receive(ep); break;
        }
      }
    }

    // IN transaction completed
    if(epr & USBD_EP_CTR_TX) {
      USB_EP_clearTX(ep);                     // STAT_TX is NAK now
      if(ep > USB_EP_MAX) continue;
      switch(ep) {
        case 0: USB_EP0_IN(); break;
        #ifdef EP1_IN_callback
        case 1: EP1_IN_callback(); break;
        #endif
        #ifdef EP2_IN_callback
        case 2: EP2_IN_callback(); break;
        #endif
        #ifdef EP3_IN_callback
        case 3: EP3_IN_callback(); break;
        #endif
        default: break;
      }
    }
  }

  // USB bus reset event interrupt
  if(istr & USBD_ISTR_RESET) {
    USBD->ISTR = (uint16_t)~USBD_ISTR_RESET;
    #ifdef USB_RESET_handler
    USB_RESET_handler();
    #endif
    USB_EP_reset();
  }

  // USB bus suspend event interrupt
  if(istr & USBD_ISTR_SUSP) {
    USBD->ISTR = (uint16_t)~USBD_ISTR_SUSP;
    #ifdef USB_SUSPEND_handler
    USB_SUSPEND_handler();
    #endif
  }

  // USB bus wakeup event interrupt
  if(istr & USBD_ISTR_WKUP) {
    USBD->ISTR = (uint16_t)~USBD_ISTR_WKUP;
  }
}
