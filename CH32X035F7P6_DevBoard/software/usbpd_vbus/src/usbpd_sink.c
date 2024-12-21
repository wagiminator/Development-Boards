// ===================================================================================
// USB PD SINK Handler for CH32X035                                           * v1.5 *
// ===================================================================================
//
// Reference:               https://github.com/openwch/ch32x035
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usbpd_sink.h"

// Variables
static pd_control_t PD_control = {
  .CC_State = CC_IDLE,
  .CC1_ConnectTimes = 0,
  .CC2_ConnectTimes = 0,
};

FixedSourceCap_t PD_SC_fixed[5];
PPSSourceCap_t   PD_SC_PPS[2];

// Buffers
__attribute__ ((aligned(4))) uint8_t PD_TR_buffer[34];  // PD transmit/receive buffer 
__attribute__ ((aligned(4))) uint8_t PD_SC_buffer[28];  // PD Source Cap buffer

// ===================================================================================
// USB PD SINK Front End Functions
// ===================================================================================

// Prototype
void PD_update(void);

// Negotiate current settings and wait until finished (return 1) or timeout (return 0)
uint8_t PD_negotiate(void) {
  uint8_t counter = 255;
  PD_control.LastSetVoltage = 0;
  PD_control.USBPD_READY = 0;
  while((!PD_control.USBPD_READY) && (--counter)) {
    DLY_ms(5);
    PD_update();
  }
  return(counter > 0);
}

// Get total number of PDOs (fixed and programmable)
uint8_t PD_getPDONum(void) {
  return PD_control.SourcePDONum;
}

// Get number of fixed power PDOs
uint8_t PD_getFixedNum(void) {
  return PD_control.SourcePDONum - PD_control.SourcePPSNum;
}

// Get number of programmable power PDOs
uint8_t PD_getPPSNum(void) {
  return PD_control.SourcePPSNum;
}

// Get voltage of specified fixed power PDO
uint16_t PD_getPDOVoltage(uint8_t pdonum) {
  return PD_control.FixedSourceCap[pdonum - 1].Voltage;
}

// Get minimum voltage of specified PDO (fixed and programmable)
uint16_t PD_getPDOMinVoltage(uint8_t pdonum) {
  uint8_t ppspos = PD_control.SourcePDONum - PD_control.SourcePPSNum;
  if(pdonum <= ppspos)
    return PD_control.FixedSourceCap[pdonum - 1].Voltage;
  else return PD_control.PPSSourceCap[pdonum - ppspos - 1].MinVoltage;
}

// Get maximum voltage of specified PDO (fixed and programmable)
uint16_t PD_getPDOMaxVoltage(uint8_t pdonum) {
  uint8_t ppspos = PD_control.SourcePDONum - PD_control.SourcePPSNum;
  if(pdonum <= ppspos)
    return PD_control.FixedSourceCap[pdonum - 1].Voltage;
  else return PD_control.PPSSourceCap[pdonum - ppspos - 1].MaxVoltage;
}

// Get max current of specified PDO (fixed and programmable)
uint16_t PD_getPDOMaxCurrent(uint8_t pdonum) {
  uint8_t ppspos = PD_control.SourcePDONum - PD_control.SourcePPSNum;
  if(pdonum <= ppspos)
    return PD_control.FixedSourceCap[pdonum - 1].Current;
  else return PD_control.PPSSourceCap[pdonum - ppspos - 1].Current;
}

// Set specified PDO and voltage; returns 0:failed, 1:success
uint8_t PD_setPDO(uint8_t pdonum, uint16_t voltage) {
  PD_control.SetPDONum  = pdonum;
  PD_control.SetVoltage = voltage;
  return PD_negotiate();
}

// Set specified voltage (in millivolts) if available; returns 0:failed, 1:success
uint8_t PD_setVoltage(uint16_t voltage) {
  uint8_t i;
  uint8_t ppspos = PD_control.SourcePDONum - PD_control.SourcePPSNum;
  for(i=0; i<PD_control.SourcePDONum; i++) {
    if(i < ppspos) {
      if(PD_control.FixedSourceCap[i].Voltage == voltage) {
        return PD_setPDO(i + 1, voltage);
      }
    }
    else {
      if((PD_control.PPSSourceCap[i-ppspos].MinVoltage <= voltage) &&
         (PD_control.PPSSourceCap[i-ppspos].MaxVoltage >= voltage)) {
        return PD_setPDO(i + 1, voltage);
      }
    }
  }
  return 0;
}

