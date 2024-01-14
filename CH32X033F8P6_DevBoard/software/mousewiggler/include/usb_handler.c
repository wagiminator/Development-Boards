// ===================================================================================
// USB Handler for CH32X035/X034/X033                                         * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usb_handler.h"

// ===================================================================================
// Variables
// ===================================================================================
volatile uint8_t  USB_SetupReq, USB_SetupTyp, USB_Config, USB_Addr, USB_ENUM_OK;
volatile uint16_t USB_SetupLen;
const uint8_t*    USB_pDescr;

// ===================================================================================
// Setup/Reset Endpoints
// ===================================================================================
void USB_EP_init(void) {
  USBFSD->UEP0_DMA    = (uint32_t)EP0_buffer;
  USBFSD->UEP0_CTRL_H = USBFS_UEP_R_RES_ACK 
                      | USBFS_UEP_T_RES_NAK;
  USBFSD->UEP0_TX_LEN = 0;

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
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  RCC->AHBPCENR  |= RCC_USBFS;
  GPIOC->CFGXR    = (GPIOC->CFGXR & ~((uint32_t)0b11111111)) | ((uint32_t)0b10000100);
  GPIOC->BSXR     = ((uint32_t)1<<1);

  #if USB_VDD > 0
  AFIO->CTLR = (AFIO->CTLR & ~(AFIO_CTLR_UDP_PUE | AFIO_CTLR_UDM_PUE | AFIO_CTLR_USB_PHY_V33)) 
             | AFIO_CTLR_UDP_PUE_10K | AFIO_CTLR_USB_IOEN;
  #else
  AFIO->CTLR = (AFIO->CTLR & ~(AFIO_CTLR_UDP_PUE | AFIO_CTLR_UDM_PUE )) 
             | AFIO_CTLR_USB_PHY_V33 | AFIO_CTLR_UDP_PUE_1K5 | AFIO_CTLR_USB_IOEN;
  #endif

  USBFSD->BASE_CTRL = 0x00;
  USB_EP_init();
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
  const uint8_t* src = USB_pDescr;
  uint8_t*       tgt = EP0_buffer;
  while(len--) *tgt++ = *src++;
}

// ===================================================================================
// Endpoint EP0 Handlers
// ===================================================================================

