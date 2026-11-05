// ===================================================================================
// USB Handler for CH32X035/X034/X033                                         * v1.3 *
// ===================================================================================
//
// USB full-speed device using the USBFS peripheral (pins PC16 = D-, PC17 = D+) with
// internal pull-up resistor (1.5k for 3.3V supply, 10k for 5V supply).
//
// The data toggles of endpoints 1..7 are handled automatically by the hardware
// (auto toggle mode). Endpoint buffers are used in DMA mode, if an endpoint has OUT
// and IN enabled, the IN buffer starts at offset 64 (see usb_descr.h). The buffers
// must be 4-byte aligned. Endpoint 4 is not supported, since it has no own DMA
// address register (its buffer follows the buffer of endpoint 0).
//
// All direct accesses to the USB peripheral are made exclusively in this handler.
// The class drivers only use the endpoint functions USB_EP_init(), USB_EP_send(),
// USB_EP_receive(), USB_EP_hold() and the variable USB_RX_len.
//
// Changes for the debug probe: support of a BOS descriptor (USB_BOS_DESCR), of
// multi-packet IN data stages of vendor requests (MS OS 2.0 descriptor) and
// USB_EP_abort().
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

// ===================================================================================
// Endpoint Register Access
// ===================================================================================
// The registers of endpoints 0..4 and 5..7 are located in two separate blocks
// (DMA: EP0..3 and EP5..7, CTRL/TX_LEN: EP0..4 and EP5..7, 32 bits per endpoint).
static __IO uint32_t* USB_EP_dmaReg(uint8_t ep) {
  return (ep < 4) ? (&USBFSD->UEP0_DMA + ep) : (&USBFSD->UEP5_DMA + (ep - 5));
}

static __IO uint16_t* USB_EP_ctrlReg(uint8_t ep) {
  return (ep < 5) ? (&USBFSD->UEP0_CTRL_H + 2*ep) : (&USBFSD->UEP5_CTRL_H + 2*(ep - 5));
}

static __IO uint16_t* USB_EP_lenReg(uint8_t ep) {
  return (ep < 5) ? (&USBFSD->UEP0_TX_LEN + 2*ep) : (&USBFSD->UEP5_TX_LEN + 2*(ep - 5));
}

#define USB_UEP_CTRL(n)     (*USB_EP_ctrlReg(n))
#define USB_UEP_T_LEN(n)    (*USB_EP_lenReg(n))

// Set IN (T) or OUT (R) response of endpoint n
#define USB_UEP_T_RES(n, r) (USB_UEP_CTRL(n) = (USB_UEP_CTRL(n) & ~USBFS_UEP_T_RES_MASK) | (r))
#define USB_UEP_R_RES(n, r) (USB_UEP_CTRL(n) = (USB_UEP_CTRL(n) & ~USBFS_UEP_R_RES_MASK) | (r))

// ===================================================================================
// Endpoint Functions (also used by the class drivers)
// ===================================================================================

// Setup endpoint (ep: 1..3, 5..7, buf: endpoint buffer, out/in: enable direction)
void USB_EP_init(uint8_t ep, uint8_t type, uint8_t* buf, uint8_t out, uint8_t in) {
  __IO uint8_t* mod;
  uint8_t mask, bits;
  (void)type;                                   // type is defined by descriptor only

  // Endpoint mode registers: EP1/4, EP2/3, EP5/6/7 (different bit positions)
  switch(ep) {
    case 1:  mod = &USBFSD->UEP4_1_MOD; mask = USBFS_UEP1_RX_EN | USBFS_UEP1_TX_EN | USBFS_UEP1_BUF_MOD;
             bits = (out ? USBFS_UEP1_RX_EN : 0) | (in ? USBFS_UEP1_TX_EN : 0); break;
    case 2:  mod = &USBFSD->UEP2_3_MOD; mask = USBFS_UEP2_RX_EN | USBFS_UEP2_TX_EN | USBFS_UEP2_BUF_MOD;
             bits = (out ? USBFS_UEP2_RX_EN : 0) | (in ? USBFS_UEP2_TX_EN : 0); break;
    case 3:  mod = &USBFSD->UEP2_3_MOD; mask = USBFS_UEP3_RX_EN | USBFS_UEP3_TX_EN | USBFS_UEP3_BUF_MOD;
             bits = (out ? USBFS_UEP3_RX_EN : 0) | (in ? USBFS_UEP3_TX_EN : 0); break;
    case 5:  mod = &USBFSD->UEP567_MOD; mask = USBFS_UEP5_RX_EN | USBFS_UEP5_TX_EN;
             bits = (out ? USBFS_UEP5_RX_EN : 0) | (in ? USBFS_UEP5_TX_EN : 0); break;
    case 6:  mod = &USBFSD->UEP567_MOD; mask = USBFS_UEP6_RX_EN | USBFS_UEP6_TX_EN;
             bits = (out ? USBFS_UEP6_RX_EN : 0) | (in ? USBFS_UEP6_TX_EN : 0); break;
    case 7:  mod = &USBFSD->UEP567_MOD; mask = USBFS_UEP7_RX_EN | USBFS_UEP7_TX_EN;
             bits = (out ? USBFS_UEP7_RX_EN : 0) | (in ? USBFS_UEP7_TX_EN : 0); break;
    default: return;                            // EP0, EP4 and EP8..15 not supported
  }

  *mod = (*mod & ~mask) | bits;                 // enable directions
  *USB_EP_dmaReg(ep) = (uint32_t)buf;           // buffer address (IN at +64 if OUT used)
  USB_UEP_T_LEN(ep)  = 0;                       // nothing to send
  USB_UEP_CTRL(ep)   = USBFS_UEP_AUTO_TOG       // auto toggle, DATA0
                     | USBFS_UEP_T_RES_NAK      // IN: NAK
                     | USBFS_UEP_R_RES_ACK;     // OUT: ACK
}