// Get active PDO
uint8_t PD_getPDO(void) {
  return PD_control.SetPDONum;
}

// Get active voltage
uint16_t PD_getVoltage(void) {
  return PD_control.SetVoltage;
}

// Get active max current
uint16_t PD_getCurrent(void) {
  uint8_t pdonum = PD_control.SetPDONum;
  uint8_t ppspos = PD_control.SourcePDONum - PD_control.SourcePPSNum;
  if(pdonum <= ppspos)
    return PD_control.FixedSourceCap[pdonum - 1].Current;
  else return PD_control.PPSSourceCap[pdonum - ppspos - 1].Current;
}

// Initialize PD registers and states, then connect
uint8_t PD_connect(void) {
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
  RCC->AHBPCENR  |= RCC_USBPD;
  GPIOB->CFGHR    = (GPIOB->CFGHR & ~( (uint32_t)0b1111<<(((14)&7)<<2) | (uint32_t)0b1111<<(((15)&7)<<2)))
                                  |  ( (uint32_t)0b0100<<(((14)&7)<<2) | (uint32_t)0b0100<<(((15)&7)<<2));
  #ifdef USB_VDD
    #if USB_VDD > 0
      AFIO->CTLR |= USBPD_IN_HVT;
    #else
      AFIO->CTLR |= USBPD_IN_HVT | USBPD_PHY_V33;
    #endif
  #else
    RCC->APB1PCENR |= RCC_PWREN;
    PWR->CTLR |= PWR_CTLR_PLS;
    if(PWR->CSR & PWR_CSR_PVDO) AFIO->CTLR |= USBPD_IN_HVT | USBPD_PHY_V33;
    else                        AFIO->CTLR |= USBPD_IN_HVT;
  #endif

  USBPD->DMA      = (uint32_t)PD_TR_buffer;
  USBPD->CONFIG   = USBPD_IE_RX_ACT | USBPD_IE_RX_RESET | USBPD_IE_TX_END  | USBPD_PD_DMA_EN;
  USBPD->STATUS   = USBPD_BUF_ERR   | USBPD_IF_RX_BIT   | USBPD_IF_RX_BYTE 
                  | USBPD_IF_RX_ACT | USBPD_IF_RX_RESET | USBPD_IF_TX_END;
  return PD_negotiate();
}

// ===================================================================================
// USB PD SINK Back End Functions
// ===================================================================================

// Enter reception mode
void PD_RX_mode(void) {
  USBPD->BMC_CLK_CNT =  USBPD_TMR_RX;
  USBPD->CONTROL     = (USBPD->CONTROL & ~USBPD_PD_TX_EN) | USBPD_BMC_START;
}

// Reset PD
void PD_reset(void) {
  USBPD->PORT_CC1 = USBPD_CC_CMP_66 | USBPD_CC_PD;
  USBPD->PORT_CC2 = USBPD_CC_CMP_66 | USBPD_CC_PD;
  PD_control.CC1_ConnectTimes  = 0;
  PD_control.CC2_ConnectTimes  = 0;
  PD_control.CC_NoneTimes      = 0;
  PD_control.SourcePDONum      = 0;
  PD_control.SourcePPSNum      = 0;
  PD_control.FixedSourceCap    = PD_SC_fixed;
  PD_control.PPSSourceCap      = PD_SC_PPS;
  PD_control.CC_State          = CC_IDLE;
  PD_control.CC_LastState      = CC_IDLE;
  PD_control.SinkMessageID     = 0;
  PD_control.SinkGoodCRCOver   = 0;
  PD_control.SourceGoodCRCOver = 0;
  PD_control.PD_Version        = USBPD_REVISION_20;
  PD_control.USBPD_READY       = 0;
  PD_control.SetPDONum         = 1;
  PD_control.LastSetPDONum     = 1;
  PD_control.SetVoltage        = 5000;
  PD_control.LastSetVoltage    = 5000;
}

// Copy buffers
void PD_memcpy(uint8_t* dest, const uint8_t* src, uint8_t n) {
  while(n--) *dest++ = *src++;
}