// Endpoint 0 SETUP handler
void USB_EP0_SETUP(void) {
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
            len = 0xff;
            break;
        }

        if(len != 0xff) {
          if(USB_SetupLen > len) USB_SetupLen = len;
          len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
          USB_EP0_copyDescr(len);
          USB_SetupLen -= len;
          USB_pDescr   += len;
        }
        break;

      case USB_SET_ADDRESS:
        USB_Addr = USB_SetupBuf->wValueL;
        break;

      case USB_GET_CONFIGURATION:
        EP0_buffer[0] = USB_Config;
        if(USB_SetupLen >= 1) len = 1;
        break;

      case USB_SET_CONFIGURATION:
        USB_Config  = USB_SetupBuf->wValueL;
        USB_ENUM_OK = 1;
        break;

      case USB_GET_INTERFACE:
        break;

      case USB_SET_INTERFACE:
        break;

      case USB_GET_STATUS:
        EP0_buffer[0] = 0x00;
        EP0_buffer[1] = 0x00;
        if(USB_SetupLen >= 2) len = 2;
        else len = USB_SetupLen;
        break;

      case USB_CLEAR_FEATURE:
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE ) {
          if(USB_SetupBuf->wValueL == 0x01) {
            if(((uint8_t*)&CfgDescr)[7] & 0x20) {
              // wake up
            }
            else len = 0xff;
          }
          else len = 0xff;
        }
        else if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {
          switch(USB_SetupBuf->wIndexL) {
            #ifdef EP1_OUT_callback
            case 0x01:
              USBFSD->UEP1_CTRL_H = (USBFSD->UEP1_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK))
                                  | USBFS_UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP1_IN_callback
            case 0x81:
              USBFSD->UEP1_CTRL_H = (USBFSD->UEP1_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                  | USBFS_UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP2_OUT_callback
            case 0x02:
              USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK)) 
                                  | USBFS_UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP2_IN_callback
            case 0x82:
              USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                  | USBFS_UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP3_OUT_callback
            case 0x03:
              USBFSD->UEP3_CTRL_H = (USBFSD->UEP3_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK))
                                  | USBFS_UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP3_IN_callback
            case 0x83:
              USBFSD->UEP3_CTRL_H = (USBFSD->UEP3_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                  | USBFS_UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP4_OUT_callback
            case 0x04:
              USBFSD->UEP4_CTRL_H = (USBFSD->UEP4_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK))
                                  | USBFS_UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP4_IN_callback
            case 0x84:
              USBFSD->UEP4_CTRL_H = (USBFSD->UEP4_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                  | USBFS_UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP5_OUT_callback
            case 0x05:
              USBFSD->UEP5_CTRL_H = (USBFSD->UEP5_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK))
                                  | USBFS_UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP5_IN_callback
            case 0x85:
              USBFSD->UEP5_CTRL_H = (USBFSD->UEP5_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                  | USBFS_UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP6_OUT_callback
            case 0x06:
              USBFSD->UEP6_CTRL_H = (USBFSD->UEP6_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK))
                                  | USBFS_UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP6_IN_callback
            case 0x86:
              USBFSD->UEP6_CTRL_H = (USBFSD->UEP6_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                  | USBFS_UEP_T_RES_NAK;
              break;
            #endif
            #ifdef EP7_OUT_callback
            case 0x07:
              USBFSD->UEP7_CTRL_H = (USBFSD->UEP7_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK))
                                  | USBFS_UEP_R_RES_ACK;
              break;
            #endif
            #ifdef EP7_IN_callback
            case 0x87:
              USBFSD->UEP7_CTRL_H = (USBFSD->UEP7_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                  | USBFS_UEP_T_RES_NAK;
              break;
            #endif
            default:
              len = 0xff;
              break;
          }
        }
        else len = 0xff;
        break;

      case USB_SET_FEATURE:
        if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE) {
          if(USB_SetupBuf->wValueL == 0x01) {
            if(!(((uint8_t*)&CfgDescr)[7] & 0x20)) len = 0xff;
          }
          else len = 0xff;
        }
        else if((USB_SetupTyp & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP) {
          if(USB_SetupBuf->wValueL == 0x00) {
            switch(USB_SetupBuf->wIndexL) {
              #ifdef EP1_OUT_callback
              case 0x01:
                USBFSD->UEP1_CTRL_H = (USBFSD->UEP1_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK)) 
                                    | USBFS_UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP1_IN_callback
              case 0x81:
                USBFSD->UEP1_CTRL_H = (USBFSD->UEP1_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                    | USBFS_UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP2_OUT_callback
              case 0x02:
                USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK)) 
                                    | USBFS_UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP2_IN_callback
              case 0x82:
                USBFSD->UEP2_CTRL_H = (USBFSD->UEP2_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                    | USBFS_UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP3_OUT_callback
              case 0x03:
                USBFSD->UEP3_CTRL_H = (USBFSD->UEP3_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK)) 
                                    | USBFS_UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP3_IN_callback
              case 0x83:
                USBFSD->UEP3_CTRL_H = (USBFSD->UEP3_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                    | USBFS_UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP4_OUT_callback
              case 0x04:
                USBFSD->UEP4_CTRL_H = (USBFSD->UEP4_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK)) 
                                    | USBFS_UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP4_IN_callback
              case 0x84:
                USBFSD->UEP4_CTRL_H = (USBFSD->UEP4_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                    | USBFS_UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP5_OUT_callback
              case 0x05:
                USBFSD->UEP5_CTRL_H = (USBFSD->UEP5_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK)) 
                                    | USBFS_UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP5_IN_callback
              case 0x85:
                USBFSD->UEP5_CTRL_H = (USBFSD->UEP5_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                    | USBFS_UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP6_OUT_callback
              case 0x06:
                USBFSD->UEP6_CTRL_H = (USBFSD->UEP6_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK)) 
                                    | USBFS_UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP6_IN_callback
              case 0x86:
                USBFSD->UEP6_CTRL_H = (USBFSD->UEP6_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                    | USBFS_UEP_T_RES_STALL;
                break;
              #endif
              #ifdef EP7_OUT_callback
              case 0x07:
                USBFSD->UEP7_CTRL_H = (USBFSD->UEP7_CTRL_H & ~(USBFS_UEP_R_TOG | USBFS_UEP_R_RES_MASK)) 
                                    | USBFS_UEP_R_RES_STALL;
                break;
              #endif
              #ifdef EP7_IN_callback
              case 0x87:
                USBFSD->UEP7_CTRL_H = (USBFSD->UEP7_CTRL_H & ~(USBFS_UEP_T_TOG | USBFS_UEP_T_RES_MASK)) 
                                    | USBFS_UEP_T_RES_STALL;
                break;
              #endif
              default:
                len = 0xff;
                break;
            }
          }
          else len = 0xff;
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

  if(len == 0xff) {
    USB_SetupReq = 0xff;
    USBFSD->UEP0_CTRL_H = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_STALL | USBFS_UEP_R_TOG | USBFS_UEP_R_RES_STALL;
  }
  else {
    USBFSD->UEP0_TX_LEN = len;
    USBFSD->UEP0_CTRL_H = USBFS_UEP_T_TOG | USBFS_UEP_T_RES_ACK | USBFS_UEP_R_TOG | USBFS_UEP_R_RES_ACK;
  }
}