// Transmit len bytes of endpoint IN buffer to host
void USB_EP_send(uint8_t ep, uint8_t len) {
  USB_UEP_T_LEN(ep) = len;
  USB_UEP_T_RES(ep, USBFS_UEP_T_RES_ACK);
}

// Ready to receive next OUT packet (ACK)
void USB_EP_receive(uint8_t ep) {
  USB_UEP_R_RES(ep, USBFS_UEP_R_RES_ACK);
}

// Do not receive OUT packets (NAK)
void USB_EP_hold(uint8_t ep) {
  USB_UEP_R_RES(ep, USBFS_UEP_R_RES_NAK);
}

// Cancel pending IN transfer (NAK)
void USB_EP_abort(uint8_t ep) {
  USB_UEP_T_RES(ep, USBFS_UEP_T_RES_NAK);
  USB_UEP_T_LEN(ep) = 0;
}

// Set (stall=1) or clear (stall=0) endpoint halt feature (addr: endpoint address)
static uint8_t USB_EP_halt(uint8_t addr, uint8_t stall) {
  uint8_t ep = addr & 0x0f;
  if(!ep || (ep == 4) || (ep > 7)) return 0;
  if(addr & 0x80) {
    if(stall) USB_UEP_T_RES(ep, USBFS_UEP_T_RES_STALL);
    else      USB_UEP_CTRL(ep) = (USB_UEP_CTRL(ep) & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK))
                               | USBFS_UEP_T_RES_NAK;     // DATA0, NAK
  }
  else {
    if(stall) USB_UEP_R_RES(ep, USBFS_UEP_R_RES_STALL);
    else      USB_UEP_CTRL(ep) = (USB_UEP_CTRL(ep) & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK))
                               | USBFS_UEP_R_RES_ACK;     // DATA0, ACK
  }
  return 1;
}

// Check if endpoint is halted
static uint8_t USB_EP_isHalted(uint8_t addr) {
  uint8_t ep = addr & 0x0f;
  if(!ep || (ep == 4) || (ep > 7)) return 0;
  if(addr & 0x80) return (USB_UEP_CTRL(ep) & USBFS_UEP_T_RES_MASK) == USBFS_UEP_T_RES_STALL;
  return (USB_UEP_CTRL(ep) & USBFS_UEP_R_RES_MASK) == USBFS_UEP_R_RES_STALL;
}

// ===================================================================================
// Setup/Reset Endpoints
// ===================================================================================
static void USB_EP_reset(void) {
  USBFSD->UEP0_DMA    = (uint32_t)EP0_buffer;
  USBFSD->UEP0_TX_LEN = 0;
  USBFSD->UEP0_CTRL_H = USBFS_UEP_T_RES_NAK | USBFS_UEP_R_RES_ACK;

  #ifdef USB_INIT_endpoints
  USB_INIT_endpoints();                     // custom EP init handler
  #endif

  USB_ENUM_OK = 0;
  USB_Config  = 0;
  USB_Addr    = 0;
}