// Send PD data
void PD_sendData(uint8_t length) {
  if((USBPD->CONFIG & USBPD_CC_SEL) == USBPD_CC_SEL) USBPD->PORT_CC2 |= USBPD_CC_LVE;
  else                                               USBPD->PORT_CC1 |= USBPD_CC_LVE;

  USBPD->BMC_CLK_CNT = USBPD_TMR_TX;
  USBPD->TX_SEL      = USBPD_TX_SOP0;
  USBPD->BMC_TX_SZ   = length;
  USBPD->STATUS      = 0;
  USBPD->CONTROL    |= USBPD_BMC_START | USBPD_PD_TX_EN;
}

// Detect CC connection; returns 0:No connection, 1:CC1 connection, 2:CC2 connection
uint8_t PD_checkCC(void) {
  uint8_t ccLine = USBPD_CCNONE;

  USBPD->PORT_CC1 &= ~(USBPD_CC_CE | USBPD_PA_CC_AI);
  USBPD->PORT_CC1 |= USBPD_CC_CMP_22;
  if(USBPD->PORT_CC1 & USBPD_PA_CC_AI) ccLine = USBPD_CC1;

  USBPD->PORT_CC2 &= ~(USBPD_CC_CE | USBPD_PA_CC_AI);
  USBPD->PORT_CC2 |= USBPD_CC_CMP_22;
  if(USBPD->PORT_CC2 & USBPD_PA_CC_AI) ccLine = USBPD_CC2;

  return ccLine;
}

void PD_PDO_analyze(void) {
  USBPD_PDO_t test;
  PD_control.SourcePPSNum = 0;

  for(uint8_t i=0; i<PD_control.SourcePDONum; i++) { 
    test.d32 = *(uint32_t*)(&PD_SC_buffer[i*4]);
    if((test.SourcePPSPDO.AugmentedPowerDataObject==3u) && 
       (test.SourcePPSPDO.SPRprogrammablePowerSupply==0)) {
         PD_control.PPSSourceCap[PD_control.SourcePPSNum].MaxVoltage = POWER_DECODE_100MV(test.SourcePPSPDO.MaxVoltageIn100mVincrements);
         PD_control.PPSSourceCap[PD_control.SourcePPSNum].MinVoltage = POWER_DECODE_100MV(test.SourcePPSPDO.MinVoltageIn100mVincrements);
         PD_control.PPSSourceCap[PD_control.SourcePPSNum].Current    = POWER_DECODE_50MA(test.SourcePPSPDO.MaxCurrentIn50mAincrements);
         PD_control.SourcePPSNum++;
    }
    else {
         PD_control.FixedSourceCap[i].Current = POWER_DECODE_10MA(test.SourceFixedPDO.MaxCurrentIn10mAunits);
         PD_control.FixedSourceCap[i].Voltage = POWER_DECODE_50MV(test.SourceFixedPDO.VoltageIn50mVunits);
    }
  }
}

// Send specified PDO
void PD_PDO_request(void) {
  uint8_t pdoNum = PD_control.SetPDONum;
  USBPD_SINKRDO_t pdo;
  USBPD_MessageHeader_t mh;
  mh.d16  = 0u;
  pdo.d32 = 0u;

  mh.MessageHeader.MessageID             = PD_control.SinkMessageID ;
  mh.MessageHeader.MessageType           = USBPD_DATA_MSG_REQUEST;
  mh.MessageHeader.NumberOfDataObjects   = 1u;
  mh.MessageHeader.SpecificationRevision = PD_control.PD_Version;

  if(pdoNum > (PD_control.SourcePDONum - PD_control.SourcePPSNum)) {
    pdo.SinkPPSRDO.ObjectPosition              = pdoNum;
    pdo.SinkPPSRDO.OutputVoltageIn20mVunits    = PD_control.SetVoltage / 20;
    pdo.SinkPPSRDO.OperatingCurrentIn50mAunits = PD_SC_PPS[pdoNum+PD_control.SourcePPSNum-PD_control.SourcePDONum-1].Current/50;
    pdo.SinkPPSRDO.NoUSBSuspend                = 1u;
    pdo.SinkPPSRDO.USBCommunicationsCapable    = 1u;
  }
  else {
    pdo.SinkFixedVariableRDO.ObjectPosition               = pdoNum;
    pdo.SinkFixedVariableRDO.MaxOperatingCurrent10mAunits = PD_SC_fixed[pdoNum-1].Current/10;
    pdo.SinkFixedVariableRDO.OperatingCurrentIn10mAunits  = PD_SC_fixed[pdoNum-1].Current/10;
    pdo.SinkFixedVariableRDO.USBCommunicationsCapable     = 1u;
    pdo.SinkFixedVariableRDO.NoUSBSuspend                 = 1u;
  }

  *(uint16_t*)&PD_TR_buffer[0] = mh.d16;
  *(uint32_t*)&PD_TR_buffer[2] = pdo.d32;
  PD_sendData(6);
}