// Endpoint 0 IN handler
void USB_EP0_IN(void) {
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

  switch(USB_SetupReq) {
    case USB_GET_DESCRIPTOR:
      len = USB_SetupLen >= EP0_SIZE ? EP0_SIZE : USB_SetupLen;
      USB_EP0_copyDescr(len);
      USB_SetupLen -= len;
      USB_pDescr   += len;
      USBFSD->UEP0_TX_LEN = len;
      USBFSD->UEP0_CTRL_H ^= USBFS_UEP_T_TOG;
      break;

    case USB_SET_ADDRESS:
      USBFSD->DEV_ADDR    = (USBFSD->DEV_ADDR & USBFS_UDA_GP_BIT) | USB_Addr;
      USBFSD->UEP0_CTRL_H = USBFS_UEP_T_RES_NAK | USBFS_UEP_R_RES_ACK;
      break;

    default:
      USBFSD->UEP0_CTRL_H = USBFS_UEP_T_RES_NAK | USBFS_UEP_R_RES_ACK;
      break;
  }
}

// Endpoint 0 OUT handler
void USB_EP0_OUT(void) {
  #ifdef USB_CLASS_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS) {
    USB_CLASS_OUT_handler();
    return;
  }
  #endif

  #ifdef USB_VENDOR_OUT_handler
  if((USB_SetupTyp & USB_REQ_TYP_MASK) == USB_REQ_TYP_VENDOR) {
    USB_VENDOR_OUT_handler();
    return;
  }
  #endif

  USBFSD->UEP0_TX_LEN = 0;
  USBFSD->UEP0_CTRL_H = USBFS_UEP_T_RES_ACK | USBFS_UEP_R_RES_ACK;
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
    uint8_t callIndex = intst & USBFS_UIS_ENDP_MASK;
    switch(intst & USBFS_UIS_TOKEN_MASK) {

      case USBFS_UIS_TOKEN_SETUP:
        EP0_SETUP_callback();
        break;

      case USBFS_UIS_TOKEN_IN:
        switch(callIndex) {
          case 0: EP0_IN_callback(); break;
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
        break;

      case USBFS_UIS_TOKEN_OUT:
        switch (callIndex) {
          case 0: EP0_OUT_callback(); break;
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
          default: break;
        }
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
    USB_EP_init();
    USBFSD->DEV_ADDR = 0;
    USBFSD->INT_FG   = 0xff;
  }
}