// ===================================================================================
// USB Init Function
// ===================================================================================
void USB_init(void) {
  // Enable USB I/O (PC16 = D-, PC17 = D+) and USBFS module
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  RCC->AHBPCENR  |= RCC_USBFS;
  GPIOC->CFGXR    = (GPIOC->CFGXR & ~((uint32_t)0b11111111)) | ((uint32_t)0b10000100);
  GPIOC->BSXR     = ((uint32_t)1<<1);

  // Select supply voltage and internal pull-up resistor
  #ifdef USB_VDD
    #if USB_VDD > 0
      AFIO->CTLR = (AFIO->CTLR & ~(AFIO_CTLR_UDP_PUE | AFIO_CTLR_UDM_PUE | AFIO_CTLR_USB_PHY_V33))
                 | AFIO_CTLR_UDP_PUE_10K | AFIO_CTLR_USB_IOEN;
    #else
      AFIO->CTLR = (AFIO->CTLR & ~(AFIO_CTLR_UDP_PUE | AFIO_CTLR_UDM_PUE ))
                 | AFIO_CTLR_USB_PHY_V33 | AFIO_CTLR_UDP_PUE_1K5 | AFIO_CTLR_USB_IOEN;
    #endif
  #else
    // Detect supply voltage with PVD (4.0V level), as in WCH SDK
    uint32_t pls;
    RCC->APB1PCENR |= RCC_PWREN;
    pls = PWR->CTLR & PWR_CTLR_PLS;           // save current PVD level
    PWR->CTLR |= PWR_CTLR_PLS;                // set PVD level to 4.0V
    DLY_us(10);                               // wait for PVD to settle
    if(PWR->CSR & PWR_CSR_PVDO)
      AFIO->CTLR = (AFIO->CTLR & ~(AFIO_CTLR_UDP_PUE | AFIO_CTLR_UDM_PUE ))
                 | AFIO_CTLR_USB_PHY_V33 | AFIO_CTLR_UDP_PUE_1K5 | AFIO_CTLR_USB_IOEN;
    else
      AFIO->CTLR = (AFIO->CTLR & ~(AFIO_CTLR_UDP_PUE | AFIO_CTLR_UDM_PUE | AFIO_CTLR_USB_PHY_V33))
                 | AFIO_CTLR_UDP_PUE_10K | AFIO_CTLR_USB_IOEN;
    PWR->CTLR = (PWR->CTLR & ~PWR_CTLR_PLS) | pls;  // restore PVD level
  #endif

  // Reset and setup USBFS device
  USBFSD->BASE_CTRL = USBFS_UC_RESET_SIE | USBFS_UC_CLR_ALL;
  DLY_us(10);
  USBFSD->BASE_CTRL = 0x00;
  USB_EP_reset();
  USBFSD->DEV_ADDR  = 0x00;
  USBFSD->BASE_CTRL = USBFS_UC_DEV_PU_EN | USBFS_UC_INT_BUSY | USBFS_UC_DMA_EN;
  USBFSD->INT_FG    = 0xff;
  USBFSD->UDEV_CTRL = USBFS_UD_PD_DIS | USBFS_UD_PORT_EN;
  USBFSD->INT_EN    = USBFS_UIE_SUSPEND | USBFS_UIE_BUS_RST | USBFS_UIE_TRANSFER;
  NVIC_EnableIRQ(USBFS_IRQn);
}

// ===================================================================================
// Endpoint Copy Function
// ===================================================================================
// Copy descriptor *USB_pDescr to Ep0
void USB_EP0_copyDescr(uint8_t len) {
  uint8_t* tgt = EP0_buffer;
  while(len--) *tgt++ = *USB_pDescr++;
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
            len = USB_pDescr[0];
            break;

          #ifdef USB_BOS_DESCR
          case USB_DESCR_TYP_BOS:
            USB_pDescr = USB_BOS_DESCR;
            len = USB_BOS_DESCR_LEN;
            break;
          #endif

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
    USBFSD->UEP0_CTRL_H = USBFS_UEP_R_TOG | USBFS_UEP_T_TOG | USBFS_UEP_R_RES_STALL | USBFS_UEP_T_RES_STALL;
  }
  else if(USB_SetupTyp & USB_REQ_TYP_IN) {    // data stage IN (DATA1 first)
    USB_SetupLen -= len;
    USBFSD->UEP0_TX_LEN = len;
    USBFSD->UEP0_CTRL_H = USBFS_UEP_R_TOG | USBFS_UEP_T_TOG | USBFS_UEP_R_RES_ACK | USBFS_UEP_T_RES_ACK;
  }
  else if(USB_SetupLen) {                     // data stage OUT (DATA1 first)
    USBFSD->UEP0_CTRL_H = USBFS_UEP_R_TOG | USBFS_UEP_T_TOG | USBFS_UEP_R_RES_ACK | USBFS_UEP_T_RES_NAK;
  }
  else {                                      // no data stage: status IN (ZLP)
    USBFSD->UEP0_TX_LEN = 0;
    USBFSD->UEP0_CTRL_H = USBFS_UEP_R_TOG | USBFS_UEP_T_TOG | USBFS_UEP_R_RES_ACK | USBFS_UEP_T_RES_ACK;
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

  #ifdef USB_VENDOR_IN_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) {
    USB_VENDOR_IN_handler();
    return;
  }
  #endif

  if((USB_SetupTyp & USB_REQ_TYP_IN) && ((USB_SetupReq == USB_GET_DESCRIPTOR)
     || ((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR))) {
    len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
    USB_EP0_copyDescr(len);                   // next packet of descriptor
    USB_SetupLen -= len;
    USBFSD->UEP0_TX_LEN  = len;
    USBFSD->UEP0_CTRL_H ^= USBFS_UEP_T_TOG;
    return;
  }

  if(USB_SetupReq == USB_SET_ADDRESS)         // status stage of SET_ADDRESS completed
    USBFSD->DEV_ADDR = (USBFSD->DEV_ADDR & USBFS_UDA_GP_BIT) | USB_Addr;

  USBFSD->UEP0_CTRL_H = USBFS_UEP_T_RES_NAK | USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK; // transfer completed
}