void PD_process(void) {
  cc_state_t temp = PD_control.CC_State;
  USBPD_MessageHeader_t mh;

  switch (PD_control.CC_State) {

    case CC_IDLE:
      NVIC_DisableIRQ(USBPD_IRQn);  
      PD_reset();
      PD_control.CC_State = CC_CHECK_CONNECT;
      break;

    case CC_CHECK_CONNECT:
      break;

    case CC_CONNECT:
      if(PD_control.CC_LastState != PD_control.CC_State) {
        PD_RX_mode();
        NVIC_SetPriority(USBPD_IRQn, 0x00);
        NVIC_EnableIRQ(USBPD_IRQn);  
      }
      break;

    case CC_SOURCE_CAP:
      if(PD_control.SinkGoodCRCOver) {
        PD_control.SinkGoodCRCOver = 0;
        NVIC_DisableIRQ(USBPD_IRQn);
        PD_PDO_analyze();
        NVIC_EnableIRQ(USBPD_IRQn);
        PD_control.CC_State = CC_SEND_REQUEST;
      }
      break;

    case CC_SEND_REQUEST:
      if(PD_control.CC_LastState != PD_control.CC_State) {
        PD_PDO_request();
      }
      if(PD_control.SourceGoodCRCOver) {
        PD_control.SourceGoodCRCOver = 0;
        PD_control.CC_State = CC_WAIT_ACCEPT;
      }
      break;

    case CC_WAIT_PS_RDY:
      break;

    case CC_PS_RDY:
      if(PD_control.SinkGoodCRCOver) {
        PD_control.SinkGoodCRCOver = 0;
        PD_control.CC_State = CC_GET_SOURCE_CAP;
        PD_control.WaitTime = 0;
      }
      break;

    case CC_GET_SOURCE_CAP:
      PD_control.USBPD_READY = 1; 
      if((PD_control.SetPDONum   != PD_control.LastSetPDONum) ||
         (PD_control.SetVoltage  != PD_control.LastSetVoltage)) {
        PD_control.LastSetPDONum  = PD_control.SetPDONum;
        PD_control.LastSetVoltage = PD_control.SetVoltage;
        PD_control.USBPD_READY    = 0; 

        mh.d16 = 0u;
        mh.MessageHeader.MessageID             = PD_control.SinkMessageID;
        mh.MessageHeader.MessageType           = USBPD_CONTROL_MSG_GET_SRC_CAP;
        mh.MessageHeader.NumberOfDataObjects   = 0u;
        mh.MessageHeader.SpecificationRevision = PD_control.PD_Version;
        *(uint16_t*)&PD_TR_buffer[0] = mh.d16;
        PD_sendData(2);
        PD_control.CC_State = CC_GET_SOURCE_CAP+1;
      }
      break;

    default:
      break;
  }
  PD_control.CC_LastState = temp;
}

