// ===================================================================================
// USB Handler for SAMD11                                                     * v1.0 *
// ===================================================================================
//
// USB full-speed device using the USB peripheral (pins PA24 = D-, PA25 = D+) with
// internal pull-up resistor. The USB module is clocked with 48MHz by the DFLL48M,
// which should run in USB clock recovery mode (SYS_USB_CRM in system.h) or in closed
// loop mode with an external 32.768kHz crystal (SYS_USE_XOSC32K).
//
// The data toggles are handled automatically by the hardware. The endpoint buffers
// must be located in SRAM (the USB DMA cannot read the flash), descriptors are copied
// into the EP0 buffer. If an endpoint has OUT and IN enabled, the IN buffer starts at
// offset 64 (see usb_descr.h). The buffers must be 4-byte aligned.
//
// All direct accesses to the USB peripheral are made exclusively in this handler.
// The class drivers only use the endpoint functions USB_EP_init(), USB_EP_send(),
// USB_EP_receive(), USB_EP_hold() and the variable USB_RX_len.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_handler.h"

// ===================================================================================
// Variables
// ===================================================================================
volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config, USB_Addr, USB_ENUM_OK;
volatile uint16_t USB_SetupLen;
volatile uint8_t  USB_RX_len;
const uint8_t*    USB_pDescr;
static uint8_t    USB_EP0_zlp;                // zero-length packet required at the end

// ===================================================================================
// Endpoint Descriptor Table and Register Access
// ===================================================================================

// Number of endpoints used (highest endpoint number + 1)
#if   defined(EP7_SIZE)
  #define USB_EP_NUM      8
#elif defined(EP6_SIZE)
  #define USB_EP_NUM      7
#elif defined(EP5_SIZE)
  #define USB_EP_NUM      6
#elif defined(EP4_SIZE)
  #define USB_EP_NUM      5
#elif defined(EP3_SIZE)
  #define USB_EP_NUM      4
#elif defined(EP2_SIZE)
  #define USB_EP_NUM      3
#else
  #define USB_EP_NUM      2
#endif

// Endpoint descriptor table in SRAM (bank 0: OUT, bank 1: IN)
static UsbDeviceDescriptor USB_EP_table[USB_EP_NUM] __attribute__((aligned(4)));

#define USB_EP(n)           (USB->DEVICE.DeviceEndpoint[n])
#define USB_BANK_OUT(n)     (USB_EP_table[n].DeviceDescBank[0])
#define USB_BANK_IN(n)      (USB_EP_table[n].DeviceDescBank[1])

// Packet size code (0: 8, 1: 16, 2: 32, 3: 64 bytes)
#define USB_SIZE_CODE(s)    ((s) <= 8 ? 0 : (s) <= 16 ? 1 : (s) <= 32 ? 2 : 3)
#define USB_PCKSIZE_64      USB_DEVICE_PCKSIZE_SIZE(3)
#define USB_PCKSIZE_EP0     USB_DEVICE_PCKSIZE_SIZE(USB_SIZE_CODE(EP0_SIZE))

// ===================================================================================
// Endpoint Functions (also used by the class drivers)
// ===================================================================================