// Endpoint 0 OUT handler
static void USB_EP0_OUT(void) {
  if(USB_SetupTyp & USB_REQ_TYP_IN) {         // status stage of IN transfer completed
    USBFSD->UEP0_CTRL_H = USBFS_UEP_T_RES_NAK | USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK;
    return;
  }

  #ifdef USB_CLASS_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) USB_CLASS_OUT_handler();
  #endif

  #ifdef USB_VENDOR_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) USB_VENDOR_OUT_handler();
  #endif

  USB_SetupLen = 0;                           // only single packet data stages
  USBFSD->UEP0_TX_LEN = 0;                    // status stage: send ZLP (DATA1)
  USBFSD->UEP0_CTRL_H = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK | USBFS_UEP_R_RES_NAK;
}

// ===================================================================================
// USB Interrupt Service Routine
// ===================================================================================
void USBFS_IRQHandler(void) __attribute__((interrupt));
void USBFS_IRQHandler(void) {
  uint8_t intflag = USBFSD->INT_FG;
  uint8_t intst   = USBFSD->INT_ST;

  // USB transfer completed interrupt
  if(intflag & USBFS_UIF_TRANSFER) {
    uint8_t ep = intst & USBFS_UIS_ENDP_MASK;
    switch(intst & USBFS_UIS_TOKEN_MASK) {

      case USBFS_UIS_TOKEN_SETUP:
        USB_EP0_SETUP();
        break;

      case USBFS_UIS_TOKEN_IN:
        if(!ep) {
          USB_EP0_IN();
          break;
        }
        if((ep == 4) || (ep > 7)) break;
        USB_UEP_T_RES(ep, USBFS_UEP_T_RES_NAK);   // NAK until next USB_EP_send()
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
          #ifdef EP5_IN_callback
          case 5: EP5_IN_callback(); break;
          #endif
          #ifdef EP6_IN_callback
          case 6: EP6_IN_callback(); break;
          #endif
          #ifdef EP7_IN_callback
          case 7: EP7_IN_callback(); break;
          #endif
          default: break;
        }
        break;

      case USBFS_UIS_TOKEN_OUT:
        if(!(intst & USBFS_UIS_TOG_OK)) break;    // ignore out-of-sync packets
        USB_RX_len = (uint8_t)USBFSD->RX_LEN;
        if(!ep) {
          USB_EP0_OUT();
          break;
        }
        if((ep == 4) || (ep > 7)) break;
        USB_UEP_R_RES(ep, USBFS_UEP_R_RES_NAK);   // NAK until USB_EP_receive()
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
          #ifdef EP5_OUT_callback
          case 5: EP5_OUT_callback(); break;
          #endif
          #ifdef EP6_OUT_callback
          case 6: EP6_OUT_callback(); break;
          #endif
          #ifdef EP7_OUT_callback
          case 7: EP7_OUT_callback(); break;
          #endif
          default: USB_EP_receive(ep); break;
        }
        break;

      default:
        break;
    }
    USBFSD->INT_FG = USBFS_UIF_TRANSFER;
  }

  // USB bus suspend or wakeup event interrupt
  if(intflag & USBFS_UIF_SUSPEND) {
    USBFSD->INT_FG = USBFS_UIF_SUSPEND;
    #ifdef USB_SUSPEND_handler
    if(USBFSD->MIS_ST & USBFS_UMS_SUSPEND) USB_SUSPEND_handler();
    #endif
  }

  // USB bus reset event interrupt
  if(intflag & USBFS_UIF_BUS_RST) {
    #ifdef USB_RESET_handler
    USB_RESET_handler();
    #endif
    USB_EP_reset();
    USBFSD->DEV_ADDR = 0;
    USBFSD->INT_FG   = 0xff;
  }
}