void PD_update(void) {
  uint8_t ccLine = PD_checkCC();
  PD_control.WaitTime++;

  if(PD_control.CC_State == CC_CHECK_CONNECT) {
    if(ccLine == USBPD_CC1) {
      PD_control.CC2_ConnectTimes = 0;
      PD_control.CC1_ConnectTimes++;
      if(PD_control.CC1_ConnectTimes > 5) {
        PD_control.CC1_ConnectTimes = 0;
        PD_control.CC_State = CC_CONNECT;
        USBPD->CONFIG &= ~USBPD_CC_SEL;
      }
    }
    else if(ccLine == USBPD_CC2) {
      PD_control.CC1_ConnectTimes = 0;
      PD_control.CC2_ConnectTimes++;
      if(PD_control.CC2_ConnectTimes > 5) {
        PD_control.CC2_ConnectTimes = 0;
        PD_control.CC_State = CC_CONNECT;
        USBPD->CONFIG |= USBPD_CC_SEL;
      }
    }
    else {
      PD_control.CC1_ConnectTimes = 0;
      PD_control.CC2_ConnectTimes = 0;
    }
  }

  if(PD_control.CC_State > CC_CHECK_CONNECT) {
    if(ccLine == USBPD_CCNONE) {
      PD_control.CC_NoneTimes++;
      if(PD_control.CC_NoneTimes > 5) {
        PD_control.CC_NoneTimes = 0;
        PD_control.CC_State = CC_IDLE;
        NVIC_DisableIRQ(USBPD_IRQn);  
      }
    } 
    else PD_control.CC_NoneTimes = 0;    
  }

  PD_process();
}

// Analyze received data
void PD_RX_analyze(void) {
  uint8_t sendGoodCRCFlag = 1;
  USBPD_MessageHeader_t mh;
  mh.d16 = *(uint16_t*)PD_TR_buffer;

  if(mh.MessageHeader.Extended == 0u) {
    if(mh.MessageHeader.NumberOfDataObjects == 0u) {
      switch(mh.MessageHeader.MessageType) {

        case USBPD_CONTROL_MSG_GOODCRC:
          sendGoodCRCFlag = 0;
          PD_control.SourceGoodCRCOver = 1;
          PD_control.SinkMessageID++;
          break;

        case USBPD_CONTROL_MSG_ACCEPT:
          PD_control.CC_State = CC_WAIT_PS_RDY;
          break;

        case USBPD_CONTROL_MSG_PS_RDY:
          PD_control.CC_State = CC_PS_RDY;
          break;

        default:
          break;
      }
    }
    else {
      switch(mh.MessageHeader.MessageType) {

        case USBPD_DATA_MSG_SRC_CAP:
          PD_control.CC_State = CC_SOURCE_CAP;
          PD_control.SourcePDONum = mh.MessageHeader.NumberOfDataObjects;
          PD_control.PD_Version = mh.MessageHeader.SpecificationRevision;
          PD_memcpy(PD_SC_buffer, &PD_TR_buffer[2], 28);
          break;

        default:
          break;
      }
    }
  }

  if(sendGoodCRCFlag) {
    DLY_us(30);
    PD_control.SinkGoodCRCOver = 0;
    USBPD_MessageHeader_t my_mh;
    my_mh.d16 = 0u;
    my_mh.MessageHeader.MessageID = mh.MessageHeader.MessageID;
    my_mh.MessageHeader.MessageType = USBPD_CONTROL_MSG_GOODCRC;
    my_mh.MessageHeader.SpecificationRevision = PD_control.PD_Version;
    *(uint16_t*)&PD_TR_buffer[0] =  my_mh.d16;
    PD_sendData(2);
  } 
}

// ===================================================================================
// USB PD Interrupt Service Routine
// ===================================================================================
void USBPD_IRQHandler(void) __attribute__((interrupt));
void USBPD_IRQHandler(void) {

  // Receive complete interrupt
  if(USBPD->STATUS & USBPD_IF_RX_ACT) {
    if((USBPD->STATUS & USBPD_BMC_AUX) == USBPD_BMC_AUX_SOP0) {
      if(USBPD->BMC_BYTE_CNT >= 6) {
        PD_RX_analyze();
      }
    }
    USBPD->STATUS |= USBPD_IF_RX_ACT;
  }

  // Transmit complete interrupt (GoodCRC only)
  if(USBPD->STATUS & USBPD_IF_TX_END) {
    USBPD->PORT_CC1 &= ~USBPD_CC_LVE;
    USBPD->PORT_CC2 &= ~USBPD_CC_LVE;
    PD_RX_mode();
    PD_control.SinkGoodCRCOver = 1;
    USBPD->STATUS |= USBPD_IF_TX_END;
  }

  // Reset interrupt
  if(USBPD->STATUS & USBPD_IF_RX_RESET) {
    USBPD->STATUS |= USBPD_IF_RX_RESET;
    PD_reset();
  }
}