// Setup endpoint (ep: 1..7, buf: endpoint buffer, out/in: enable direction)
void USB_EP_init(uint8_t ep, uint8_t type, uint8_t* buf, uint8_t out, uint8_t in) {
  uint8_t cfg = 0;
  if(!ep || (ep >= USB_EP_NUM)) return;         // EP0 and unused EPs not supported
  USB_EP(ep).EPCFG.reg = 0;                     // disable endpoint
  if(out) {                                     // OUT: buffer at offset 0, ACK
    USB_BANK_OUT(ep).ADDR.reg    = (uint32_t)buf;
    USB_BANK_OUT(ep).PCKSIZE.reg = USB_PCKSIZE_64 | USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(64);
    cfg |= USB_DEVICE_EPCFG_EPTYPE0((type & 3) + 1);
  }
  if(in) {                                      // IN: buffer at offset 64 if OUT is used
    USB_BANK_IN(ep).ADDR.reg     = (uint32_t)(out ? buf + 64 : buf);
    USB_BANK_IN(ep).PCKSIZE.reg  = USB_PCKSIZE_64;
    cfg |= USB_DEVICE_EPCFG_EPTYPE1((type & 3) + 1);
  }
  USB_EP(ep).EPCFG.reg       = cfg;             // set endpoint types
  USB_EP(ep).EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_DTGLOUT // DATA0
                             | USB_DEVICE_EPSTATUSCLR_DTGLIN
                             | USB_DEVICE_EPSTATUSCLR_BK0RDY  // OUT: ready (ACK)
                             | USB_DEVICE_EPSTATUSCLR_BK1RDY  // IN:  nothing (NAK)
                             | USB_DEVICE_EPSTATUSCLR_STALLRQ0
                             | USB_DEVICE_EPSTATUSCLR_STALLRQ1;
  USB_EP(ep).EPINTFLAG.reg   = USB_DEVICE_EPINTFLAG_TRCPT0 | USB_DEVICE_EPINTFLAG_TRCPT1;
  USB_EP(ep).EPINTENSET.reg  = (out ? USB_DEVICE_EPINTENSET_TRCPT0 : 0)
                             | (in  ? USB_DEVICE_EPINTENSET_TRCPT1 : 0);
}

// Transmit len bytes of endpoint IN buffer to host
void USB_EP_send(uint8_t ep, uint8_t len) {
  USB_BANK_IN(ep).PCKSIZE.reg = (ep ? USB_PCKSIZE_64 : USB_PCKSIZE_EP0)
                              | USB_DEVICE_PCKSIZE_BYTE_COUNT(len);
  USB_EP(ep).EPINTFLAG.reg    = USB_DEVICE_EPINTFLAG_TRCPT1;
  USB_EP(ep).EPSTATUSSET.reg  = USB_DEVICE_EPSTATUSSET_BK1RDY;
}

// Ready to receive next OUT packet (ACK)
void USB_EP_receive(uint8_t ep) {
  USB_BANK_OUT(ep).PCKSIZE.reg = (ep ? USB_PCKSIZE_64 : USB_DEVICE_PCKSIZE_SIZE(3))
                               | USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(64);
  USB_EP(ep).EPSTATUSCLR.reg   = USB_DEVICE_EPSTATUSCLR_BK0RDY;
}

// Do not receive OUT packets (NAK)
void USB_EP_hold(uint8_t ep) {
  USB_EP(ep).EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_BK0RDY;
}

// Set (stall=1) or clear (stall=0) endpoint halt feature (addr: endpoint address)
static uint8_t USB_EP_halt(uint8_t addr, uint8_t stall) {
  uint8_t ep = addr & 0x0f;
  if(!ep || (ep >= USB_EP_NUM)) return 0;
  if(addr & 0x80) {
    if(stall) USB_EP(ep).EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_STALLRQ1;
    else {
      USB_EP(ep).EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_STALLRQ1  // clear stall
                                 | USB_DEVICE_EPSTATUSCLR_DTGLIN;   // DATA0
      USB_EP(ep).EPINTFLAG.reg   = USB_DEVICE_EPINTFLAG_STALL1;
    }
  }
  else {
    if(stall) USB_EP(ep).EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_STALLRQ0;
    else {
      USB_EP(ep).EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_STALLRQ0  // clear stall
                                 | USB_DEVICE_EPSTATUSCLR_DTGLOUT;  // DATA0
      USB_EP(ep).EPINTFLAG.reg   = USB_DEVICE_EPINTFLAG_STALL0;
    }
  }
  return 1;
}

// Check if endpoint is halted
static uint8_t USB_EP_isHalted(uint8_t addr) {
  uint8_t ep = addr & 0x0f;
  if(!ep || (ep >= USB_EP_NUM)) return 0;
  if(addr & 0x80) return (USB_EP(ep).EPSTATUS.reg & USB_DEVICE_EPSTATUS_STALLRQ1) ? 1 : 0;
  return (USB_EP(ep).EPSTATUS.reg & USB_DEVICE_EPSTATUS_STALLRQ0) ? 1 : 0;
}

// ===================================================================================
// Setup/Reset Endpoints
// ===================================================================================
static void USB_EP_reset(void) {
  uint8_t i;

  // Disable all endpoints
  for(i = 0; i < 8; i++) USB_EP(i).EPCFG.reg = 0;
  for(i = 0; i < sizeof(USB_EP_table); i++) ((uint8_t*)USB_EP_table)[i] = 0;

  // Setup endpoint 0 (control): OUT and IN use EP0_buffer
  USB_BANK_OUT(0).ADDR.reg    = (uint32_t)EP0_buffer;
  USB_BANK_OUT(0).PCKSIZE.reg = USB_DEVICE_PCKSIZE_SIZE(3)
                              | USB_DEVICE_PCKSIZE_MULTI_PACKET_SIZE(64);
  USB_BANK_IN(0).ADDR.reg     = (uint32_t)EP0_buffer;
  USB_BANK_IN(0).PCKSIZE.reg  = USB_PCKSIZE_EP0;
  USB_EP(0).EPCFG.reg         = USB_DEVICE_EPCFG_EPTYPE0(1)   // control OUT
                              | USB_DEVICE_EPCFG_EPTYPE1(1);  // control IN
  USB_EP(0).EPSTATUSSET.reg   = USB_DEVICE_EPSTATUSSET_BK0RDY;// OUT: NAK (SETUP: ACK)
  USB_EP(0).EPSTATUSCLR.reg   = USB_DEVICE_EPSTATUSCLR_BK1RDY;// IN:  NAK
  USB_EP(0).EPINTENSET.reg    = USB_DEVICE_EPINTENSET_RXSTP   // SETUP received
                              | USB_DEVICE_EPINTENSET_TRCPT0  // OUT completed
                              | USB_DEVICE_EPINTENSET_TRCPT1; // IN completed

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
  uint32_t cal, transn, transp, trim;

  // Setup USB pins PA24 (D-) and PA25 (D+), peripheral function G
  PORT->Group[0].PMUX[24 >> 1].reg = PORT_PMUX_PMUXE(6) | PORT_PMUX_PMUXO(6);
  PORT->Group[0].PINCFG[24].reg    = PORT_PINCFG_PMUXEN;
  PORT->Group[0].PINCFG[25].reg    = PORT_PINCFG_PMUXEN;

  // Enable USB module clocks (48MHz from DFLL48M)
  PM->AHBMASK.reg  |= PM_AHBMASK_USB;
  PM->APBBMASK.reg |= PM_APBBMASK_USB;
  #if F_CPU == 48000000
  GCLK_setCH(USB_GCLK_ID, GCLK_SYS);            // GCLK0 (48MHz) -> USB
  #else
  GCLK_setGEN(GCLK_48M, GCLK_SOURCE_DFLL48M, 1);// GCLK4 = DFLL48M (48MHz)
  GCLK_setCH(USB_GCLK_ID, GCLK_48M);            // GCLK4 -> USB
  #endif

  // Reset USB module
  USB->DEVICE.CTRLA.reg = USB_CTRLA_SWRST;
  while(USB->DEVICE.SYNCBUSY.reg & USB_SYNCBUSY_SWRST);

  // Load pad calibration values from NVM (use defaults if not programmed)
  cal    = *(uint32_t*)USB_FUSES_TRANSN_ADDR;
  transn = (cal & USB_FUSES_TRANSN_Msk) >> USB_FUSES_TRANSN_Pos;
  transp = (cal & USB_FUSES_TRANSP_Msk) >> USB_FUSES_TRANSP_Pos;
  trim   = (cal & USB_FUSES_TRIM_Msk)   >> USB_FUSES_TRIM_Pos;
  if(transn == 0x1f) transn = 5;
  if(transp == 0x1f) transp = 29;
  if(trim   == 0x07) trim   = 3;
  USB->DEVICE.PADCAL.reg = USB_PADCAL_TRANSN(transn) | USB_PADCAL_TRANSP(transp)
                         | USB_PADCAL_TRIM(trim);

  // Setup USB device (full-speed, detached during configuration)
  USB->DEVICE.DESCADD.reg = (uint32_t)USB_EP_table;   // endpoint descriptor table
  USB->DEVICE.CTRLB.reg   = USB_DEVICE_CTRLB_DETACH;  // full-speed, detached
  USB->DEVICE.CTRLA.reg   = USB_CTRLA_MODE_DEVICE | USB_CTRLA_RUNSTDBY;
  USB_EP_reset();
  USB->DEVICE.INTFLAG.reg  = 0xffff;                  // clear all interrupt flags
  USB->DEVICE.INTENSET.reg = USB_DEVICE_INTENSET_EORST     // end of bus reset
                           | USB_DEVICE_INTENSET_SUSPEND;  // bus suspend
  USB->DEVICE.CTRLA.reg   |= USB_CTRLA_ENABLE;        // enable USB module
  while(USB->DEVICE.SYNCBUSY.reg & USB_SYNCBUSY_ENABLE);
  NVIC_EnableIRQ(USB_IRQn);                           // enable USB interrupt
  USB->DEVICE.CTRLB.reg  &= ~USB_DEVICE_CTRLB_DETACH; // attach (internal pull-up)
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

// Send IN packet on EP0 (len bytes from EP0_buffer, len = 0: ZLP) and prepare OUT bank
// for the status stage (or the next data packet)
static void USB_EP0_sendIN(uint8_t len) {
  USB_EP_receive(0);                          // OUT: ready (status stage)
  USB_EP_send(0, len);                        // IN:  send packet
}

// Endpoint 0 SETUP handler
static void USB_EP0_SETUP(void) {
  uint8_t len = 0;
  USB_EP0_zlp  = 0;
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
          if(USB_SetupLen > len) {            // descriptor shorter than requested?
            USB_SetupLen = len;
            USB_EP0_zlp  = !(len & (EP0_SIZE - 1)); // multiple of EP0_SIZE -> ZLP
          }
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
    USB_EP(0).EPSTATUSSET.reg = USB_DEVICE_EPSTATUSSET_STALLRQ0
                              | USB_DEVICE_EPSTATUSSET_STALLRQ1;
  }
  else if(USB_SetupTyp & USB_REQ_TYP_IN) {    // data stage IN
    USB_SetupLen -= len;
    USB_EP0_sendIN(len);
  }
  else if(USB_SetupLen) {                     // data stage OUT
    USB_EP_receive(0);
  }
  else {                                      // no data stage: status IN (ZLP)
    USB_EP0_sendIN(0);
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

  if((USB_SetupTyp & USB_REQ_TYP_IN) && (USB_SetupReq == USB_GET_DESCRIPTOR)) {
    if(!USB_SetupLen) {                       // all data sent?
      if(USB_EP0_zlp) {                       // terminate transfer with ZLP
        USB_EP0_zlp = 0;
        USB_EP_send(0, 0);
      }
      return;
    }
    len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
    USB_EP0_copyDescr(len);                   // next packet of descriptor
    USB_SetupLen -= len;
    USB_EP_send(0, len);
    return;
  }

  if(USB_SetupReq == USB_SET_ADDRESS) {       // status stage of SET_ADDRESS completed
    USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN | USB_DEVICE_DADD_DADD(USB_Addr);
    USB_SetupReq = 0xff;
  }
}

// Endpoint 0 OUT handler
static void USB_EP0_OUT(void) {
  if(USB_SetupTyp & USB_REQ_TYP_IN) return;   // status stage of IN transfer completed

  #ifdef USB_CLASS_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) USB_CLASS_OUT_handler();
  #endif

  #ifdef USB_VENDOR_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) USB_VENDOR_OUT_handler();
  #endif

  USB_SetupLen = 0;                           // only single packet data stages
  USB_EP_send(0, 0);                          // status stage: send ZLP
}

// ===================================================================================
// USB Interrupt Service Routine
// ===================================================================================
void USB_IRQHandler(void) {
  uint16_t intflag = USB->DEVICE.INTFLAG.reg;
  uint16_t epints  = USB->DEVICE.EPINTSMRY.reg;
  uint8_t  ep, flags;

  // USB bus reset event interrupt
  if(intflag & USB_DEVICE_INTFLAG_EORST) {
    USB->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_EORST;
    #ifdef USB_RESET_handler
    USB_RESET_handler();
    #endif
    USB->DEVICE.DADD.reg = USB_DEVICE_DADD_ADDEN;     // address 0
    USB_EP_reset();
    return;
  }

  // USB bus suspend event interrupt
  if(intflag & USB_DEVICE_INTFLAG_SUSPEND) {
    USB->DEVICE.INTFLAG.reg = USB_DEVICE_INTFLAG_SUSPEND;
    #ifdef USB_SUSPEND_handler
    USB_SUSPEND_handler();
    #endif
  }

  // Endpoint 0 (control)
  if(epints & 1) {
    flags = USB_EP(0).EPINTFLAG.reg;

    // SETUP packet received
    if(flags & USB_DEVICE_EPINTFLAG_RXSTP) {
      USB_EP(0).EPINTFLAG.reg   = USB_DEVICE_EPINTFLAG_RXSTP   // clear flags
                                | USB_DEVICE_EPINTFLAG_TRCPT0
                                | USB_DEVICE_EPINTFLAG_TRCPT1;
      USB_EP(0).EPSTATUSCLR.reg = USB_DEVICE_EPSTATUSCLR_STALLRQ0  // clear stall
                                | USB_DEVICE_EPSTATUSCLR_STALLRQ1
                                | USB_DEVICE_EPSTATUSCLR_BK1RDY;   // cancel pending IN
      USB_EP0_SETUP();
    }
    else {
      // OUT packet received (data or status stage)
      if(flags & USB_DEVICE_EPINTFLAG_TRCPT0) {
        USB_EP(0).EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
        USB_RX_len = USB_BANK_OUT(0).PCKSIZE.reg & USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk;
        USB_EP0_OUT();                        // OUT bank stays busy (NAK)
      }

      // IN packet sent (data or status stage)
      if(flags & USB_DEVICE_EPINTFLAG_TRCPT1) {
        USB_EP(0).EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
        USB_EP0_IN();
      }
    }
  }

  // Endpoints 1..7
  for(ep = 1; ep < USB_EP_NUM; ep++) {
    if(!(epints & ((uint16_t)1 << ep))) continue;
    flags = USB_EP(ep).EPINTFLAG.reg;

    // IN packet sent (bank 1 not ready -> NAK until next USB_EP_send())
    if(flags & USB_DEVICE_EPINTFLAG_TRCPT1) {
      USB_EP(ep).EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT1;
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
        #ifdef EP6_IN_callback
        case 6: EP6_IN_callback(); break;
        #endif
        #ifdef EP7_IN_callback
        case 7: EP7_IN_callback(); break;
        #endif
        default: break;
      }
    }

    // OUT packet received (bank 0 busy -> NAK until USB_EP_receive())
    if(flags & USB_DEVICE_EPINTFLAG_TRCPT0) {
      USB_EP(ep).EPINTFLAG.reg = USB_DEVICE_EPINTFLAG_TRCPT0;
      USB_RX_len = USB_BANK_OUT(ep).PCKSIZE.reg & USB_DEVICE_PCKSIZE_BYTE_COUNT_Msk;
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
        #ifdef EP6_OUT_callback
        case 6: EP6_OUT_callback(); break;
        #endif
        #ifdef EP7_OUT_callback
        case 7: EP7_OUT_callback(); break;
        #endif
        default: USB_EP_receive(ep); break;
      }
    }

    // Clear other flags (e.g. STALL, TRFAIL)
    USB_EP(ep).EPINTFLAG.reg = flags & ~(USB_DEVICE_EPINTFLAG_TRCPT0
                                       | USB_DEVICE_EPINTFLAG_TRCPT1);
  }
}
