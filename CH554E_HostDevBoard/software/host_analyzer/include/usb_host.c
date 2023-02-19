// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#include <debug.h>
#include <delay.h>
#include <stdio.h>
#include <string.h>
#include "usb_host.h"

#pragma disable_warning 84
#pragma disable_warning 110

extern __xdata __at (0x0000) uint8_t RxBuffer[MAX_PACKET_SIZE];
extern __xdata __at (0x0040) uint8_t TxBuffer[MAX_PACKET_SIZE];

__bit HubLowSpeed;

// Define user temporary buffers, used to process descriptors during enumeration, and 
// can also be used as ordinary temporary buffers at the end of enumeration.
__xdata uint8_t Com_Buffer[COM_BUF_SIZE];

// ===================================================================================
// Close the HUB port
// ===================================================================================
void DisableRootHubPort(void) {
  #ifdef FOR_ROOT_UDISK_ONLY
  CH554DiskStatus = DISK_DISCONNECT;
  #endif
  #ifndef DISK_BASE_BUF_LEN
  ThisUsbDev.DeviceStatus = ROOT_DEV_DISCONNECT;
  ThisUsbDev.DeviceAddress = 0x00;
  #endif
}

// ===================================================================================
// Analyze the status of ROOT-HUB and handle the event of device plugging and un-
// plugging on the ROOT-HUB port. If the device is pulled out, the DisableRootHubPort()
// function is called in the function to close the port, insert an event, and set the 
// status bit of the corresponding port.
// Return ERR_SUCCESS for no case, return ERR_USB_CONNECT for detected new connection, 
// return ERR_USB_DISCON for detected disconnection.
// ===================================================================================
uint8_t AnalyzeRootHub(void) { 
  uint8_t s;
  s = ERR_SUCCESS;
  if(USB_MIS_ST & bUMS_DEV_ATTACH) {                  // device exists
    #ifdef DISK_BASE_BUF_LEN
    if(CH554DiskStatus == DISK_DISCONNECT
    #else
    if(ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT // device plugged in detected
    #endif
    || (UHOST_CTRL & bUH_PORT_EN) == 0x00 ) {
      // It is detected that a device is plugged in, but it has not been allowed, 
      // indicating that it has just been plugged in.
      DisableRootHubPort();                           // close port
      #ifdef DISK_BASE_BUF_LEN
      CH554DiskStatus = DISK_CONNECT;
      #else
      //ThisUsbDev.DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
      ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;   // set connection flag
      #endif
      #if DEBUG_ENABLE
      printf("USB dev in\n");
      #endif
      s = ERR_USB_CONNECT;
    }
  }
  #ifdef DISK_BASE_BUF_LEN
  else if(CH554DiskStatus >= DISK_CONNECT) {
  #else
  else if(ThisUsbDev.DeviceStatus >= ROOT_DEV_CONNECTED) {  // device unplugged detected
  #endif
    DisableRootHubPort();                             // close port
    #if DEBUG_ENABLE		
    printf("USB dev out\n");
    #endif
    if(s == ERR_SUCCESS) s = ERR_USB_DISCON;
  }
  //UIF_DETECT = 0;                                     // clear interrupt flag
  return(s);
}

// ===================================================================================
// Set the address of the USB device currently operated by the USB host
// ===================================================================================
void SetHostUsbAddr(uint8_t addr) {
  USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | addr & 0x7F;
}

// ===================================================================================
// Set the current USB speed
// ===================================================================================
#ifndef	FOR_ROOT_UDISK_ONLY
void SetUsbSpeed(uint8_t FullSpeed) {
  if(FullSpeed) {                       // full speed
    USB_CTRL &= ~bUC_LOW_SPEED;         // set full speed
    UH_SETUP &= ~bUH_PRE_PID_EN;        // disable PRE PID
  }
  else {
    USB_CTRL |=  bUC_LOW_SPEED;         // set low speed
  }
}
#endif

// ===================================================================================
// After detecting the device, reset the bus, prepare for enumerating the device, set 
// the default to full speed.
// ===================================================================================
void ResetRootHubPort(void) {
  UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;       // maximum packet size for endpoint 0 of a USB device
  #ifndef DISK_BASE_BUF_LEN	
  memset(&ThisUsbDev,0,sizeof(ThisUsbDev));   // empty structure
  #endif
  SetHostUsbAddr(0x00);
  UHOST_CTRL &= ~bUH_PORT_EN;                 // turn off the port
  SetUsbSpeed(1);                             // default is full speed
  UHOST_CTRL = UHOST_CTRL & ~bUH_LOW_SPEED | bUH_BUS_RESET;  // start reset
  DLY_ms(20);                                 // reset time 10mS to 20mS
  UHOST_CTRL = UHOST_CTRL & ~bUH_BUS_RESET;   // end reset
  DLY_us(250);
  UIF_DETECT = 0;                             // clear interrupt flag
}

// ===================================================================================
// Enable the ROOT-HUB port, set the corresponding bUH_PORT_EN to 1 to open the port, 
// and the disconnection of the device may cause the return failure.
// Return ERR_SUCCESS for a new connection detected, return ERR_USB_DISCON for no 
// connection.
// ===================================================================================
uint8_t EnableRootHubPort(void) {
  #ifdef DISK_BASE_BUF_LEN
  if(CH554DiskStatus < DISK_CONNECT) CH554DiskStatus = DISK_CONNECT;
  #else
  if(ThisUsbDev.DeviceStatus < ROOT_DEV_CONNECTED) ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;
  #endif
  if(USB_MIS_ST & bUMS_DEV_ATTACH) {                                // have device
    #ifndef DISK_BASE_BUF_LEN
    if((UHOST_CTRL & bUH_PORT_EN) == 0x00 ) {                       // not yet enabled
      ThisUsbDev.DeviceSpeed = USB_MIS_ST & bUMS_DM_LEVEL ? 0 : 1;
      if(ThisUsbDev.DeviceSpeed == 0) UHOST_CTRL |= bUH_LOW_SPEED;  // low speed
    }
    #endif
    USB_CTRL |= bUC_DMA_EN;       // start USB host and DMA, automatically suspend before interrupt flag is cleared
    UH_SETUP = bUH_SOF_EN;		
    UHOST_CTRL |= bUH_PORT_EN;    // enable HUB port
    return(ERR_SUCCESS);
  }
  return(ERR_USB_DISCON);
}

// ===================================================================================
// Select the HUB port to be operated
// ===================================================================================
#ifndef DISK_BASE_BUF_LEN
void SelectHubPort(uint8_t HubPortIndex) {
  if(HubPortIndex) {  // Select designated port of external HUB that operates designated ROOT-HUB port
    SetHostUsbAddr(DevOnHubPort[HubPortIndex-1].DeviceAddress); // set address of USB device currently operated by USB host
    SetUsbSpeed(DevOnHubPort[HubPortIndex-1].DeviceSpeed);      // set current USB speed
    if(DevOnHubPort[HubPortIndex-1].DeviceSpeed == 0) {         // communication with low-speed USB devices through an external HUB requires a pre-ID
      UH_SETUP |= bUH_PRE_PID_EN;                               // enable PRE PIDs
      HubLowSpeed = 1;
      DLY_us(100);
    }
  }
  else {
    HubLowSpeed = 0;        			
    SetHostUsbAddr(ThisUsbDev.DeviceAddress);   // set address of USB device currently operated by USB host
    SetUsbSpeed(ThisUsbDev.DeviceSpeed);        // set speed of USB device
  }
}
#endif

// ===================================================================================
// Wait for USB interrupt
// Return ERR_SUCCESS:     data received or sent successfully
//        ERR_USB_UNKNOWN: failed to receive or send data
// ===================================================================================
uint8_t WaitUSB_Interrupt(void) {
  uint16_t  i;
  for(i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- ){;}
  return(UIF_TRANSFER ? ERR_SUCCESS : ERR_USB_UNKNOWN);
}

// ===================================================================================
// CH554 transmission transaction, input destination endpoint address/PID token, 
// synchronization flag, NAK retry total time in 20uS (0 means no retry, 
// 0xFFFF infinite retry), return 0 success, timeout/error retry
// This subroutine focuses on easy understanding, but in practical applications, in 
// order to increase the running speed, the code of this subroutine should be optimized.
// Return ERR_USB_UNKNOWN:  timeout, possible hardware exception
//        ERR_USB_DISCON:   device disconnected
//        ERR_USB_CONNECT:  device connected
//        ERR_SUCCESS:      transfer complete
// ===================================================================================
uint8_t USBHostTransact(uint8_t endp_pid, uint8_t tog, uint16_t timeout) {
  //uint8_t TransRetry;
  #define TransRetry UEP0_T_LEN         // save memory
  uint8_t s, r;
  uint16_t i;
  UH_RX_CTRL = UH_TX_CTRL = tog;
  TransRetry = 0;

  do {
    UH_EP_PID = endp_pid;               // specify token PID and destination endpoint number
    UIF_TRANSFER = 0;                   // allow transmission
    // s = WaitUSB_Interrupt();
    for(i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i--);
    UH_EP_PID = 0x00;                   // stop USB transfer
    //if(s != ERR_SUCCESS) return(s);   // interrupt timeout, may be a hardware exception
    if(UIF_TRANSFER == 0) return(ERR_USB_UNKNOWN);
    if(UIF_DETECT) {                    // USB device plug event
    //DLY_us(200);                      // wait for the transfer to complete
    UIF_DETECT = 0;                     // clear interrupt flag
    s = AnalyzeRootHub();               // analyze ROOT-HUB status
    if(s == ERR_USB_CONNECT) FoundNewDev = 1;
      #ifdef DISK_BASE_BUF_LEN
      if(CH554DiskStatus == DISK_DISCONNECT) return(ERR_USB_DISCON);  // USB device disconnect event
      if(CH554DiskStatus == DISK_CONNECT) return(ERR_USB_CONNECT);    // USB device connect event
      #else
      if(ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT) return(ERR_USB_DISCON);  // USB device disconnect event
      if(ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED) return(ERR_USB_CONNECT);  // USB device connect event
      #endif
      DLY_us(200);                      // wait for the transfer to complete
    }
    if(UIF_TRANSFER) {                  // transfer complete
      if(U_TOG_OK) return(ERR_SUCCESS);
      r = USB_INT_ST & MASK_UIS_H_RES;  // USB device answer status
      if(r == USB_PID_STALL) return(r | ERR_USB_TRANSFER);
      if(r == USB_PID_NAK) {
        if(timeout == 0) return(r | ERR_USB_TRANSFER);
        if(timeout < 0xFFFF) timeout--;
        --TransRetry;
      }
      else switch(endp_pid >> 4) {
        case USB_PID_SETUP:
        case USB_PID_OUT:
          //if(U_TOG_OK) return(ERR_SUCCESS);
          //if(r == USB_PID_ACK) return(ERR_SUCCESS);
          //if(r == USB_PID_STALL || r == USB_PID_NAK) return(r | ERR_USB_TRANSFER);
          if(r) return(r | ERR_USB_TRANSFER);   // not timeout/error, unexpected response
          break;                                // timeout retry
        case USB_PID_IN:
          //if(U_TOG_OK) return(ERR_SUCCESS);
          //if(tog ? r == USB_PID_DATA1 : r == USB_PID_DATA0) return(ERR_SUCCESS);
          //if(r == USB_PID_STALL || r == USB_PID_NAK) return(r | ERR_USB_TRANSFER);
          if(r == USB_PID_DATA0 && r == USB_PID_DATA1) {  // if not synchronized, discard it and try again
          }                                               // retry out of sync
          else if(r) return(r | ERR_USB_TRANSFER);        // not timeout/error, unexpected response
          break;                                          // timeout retry
        default:
          return(ERR_USB_UNKNOWN);                        // impossible situation
          break;
      }
    }
    else {                        // other interrupts, situations that should not happen
      USB_INT_FG = 0xFF;          // clear interrupt flag
    }
    DLY_us(15);	
  } while(++TransRetry < 3);
  return(ERR_USB_TRANSFER);       // response timeout
}

// ===================================================================================
// Execute control transmission, the 8-byte request code is in pSetupReq, and DataBuf 
// is an optional sending and receiving buffer.
// Return ERR_USB_BUF_OVER: IN state phase error
//        ERR_SUCCESS:      data exchange succeeded
// ===================================================================================
uint8_t HostCtrlTransfer(__xdata uint8_t *DataBuf, uint8_t *RetLen) {
  uint16_t RemLen = 0;
  uint8_t s, RxLen, RxCnt, TxCnt;
  __xdata uint8_t *pBuf;
  uint8_t *pLen;
  pBuf = DataBuf;
  pLen = RetLen;
  DLY_us(200);
  if(pLen) *pLen = 0;       // total length of actual successful sending and receiving
  UH_TX_LEN = sizeof(USB_SETUP_REQ);
  s = USBHostTransact((uint8_t)(USB_PID_SETUP << 4 | 0x00), 0x00, 10000); // SETUP stage, 200mS timeout
  if(s != ERR_SUCCESS) return(s);
  UH_RX_CTRL = UH_TX_CTRL = bUH_R_TOG | bUH_R_AUTO_TOG | bUH_T_TOG | bUH_T_AUTO_TOG;  // default DATA1
  UH_TX_LEN = 0x01;     // default state of no data is IN
  RemLen = (pSetupReq -> wLengthH << 8)|( pSetupReq -> wLengthL);
  if(RemLen && pBuf) {  // need to send and receive data
    if(pSetupReq -> bRequestType & USB_REQ_TYP_IN) {    // receive
      while(RemLen) {
        DLY_us(200);
        s = USBHostTransact((uint8_t)(USB_PID_IN << 4 | 0x00), UH_RX_CTRL, 200000/20);  // IN data
        if(s != ERR_SUCCESS) return(s);
        RxLen = USB_RX_LEN < RemLen ? USB_RX_LEN : RemLen;
        RemLen -= RxLen;
        if(pLen) *pLen += RxLen;  // total length of actual successful sending and receiving
        //memcpy(pBuf, RxBuffer, RxLen);
        //pBuf += RxLen;
        for(RxCnt = 0; RxCnt != RxLen; RxCnt ++) {
          *pBuf = RxBuffer[RxCnt];
          pBuf++;
        }
        if(USB_RX_LEN == 0 || (USB_RX_LEN & (UsbDevEndp0Size - 1))) break;  // short bag
      }
      UH_TX_LEN = 0x00;                     // status phase is OUT
    }
    else {                                  // send
      while(RemLen) {
        DLY_us(200);
        UH_TX_LEN = RemLen >= UsbDevEndp0Size ? UsbDevEndp0Size : RemLen;
        //memcpy(TxBuffer, pBuf, UH_TX_LEN);
        //pBuf += UH_TX_LEN;
        #ifndef DISK_BASE_BUF_LEN
        if(pBuf[1] == 0x09) {               // HID class command processing
          Set_Port = Set_Port^1;
          *pBuf = Set_Port;
          #if DEBUG_ENABLE									
          printf("SET_PORT  %02X  %02X ",(uint16_t)(*pBuf),(uint16_t)(Set_Port));
          #endif									
        }
        #endif
        for(TxCnt = 0; TxCnt != UH_TX_LEN; TxCnt ++) {
          TxBuffer[TxCnt] = *pBuf;
          pBuf ++;
        }
        s = USBHostTransact(USB_PID_OUT << 4 | 0x00, UH_TX_CTRL, 200000/20);  // OUT data
        if(s != ERR_SUCCESS) return(s);
        RemLen -= UH_TX_LEN;
        if(pLen) *pLen += UH_TX_LEN;        // total length of actual successful sending and receiving
      }
      //UH_TX_LEN = 0x01;                   // status phase is IN
    }
  }
  DLY_us(200);
  s = USBHostTransact((UH_TX_LEN ? USB_PID_IN << 4 | 0x00: USB_PID_OUT << 4 | 0x00), bUH_R_TOG | bUH_T_TOG, 200000/20); // STATUS stage
  if(s != ERR_SUCCESS) return(s);
  if(UH_TX_LEN == 0) return(ERR_SUCCESS);   // state OUT
  if(USB_RX_LEN == 0) return(ERR_SUCCESS);  // state IN, check IN state return data length
  return(ERR_USB_BUF_OVER);                 // IN state phase error
}

// ===================================================================================
// Copy control transfer request packet
// ===================================================================================
void CopySetupReqPkg(__code uint8_t *pReqPkt) {
  uint8_t i;
  if(HubLowSpeed) {                         // low-speed devices under the HUB
    ((__xdata uint8_t*)pSetupReq)[0] = *pReqPkt;			
    for(i=1; i!=sizeof(USB_SETUP_REQ)+1; i++) {
      ((__xdata uint8_t*)pSetupReq)[i] = *pReqPkt;
      pReqPkt++;
    }
  }
  if(HubLowSpeed == 0) {
    for(i=0; i!=sizeof(USB_SETUP_REQ); i++) {
      ((__xdata uint8_t*)pSetupReq)[i] = *pReqPkt;
      pReqPkt++;
    }			
  }
}

// ===================================================================================
// Get the device descriptor and return it in TxBuffer
// Return ERR_USB_BUF_OVER: wrong descriptor length
//        ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlGetDeviceDescr(void) {
  uint8_t s;
  uint8_t len;
  UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
  CopySetupReqPkg(SetupGetDevDescr);
  s = HostCtrlTransfer(Com_Buffer, (uint8_t*)&len);   // execute control transfer
  if(s != ERR_SUCCESS) return(s);

  // Endpoint 0 maximum packet length, this is simplified processing, normally you 
  // should first obtain the first 8 bytes and update UsbDevEndp0Size immediately 
  // before continuing
  UsbDevEndp0Size = ((PXUSB_DEV_DESCR)Com_Buffer) -> bMaxPacketSize0;
  if(len < ((PUSB_SETUP_REQ)SetupGetDevDescr) -> wLengthL) return(ERR_USB_BUF_OVER);  // wrong descriptor length
  return(ERR_SUCCESS);
}

// ===================================================================================
// Get the configuration descriptor and return it in TxBuffer
// Return ERR_USB_BUF_OVER: wrong descriptor length
//        ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlGetConfigDescr(void) {
  uint8_t s,len;
  CopySetupReqPkg(SetupGetCfgDescr);
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  len = (uint8_t)(((PXUSB_CFG_DESCR)Com_Buffer)->wTotalLength);
  CopySetupReqPkg(SetupGetCfgDescr);
  if(HubLowSpeed) pSetupReq -> wLengthH = len;        // total length of configuration descriptor 
  else            pSetupReq -> wLengthL = len;
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  #ifdef DISK_BASE_BUF_LEN
  if(len>64) len = 64;
  for(s=0; s!=len; s++) TxBuffer[s]=Com_Buffer[s];    // when using U-disk, it needs to be copied to TxBuffer
  #endif
  return(ERR_SUCCESS);
}

// ===================================================================================
// Set USB device address
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlSetUsbAddress(uint8_t addr) {
  uint8_t s;
  CopySetupReqPkg(SetupSetUsbAddr);
  if(HubLowSpeed) pSetupReq -> wValueH = addr;        // USB device address
  else            pSetupReq -> wValueL = addr;
  s = HostCtrlTransfer(NULL, NULL);                   // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  SetHostUsbAddr(addr);     // set address of USB device currently operated by USB host
  DLY_ms(10);               // wait for USB device to complete operation
  return(ERR_SUCCESS);
}

// ===================================================================================
// Set USB device configuration
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlSetUsbConfig(uint8_t cfg) {
  CopySetupReqPkg(SetupSetUsbConfig);
  if(HubLowSpeed) pSetupReq -> wValueH = cfg;         // USB device configuration
  else            pSetupReq -> wValueL = cfg;
  return(HostCtrlTransfer(NULL, NULL));               // execute control transfer
}

// ===================================================================================
// Clear endpoint STALL
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlClearEndpStall(uint8_t endp) {
  CopySetupReqPkg(SetupClrEndpStall);                 // clear endpoint errors
  if(HubLowSpeed) pSetupReq -> wIndexH = endp;        // endpoint address
  else            pSetupReq -> wIndexL = endp;
  return(HostCtrlTransfer(NULL, NULL));               // execute control transfer
}

#ifndef DISK_BASE_BUF_LEN
// ===================================================================================
// Set the USB device interface
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlSetUsbIntercace(uint8_t cfg) {
  CopySetupReqPkg(SetupSetUsbInterface);
  if(HubLowSpeed) pSetupReq -> wValueH = cfg;         // USB device configuration
  else            pSetupReq -> wValueL = cfg;
  return(HostCtrlTransfer(NULL, NULL));               // execute control transfer
}

// ===================================================================================
// Get the HID device report descriptor and return it in TxBuffer
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlGetHIDDeviceReport(uint8_t infc) {
  uint8_t s;
  uint8_t len;
  CopySetupReqPkg(SetupSetHIDIdle);
  if(HubLowSpeed) TxBuffer[5] = infc;
  else            TxBuffer[4] = infc;
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  CopySetupReqPkg(SetupGetHIDDevReport);
  if(HubLowSpeed) TxBuffer[5] = infc;
  else            TxBuffer[4] = infc;
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  return(ERR_SUCCESS);
}

// ===================================================================================
// Get the HUB descriptor and return it in TxBuffer
// Return ERR_SUCCESS:      success
//        ERR_USB_BUF_OVER: wrong length
// ===================================================================================
uint8_t CtrlGetHubDescr(void) {
  uint8_t s;
  uint8_t len;
  CopySetupReqPkg(SetupGetHubDescr);
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  if(len < ((PUSB_SETUP_REQ)SetupGetHubDescr) -> wLengthL)
    return(ERR_USB_BUF_OVER);                         // wrong descriptor length
  //if(len < 4) return(ERR_USB_BUF_OVER);
  return(ERR_SUCCESS);
}

// ===================================================================================
// Query the status of the HUB port and return it in TxBuffer
// Return ERR_SUCCESS:      success
//        ERR_USB_BUF_OVER: wrong length
// ===================================================================================
uint8_t HubGetPortStatus(uint8_t HubPortIndex) {
  uint8_t s;
  uint8_t len;
  pSetupReq -> bRequestType = HUB_GET_PORT_STATUS;
  pSetupReq -> bRequest = HUB_GET_STATUS;
  pSetupReq -> wValueL = 0x00;
  pSetupReq -> wValueH = 0x00;
  pSetupReq -> wIndexL = HubPortIndex;
  pSetupReq -> wIndexH = 0x00;
  pSetupReq -> wLengthL = 0x04;
  pSetupReq -> wLengthH = 0x00;
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  if(len < 4) return(ERR_USB_BUF_OVER);               // wrong descriptor length
  return(ERR_SUCCESS);
}

// ===================================================================================
// Set HUB port characteristics
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt) {
  pSetupReq -> bRequestType = HUB_SET_PORT_FEATURE;
  pSetupReq -> bRequest = HUB_SET_FEATURE;
  pSetupReq -> wValueL = FeatureSelt;
  pSetupReq -> wValueH = 0x00;
  pSetupReq -> wIndexL = HubPortIndex;
  pSetupReq -> wIndexH = 0x00;
  pSetupReq -> wLengthL = 0x00;
  pSetupReq -> wLengthH = 0x00;
  return(HostCtrlTransfer(NULL, NULL));               // execute control transfer
}

// ===================================================================================
// Clear HUB port characteristics
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt) {
  pSetupReq -> bRequestType = HUB_CLEAR_PORT_FEATURE;
  pSetupReq -> bRequest = HUB_CLEAR_FEATURE;
  pSetupReq -> wValueL = FeatureSelt;
  pSetupReq -> wValueH = 0x00;
  pSetupReq -> wIndexL = HubPortIndex;
  pSetupReq -> wIndexH = 0x00;
  pSetupReq -> wLengthL = 0x00;
  pSetupReq -> wLengthH = 0x00;
  return(HostCtrlTransfer(NULL, NULL));               // execute control transfer
}

// ===================================================================================
// Printer class commands
// Return ERR_USB_BUF_OVER: wrong descriptor length
//        ERR_SUCCESS:      success
// ===================================================================================
uint8_t CtrlGetXPrinterReport1(void) {
  uint8_t s;
  uint16_t len;
  CopySetupReqPkg(XPrinterReport);
  s = HostCtrlTransfer(Com_Buffer, (uint8_t *)&len);  // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  //if(len < ((XPrinterReport[7]<<8) | (XPrinterReport[6])))
  //  return(ERR_USB_BUF_OVER);                       // wrong descriptor length
  return(ERR_SUCCESS);
}

// ===================================================================================
// Analyze the address of the HID interrupt endpoint from the descriptor, if the 
// HubPortIndex is 0, save it to ROOTHUB, if it is a non-zero value, save it to 
// the structure under the HUB.
// buf：           data buffer address to be analyzed
// HubPortIndex：  0 means the root HUB, non-0 means the port number under the external HUB
// Return:        endpoints
// ===================================================================================
uint8_t AnalyzeHidIntEndp(__xdata uint8_t *buf, uint8_t HubPortIndex) {
  uint8_t i, s, l;
  s = 0;
  if(HubPortIndex)
    memset(DevOnHubPort[HubPortIndex-1].GpVar,0,sizeof(DevOnHubPort[HubPortIndex-1].GpVar));  // empty array
  else
    memset(ThisUsbDev.GpVar,0,sizeof(ThisUsbDev.GpVar));    // empty array

  // Search interrupt endpoint descriptors, skip configuration descriptors and interface descriptors
  for(i=0; i<(uint8_t)(((PXUSB_CFG_DESCR)buf)->wTotalLength); i+=l) {
    if(((PXUSB_ENDP_DESCR)(buf+i))->bDescriptorType == USB_DESCR_TYP_ENDP                       // enpoint descriptor?
      &&(((PXUSB_ENDP_DESCR)(buf+i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_INTER // interrupt endpoint?
      &&(((PXUSB_ENDP_DESCR)(buf+i))->bEndpointAddress & USB_ENDP_DIR_MASK)) {                  // IN endpoint?

      // Save address of interrupt endpoint, bit 7 is used for synchronization flag bit, cleared to 0
      if(HubPortIndex)
        DevOnHubPort[HubPortIndex-1].GpVar[s] = ((PXUSB_ENDP_DESCR)(buf+i))->bEndpointAddress & USB_ENDP_ADDR_MASK;
      else
        ThisUsbDev.GpVar[s] = ((PXUSB_ENDP_DESCR)(buf+i))->bEndpointAddress & USB_ENDP_ADDR_MASK;                                                        
      #if DEBUG_ENABLE	
      printf("Endpoint: %02x ",(uint16_t)ThisUsbDev.GpVar[s]);
      #endif
      s++;
      if(s >= 4) break;                               // only 4 endpoints are analyzed
    }
    l = ((PXUSB_ENDP_DESCR)(buf+i)) -> bLength;       // current descriptor length, skip
    if(l > 16) break;
  }
  #if DEBUG_ENABLE
  printf("\n");
  #endif
  return(s);
}

// ===================================================================================
// Analyze the batch endpoints, GpVar[0], GpVar[1] store the upload endpoints. 
// GpVar[2], GpVar[3] store the download endpoint
// buf：           data buffer address to be analyzed
// HubPortIndex：  0 means the root HUB, non-0 means the port number under the external HUB
// Return:        0
// ===================================================================================
uint8_t AnalyzeBulkEndp(__xdata uint8_t *buf, uint8_t HubPortIndex) {
  uint8_t i, s1,s2, l;
  s1 = 0; s2 = 2;
  if(HubPortIndex)
    memset(DevOnHubPort[HubPortIndex-1].GpVar,0,sizeof(DevOnHubPort[HubPortIndex-1].GpVar));          // empty array
  else
    memset(ThisUsbDev.GpVar,0,sizeof(ThisUsbDev.GpVar));  // empty array

  // Search interrupt endpoint descriptors, skip configuration descriptors and interface descriptors
  for(i=0; i<(uint8_t)(((PXUSB_CFG_DESCR)buf)->wTotalLength); i+=l) {
    if((((PXUSB_ENDP_DESCR)(buf+i))->bDescriptorType == USB_DESCR_TYP_ENDP)                           // endpoint descriptor?
      && ((((PXUSB_ENDP_DESCR)(buf+i))->bmAttributes & USB_ENDP_TYPE_MASK) == USB_ENDP_TYPE_BULK)) {  // bulk endpoint?

      if(HubPortIndex) {
        if(((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_DIR_MASK)
          DevOnHubPort[HubPortIndex-1].GpVar[s1++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
        else
          DevOnHubPort[HubPortIndex-1].GpVar[s2++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
      }
      else {
        if(((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_DIR_MASK)
          ThisUsbDev.GpVar[s1++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
        else
          ThisUsbDev.GpVar[s2++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
      }
      if(s1 == 2) s1 = 1;
      if(s2 == 4) s2 = 3;			
    }
    l = ((PXUSB_ENDP_DESCR)(buf+i)) -> bLength;           // current descriptor length, skip
    if(l > 16) break;
  }
  return(0);
}

// ===================================================================================
// Try to start AOA mode
// ===================================================================================
uint8_t TouchStartAOA(void) {
  uint8_t len,s,i,Num;
  uint16_t cp_len;
  CopySetupReqPkg(GetProtocol);                       // get protocol version number
  s = HostCtrlTransfer(Com_Buffer, &len);             // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  if(Com_Buffer[0] <2 ) return ERR_AOA_PROTOCOL;
  for(i=0; i<6; i++) {                                // output string
    Num=Sendlen[i];
    CopySetupReqPkg(&SetStringID[8*i]);
    cp_len = (pSetupReq -> wLengthH << 8) | (pSetupReq -> wLengthL);
    memcpy(Com_Buffer, &StringID[Num], cp_len);
    s = HostCtrlTransfer(Com_Buffer, &len);           // execute control transfer
    if(s != ERR_SUCCESS) return(s);
  }

  CopySetupReqPkg(TouchAOAMode);
  s = HostCtrlTransfer(Com_Buffer, &len);             // execute control transfer
  if(s != ERR_SUCCESS) return(s);
  return ERR_SUCCESS;
}

// ===================================================================================
// Initialize the USB device of the specified ROOT-HUB port
// RootHubIndex:  Designated port, built-in HUB port number 0/1
// ===================================================================================
uint8_t InitRootDevice(void) {
  uint8_t t, i, s, cfg, dv_cls, if_cls, ifc;
  uint8_t touchaoatm = 0;
  t = 0;
  #if DEBUG_ENABLE
  printf("Reset USB Port\n");
  #endif

USBDevEnum:
  for(i=0; i<t; i++) {
    DLY_ms(100);	
    if(t > 10) return(s);			
  }
  ResetRootHubPort();                         // after detecting device, reset USB bus of corresponding port
  for(i=0, s=0; i<100; i++) {                 // wait for USB device to reset and reconnect, 100ms timeout
    DLY_ms(1);
    if(EnableRootHubPort() == ERR_SUCCESS) {  // enable the ROOT-HUB port
      i = 0;
      s++;                                    // timer waits for USB device to stabilize after connection
      if(s > (20 + t)) break;                 // has been connected stably for 15ms
    }	
    if(i) {                                   // device not connecting after reset
      DisableRootHubPort();
      #if DEBUG_ENABLE
      //printf("Disable USB port because of disconnect\n");
      #endif
    }
  }
  SelectHubPort(0);

  #if DEBUG_ENABLE
  printf("Device Descriptor: ");
  #endif

  s = CtrlGetDeviceDescr();                   // get device descriptor
  if(s == ERR_SUCCESS) {
    #if DEBUG_ENABLE
    for (i = 0; i<((PUSB_SETUP_REQ)SetupGetDevDescr)->wLengthL; i++)
      printf("0x%02X ", (uint16_t)(Com_Buffer[i]));				
    printf("\n");                             // show descriptor
    #endif

    ThisUsbDev.DeviceVID = ((PXUSB_DEV_DESCR)Com_Buffer)->idVendor;  // save VID
    ThisUsbDev.DevicePID = ((PXUSB_DEV_DESCR)Com_Buffer)->idProduct; // save PID
    dv_cls = ((PXUSB_DEV_DESCR)Com_Buffer)->bDeviceClass;   // device class code
    // Set address of USB device, RootHubIndex ensures that the two HUB ports are assigned different addresses
    s = CtrlSetUsbAddress(((PUSB_SETUP_REQ)SetupSetUsbAddr)->wValueL);
    if(s == ERR_SUCCESS) {
      ThisUsbDev.DeviceAddress = ((PUSB_SETUP_REQ)SetupSetUsbAddr)->wValueL;  // save USB address
      #if DEBUG_ENABLE
      printf("Config Descriptor: ");
      #endif
      s = CtrlGetConfigDescr();               // get configuration descriptor
      if(s == ERR_SUCCESS) {
        cfg = ((PXUSB_CFG_DESCR)Com_Buffer) -> bConfigurationValue;
        ifc = ((PXUSB_CFG_DESCR)Com_Buffer) -> bNumInterfaces;
        #if DEBUG_ENABLE
        for(i=0; i<(uint8_t)(((PXUSB_CFG_DESCR)Com_Buffer)->wTotalLength); i++)
          printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
        printf("\n");
        #endif
        // Analyze the configuration descriptor, obtain endpoint data/endpoint address/endpoint size, etc., 
        // update variables endp_addr and endp_size, etc.
        if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer)->itf_descr.bInterfaceClass; // interface class code
        // USB storage device, basically confirmed to be a U disk
        if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE) {
          AnalyzeBulkEndp(Com_Buffer, 0);
          #if DEBUG_ENABLE
          for(i=0; i!=4 ;i++)
            printf("%02x ",(uint16_t)ThisUsbDev.GpVar[i] );
          printf("\n");
          #endif
          s = CtrlSetUsbConfig(cfg);          // set USB device configuration
          if(s == ERR_SUCCESS) {
            ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
            ThisUsbDev.DeviceType = USB_DEV_CLASS_STORAGE;
            #if DEBUG_ENABLE												
            printf("USB-Disk Ready\n");
            #endif											
            SetUsbSpeed(1);                   // default is full speed
            return(ERR_SUCCESS);
          }
        }

        // Printer device
        else if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_PRINTER 
          && ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceSubClass == 0x01) {
          #if DEBUG_ENABLE										
          printf("USB-Print OK\n");
          #endif									
          if((Com_Buffer[19] == 5) && (Com_Buffer[20]&&0x80))
            ThisUsbDev.GpVar[0] = Com_Buffer[20];               // IN endpoint
          else if((Com_Buffer[19] == 5) && ((Com_Buffer[20]&&0x80) == 0))
            ThisUsbDev.GpVar[1] = Com_Buffer[20];               // OUT endpoint
          if((Com_Buffer[26] == 5) && (Com_Buffer[20]&&0x80))
            ThisUsbDev.GpVar[0] = Com_Buffer[27];               // IN endpoint
          else if((Com_Buffer[26] == 5) && ((Com_Buffer[20]&&0x80) == 0))
            ThisUsbDev.GpVar[1] = Com_Buffer[27];               // OUT endpoint
          s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
          if(s == ERR_SUCCESS)  {
            s = CtrlSetUsbIntercace(cfg);
            s = CtrlGetXPrinterReport1();                       // printer class commands
            if(s == ERR_SUCCESS) {
              ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
              ThisUsbDev.DeviceType = USB_DEV_CLASS_PRINTER;
              #if DEBUG_ENABLE														 
              printf("USB-Print Ready\n");
              #endif													 
              SetUsbSpeed(1);                                   // default is full speed
              return(ERR_SUCCESS);
            }
          }
        }

        // HID device (keyboard, mouse, etc.)
        else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) 
          && (((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceSubClass <= 0x01)) {
          // Analyze the address of the HID interrupt endpoint from the descriptor
          s = AnalyzeHidIntEndp(Com_Buffer, 0);
          #if DEBUG_ENABLE
          printf("AnalyzeHidIntEndp %02x\n",(uint16_t)s);
          #endif
          if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceProtocol;
          #if DEBUG_ENABLE
          printf("CtrlSetUsbConfig %02x\n",(uint16_t)cfg);
          #endif
          s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
          if(s == ERR_SUCCESS) {
            #if DEBUG_ENABLE
            printf("HID Report Descriptor: ");
            #endif
            for(dv_cls=0; dv_cls<ifc; dv_cls++) {
              s = CtrlGetHIDDeviceReport(dv_cls);               // get report descriptor
              if(s == ERR_SUCCESS) {
                #if DEBUG_ENABLE
                for (i=0; i<64; i++)
                  printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
                printf("\n");
                #endif
              }
            }
            //Set_Idle();
            // The endpoint information needs to be saved so that the main program can perform USB transmission
            ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;

            // Keyboard
            if(if_cls == 1) {
              ThisUsbDev.DeviceType = DEV_TYPE_KEYBOARD;
              // Further initialization, such as device keyboard indicator LED, etc.
              if(ifc > 1) {
                #if DEBUG_ENABLE
                printf("USB_DEV_CLASS_HID Ready\n");
                #endif
                ThisUsbDev.DeviceType = USB_DEV_CLASS_HID;      // composite HID device
              }
              #if DEBUG_ENABLE														
              printf("USB-Keyboard Ready\n");
              #endif
              SetUsbSpeed(1);                                   // default is full speed
              return(ERR_SUCCESS);
            }

            // Mouse
            else if(if_cls == 2) {
              ThisUsbDev.DeviceType = DEV_TYPE_MOUSE;
              // In order to query the mouse state in the future, the descriptor should 
              // be analyzed to obtain the address, length and other information of the 
              // interrupt port.
              if(ifc > 1) {
                #if DEBUG_ENABLE
                printf("USB_DEV_CLASS_HID Ready\n");
                #endif
                ThisUsbDev.DeviceType = USB_DEV_CLASS_HID;      // composite HID device
              }															
              #if DEBUG_ENABLE
              printf("USB-Mouse Ready\n");
              #endif
              SetUsbSpeed(1);                                   // default is full speed
              return(ERR_SUCCESS);
            }
            s = ERR_USB_UNSUPPORT;
          }
        }

        // HUB type device (a hub, etc.)
        else if(dv_cls == USB_DEV_CLASS_HUB) {
          // Analyze the address of the HID interrupt endpoint from the descriptor
          s = AnalyzeHidIntEndp(Com_Buffer, 0);
          #if DEBUG_ENABLE
          printf("AnalyzeHidIntEndp %02x\n", (uint16_t)s);
          printf("Hub Descriptor:");
          #endif
          s = CtrlGetHubDescr();
          if(s == ERR_SUCCESS) {
            #if DEBUG_ENABLE
            for(i=0; i<Com_Buffer[0]; i++)
              printf("0x%02X ",(uint16_t)(Com_Buffer[i]));
            printf("\n");
            #endif
            ThisUsbDev.GpHUBPortNum = ((PXUSB_HUB_DESCR)Com_Buffer) -> bNbrPorts;   // save number of HUB ports
            // Because when defining the structure DevOnHubPort, it is artificially 
            // assumed that each HUB does not exceed HUB_MAX_PORTS ports
            if(ThisUsbDev.GpHUBPortNum > HUB_MAX_PORTS)
              ThisUsbDev.GpHUBPortNum = HUB_MAX_PORTS;
            #if DEBUG_ENABLE
            printf("Hub Product\n");
            #endif
            s = CtrlSetUsbConfig(cfg);                          // set USB device configuration
            if(s == ERR_SUCCESS) {
              ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
              ThisUsbDev.DeviceType = USB_DEV_CLASS_HUB;
              // The endpoint information needs to be saved so that the main program 
              // can perform USB transmission. Originally, the interrupt endpoint can 
              // be used for HUB event notification, but this program uses query status 
              // control transmission instead.
              // Power on each port of the HUB, query the status of each port, initialize 
              // the HUB port with device connection, and initialize the device.
              for(i=1; i<=ThisUsbDev.GpHUBPortNum; i++) {       // power on each port of the HUB
                DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT; // clear status of device on external HUB port
                s = HubSetPortFeature(i, HUB_PORT_POWER);
                if(s != ERR_SUCCESS) {
                  #if DEBUG_ENABLE
                  printf("Ext-HUB Port_%1d# power on error\n", (uint16_t)i);  // failed to power on the port
                  #endif
                }
              }
              SetUsbSpeed(1);                                   // default is full speed
              return(ERR_SUCCESS);
            }
          }
        }

        // Other devices
        else {
          #if DEBUG_ENABLE
          printf("dv_cls %02x\n", (uint16_t)dv_cls);
          printf("if_cls %02x\n", (uint16_t)if_cls);
          printf("if_subcls %02x\n", (uint16_t)( (PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceSubClass);
          #endif
          AnalyzeBulkEndp(Com_Buffer , 0);                      // parse out bulk endpoints
          #if DEBUG_ENABLE
          for(i=0; i!=4; i++)
            printf("%02x ", (uint16_t)ThisUsbDev.GpVar[i]);
          printf("\n");
          #endif
          s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
          if(s == ERR_SUCCESS) {
            #if DEBUG_ENABLE						
            printf("%02x %02x\n", (uint16_t)ThisUsbDev.DeviceVID, (uint16_t)ThisUsbDev.DevicePID);
            #endif
            if((ThisUsbDev.DeviceVID == 0x18D1) && (ThisUsbDev.DevicePID&0xff00) == 0x2D00) {   // AOA accessories
              #if DEBUG_ENABLE
              printf("AOA Mode\n");
              #endif
              ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
              ThisUsbDev.DeviceType = DEF_AOA_DEVICE;           // just a custom variable class, not a USB protocol class
              SetUsbSpeed(1);                                   // default is full speed
              return(ERR_SUCCESS);
            }
            // If it is not AOA accessory mode, try starting accessory mode.
            else {
              s = TouchStartAOA();
              if(s == ERR_SUCCESS) {
                if(touchaoatm < 3) {  // limit number of AOA starts
                  touchaoatm++;
                  DLY_ms(500);        // some Android devices automatically disconnect and reconnect, so it is best to have a delay here
                  goto USBDevEnum;    // In fact, there is no need to jump here. The AOA protocol stipulates that the device will automatically reconnect to the bus.
                }
                // Execute to this point, indicating that AOA may not be supported, or other devices
                ThisUsbDev.DeviceType = dv_cls ? dv_cls : if_cls;
                ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                SetUsbSpeed(1);                                 // default is full speed
                return(ERR_SUCCESS);                            // unknown device initialized successfully
              }							
            }
          }
        }
      }
    }
  }
  #if DEBUG_ENABLE
  printf("InitRootDev Err = %02X\n", (uint16_t)s);
  #endif
  ThisUsbDev.DeviceStatus = ROOT_DEV_FAILED;
  SetUsbSpeed(1);                                               // default is full speed
  t++;
  goto USBDevEnum;
}

// ===================================================================================
// Enumerate USB devices of all ROOT-HUB ports
// ===================================================================================
uint8_t EnumAllRootDevice(void) {
  __idata uint8_t s;
  #if DEBUG_ENABLE
  printf("EnumUSBDev\n");
  #endif
  if(ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED) { // device has just been plugged in and has not been initialized
    s = InitRootDevice();                             // initialize/enumerate USB devices of specified HUB port
    if(s != ERR_SUCCESS) return(s);
  }
  return(ERR_SUCCESS);
}

// ===================================================================================
// Initialize the secondary USB device after enumerating the external HUB
// Return ERR_SUCCESS:      success
//        ERR_USB_UNKNOWN:  unknown device
// ===================================================================================
uint8_t InitDevOnHub(uint8_t HubPortIndex) {
  uint8_t i, s, cfg, dv_cls, if_cls;
  uint8_t ifc;
  #if DEBUG_ENABLE
  printf("Init dev @ExtHub-port_%1d ", (uint16_t)HubPortIndex);
  #endif
  if(HubPortIndex == 0) return(ERR_USB_UNKNOWN);

  // Select the specified port of the external HUB that operates the specified 
  // ROOT-HUB port, select the speed
  SelectHubPort(HubPortIndex);
  #if DEBUG_ENABLE
  printf("GetDevDescr: ");
  #endif
  s = CtrlGetDeviceDescr();                               // get device descriptor
  if(s != ERR_SUCCESS) return(s);
  DevOnHubPort[HubPortIndex-1].DeviceVID = ((PXUSB_DEV_DESCR)Com_Buffer)->idVendor;  // VID
  DevOnHubPort[HubPortIndex-1].DevicePID = ((PXUSB_DEV_DESCR)Com_Buffer)->idProduct; // PID

  dv_cls = ((PXUSB_DEV_DESCR)Com_Buffer) -> bDeviceClass; // device class code
  cfg = (1<<4) + HubPortIndex;                            // calculate a USB address to avoid address overlap
  s = CtrlSetUsbAddress(cfg);                             // set USB device address
  if(s != ERR_SUCCESS) return(s);
  DevOnHubPort[HubPortIndex-1].DeviceAddress = cfg;       // save the assigned USB address

  #if DEBUG_ENABLE
  printf("Config Descriptor: ");
  #endif
  s = CtrlGetConfigDescr();                               // get configuration descriptor
  if(s != ERR_SUCCESS) return(s);
  cfg = ((PXUSB_CFG_DESCR)Com_Buffer) -> bConfigurationValue;
  #if DEBUG_ENABLE
  for(i=0; i<(uint8_t)(((PXUSB_CFG_DESCR)Com_Buffer)->wTotalLength); i++)
    printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
  printf("\n");
  #endif

  // Analyze the configuration descriptor, obtain endpoint data/endpoint address/endpoint size, etc., 
  // update variables endp_addr and endp_size, etc.
  if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceClass; // interface class code

  // USB storage device, basically confirmed to be a U-disk
  if(dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE) {
    AnalyzeBulkEndp(Com_Buffer, HubPortIndex);
    #if DEBUG_ENABLE
    for(i=0; i!=4; i++)
      printf("%02x ", (uint16_t)DevOnHubPort[HubPortIndex-1].GpVar[i]);
    printf("\n");
    #endif
    s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
    if(s == ERR_SUCCESS) {
      DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
      DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_STORAGE;
      #if DEBUG_ENABLE
      printf("USB-Disk Ready\n");
      #endif
      SetUsbSpeed(1);                                     // default is full speed
      return(ERR_SUCCESS);
    }
  }

  // HID device (keyboard, mouse, etc.)
  else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) 
    && (((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceSubClass <= 0x01)) {
    ifc = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> cfg_descr.bNumInterfaces;
    // analyze the address of the HID interrupt endpoint from the descriptor
    s = AnalyzeHidIntEndp(Com_Buffer, HubPortIndex);
    #if DEBUG_ENABLE
    printf("AnalyzeHidIntEndp %02x\n", (uint16_t)s);
    #endif
    if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceProtocol;
    s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
    if(s == ERR_SUCCESS) {
      for(dv_cls=0; dv_cls<ifc; dv_cls++) {
        s = CtrlGetHIDDeviceReport(dv_cls);               // get report descriptor
        if(s == ERR_SUCCESS) {
          #if DEBUG_ENABLE
          for(i=0; i<64; i++)
            printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
          printf("\n");
          #endif
        }
      }

      // The endpoint information needs to be saved so that the main program can 
      // perform USB transmission
      DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;

      // Keyboard
      if(if_cls == 1) {
        DevOnHubPort[HubPortIndex-1].DeviceType = DEV_TYPE_KEYBOARD;
        // Further initialization, such as device keyboard indicator LED, etc.
        if(ifc > 1) {
          #if DEBUG_ENABLE
          printf("USB_DEV_CLASS_HID Ready\n");
          #endif
          DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_HID;  // composite HID device
        }
        #if DEBUG_ENABLE
        printf("USB-Keyboard Ready\n");
        #endif
        SetUsbSpeed(1);                                   // default is full speed
        return(ERR_SUCCESS);
      }

      // Mouse
      else if(if_cls == 2) {
        DevOnHubPort[HubPortIndex-1].DeviceType = DEV_TYPE_MOUSE;
        // In order to query the mouse state in the future, the descriptor should 
        // be analyzed to obtain the address, length and other information of the 
        // interrupt port.
        if(ifc > 1) {
          #if DEBUG_ENABLE
          printf("USB_DEV_CLASS_HID Ready\n");
          #endif
          DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_HID;  // composite HID device
        }
        #if DEBUG_ENABLE
        printf("USB-Mouse Ready\n");
        #endif
        SetUsbSpeed(1);                                   // default is full speed
        return(ERR_SUCCESS);
      }
      s = ERR_USB_UNSUPPORT;
    }
  }

  // HUB type device (a hub, etc.)
  else if(dv_cls == USB_DEV_CLASS_HUB) {
    DevOnHubPort[HubPortIndex-1].DeviceType = USB_DEV_CLASS_HUB;
    #if DEBUG_ENABLE
    // Need to support multi-level HUB cascading, please refer to this program for expansion
    printf("This program don't support Level 2 HUB\n");
    #endif
    s = HubClearPortFeature(i, HUB_PORT_ENABLE);          // disable HUB port
    if(s != ERR_SUCCESS) return(s);
    s = ERR_USB_UNSUPPORT;
  }

  // Other devices
  else {
    AnalyzeBulkEndp(Com_Buffer , HubPortIndex);           // parse out bulk endpoints
    #if DEBUG_ENABLE
    for(i=0; i!=4; i++)
      printf("%02x ", (uint16_t)DevOnHubPort[HubPortIndex-1].GpVar[i]);
    printf("\n");
    #endif
    s = CtrlSetUsbConfig(cfg);                            // set USB device configuration
    if(s == ERR_SUCCESS) {
      // The endpoint information needs to be saved so that the main program can perform USB transmission
      DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
      DevOnHubPort[HubPortIndex-1].DeviceType = dv_cls ? dv_cls : if_cls;
      SetUsbSpeed(1);                                     // default is full speed
      return(ERR_SUCCESS);                                // unknown device initialized successfully
    }
  }
  #if DEBUG_ENABLE
  printf("InitDevOnHub Err = %02X\n", (uint16_t)s);
  #endif
  DevOnHubPort[HubPortIndex-1].DeviceStatus = ROOT_DEV_FAILED;
  SetUsbSpeed(1);                                         // default is full speed
  return(s);
}

// ===================================================================================
// Enumerates each port of the external HUB on the specified ROOT-HUB port, checks 
// whether each port has a connection or removal event and initializes the secondary 
// USB device.
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t EnumHubPort(void) {
  uint8_t i, s;
  for(i=1; i<=ThisUsbDev.GpHUBPortNum; i++) {   // query whether the port of the hub has changed
    SelectHubPort(0);                           // select to operate designated ROOT-HUB port, set current USB speed and USB address of operated device
    s = HubGetPortStatus(i);                    // get port status
    if(s != ERR_SUCCESS) return(s);             // maybe HUB is disconnected

    // Found that there is a device connected
    if(((Com_Buffer[0]&(1<<(HUB_PORT_CONNECTION&0x07))) 
      && (Com_Buffer[2]&(1<<(HUB_C_PORT_CONNECTION&0x07)))) 
      || (Com_Buffer[2] == 0x10)) {
      DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_CONNECTED;  // there is a device connected
      DevOnHubPort[i-1].DeviceAddress = 0x00;
      s = HubGetPortStatus(i);                    // get port status
      if(s != ERR_SUCCESS) return(s);             // maybe HUB is disconnected

      DevOnHubPort[i-1].DeviceSpeed = Com_Buffer[1] & (1<<(HUB_PORT_LOW_SPEED&0x07)) ? 0 : 1; // low speed or full speed
      #if DEBUG_ENABLE
      if(DevOnHubPort[i-1].DeviceSpeed)
        printf("Found full speed device on port %1d\n", (uint16_t)i);
      else
        printf("Found low speed device on port %1d\n", (uint16_t)i);
      #endif

      DLY_ms(200);                                // wait for device to power on and stabilize
      s = HubSetPortFeature(i, HUB_PORT_RESET);   // reset port with device connection
      if(s != ERR_SUCCESS) return(s);             // maybe the HUB is disconnected
      #if DEBUG_ENABLE
      printf("Reset port and then wait in\n");
      #endif

      // Query reset port until reset is completed, and display completed status
      do {
        DLY_ms(1);
        s = HubGetPortStatus(i);
        if(s != ERR_SUCCESS) return(s);                     // maybe the HUB is disconnected
      } while(Com_Buffer[0] & (1<<(HUB_PORT_RESET&0x07)));  // port is reset, wait
      DLY_ms(100);
      s = HubClearPortFeature(i, HUB_C_PORT_RESET);         // clear reset complete flag
      //s = HubSetPortFeature(i, HUB_PORT_ENABLE);          // enable HUB port
      s = HubClearPortFeature(i, HUB_C_PORT_CONNECTION);    // clear connection or remove change flag
      if(s != ERR_SUCCESS) return(s);
      s = HubGetPortStatus(i);                              // read status again and check whether device is still there
      if(s != ERR_SUCCESS) return(s);
      if((Com_Buffer[0]&(1<<(HUB_PORT_CONNECTION&0x07))) == 0)
        DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT; // device is gone
      s = InitDevOnHub( i );                                // initialize secondary USB device
      if(s != ERR_SUCCESS) return(s);
      SetUsbSpeed(1);                                       // default is full speed
    }
    else if(Com_Buffer[2]&(1<<(HUB_C_PORT_ENABLE&0x07))) {  // device connection error
      HubClearPortFeature(i, HUB_C_PORT_ENABLE);            // clear connection error flag
      #if DEBUG_ENABLE
      printf("Device on port error\n");
      #endif
      s = HubSetPortFeature(i, HUB_PORT_RESET);             // reset port with device
      if(s != ERR_SUCCESS) return(s);                       // maybe the HUB is disconnected

      // Query reset port until reset is completed, and display completed status
      do {
        DLY_ms(1);
        s = HubGetPortStatus(i);
        if(s != ERR_SUCCESS) return(s);                     // maybe the HUB is disconnected
      } while(Com_Buffer[0] & (1<<(HUB_PORT_RESET&0x07)));  // port is reset, wait
    }
    else if((Com_Buffer[0]&(1<<(HUB_PORT_CONNECTION&0x07))) == 0) { // device disconnected
      if(DevOnHubPort[i-1].DeviceStatus >= ROOT_DEV_CONNECTED) {
        #if DEBUG_ENABLE
        printf("Device on port %1d removed\n", (uint16_t)i);
        #endif
      }
      DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT; // there is a device connected
      if(Com_Buffer[2]&(1<<(HUB_C_PORT_CONNECTION&0x07)))
        HubClearPortFeature(i, HUB_C_PORT_CONNECTION);      // clear remove change flag
    }
  }
  return(ERR_SUCCESS);                                      // return operation successful
}

// ===================================================================================
// Enumerate all secondary USB devices behind the external HUB under the ROOT-HUB port
// Return ERR_SUCCESS:      success
// ===================================================================================
uint8_t EnumAllHubPort(void) {
  uint8_t s;
  if((ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS) 
    && (ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB)) { // HUB enumeration succeeded
    // Select to operate designated ROOT-HUB port, set current USB speed and 
    // USB address of operated device
    SelectHubPort(0);
    // Power on each port of HUB, query status of each port, initialize HUB port 
    // with device connection, and initialize device
    //for(i=1; i<=ThisUsbDev.GpVar; i++) {        // initialize each port of HUB
      //s = HubSetPortFeature(i, HUB_PORT_POWER); // power on each port of HUB
      //if(s != ERR_SUCCESS) return(s);           // maybe the HUB is disconnected
    //}

    // Enumerate each port of external HUB hub on specified ROOT-HUB port, and 
    // check whether each port has a connection or removal event
    s = EnumHubPort();
    if(s != ERR_SUCCESS) {                        // maybe the HUB is disconnected
      #if DEBUG_ENABLE
      printf("EnumAllHubPort err = %02X\n", (uint16_t)s);
      #endif
    }
    SetUsbSpeed(1);                               // default is full speed
  }
  return(ERR_SUCCESS);
}

// ===================================================================================
// Search for the port number of the specified type of device on each port of ROOT-HUB
// and external HUB, if the output port number is 0xFFFF, it cannot be found.
// Input:   The type of device being searched for
// Return:  The high 8 bits of the output are the ROOT-HUB port number, the low 8 bits 
//          are the port number of the external HUB, and if the low 8 bits are 0, the 
//          device is directly on the ROOT-HUB port.
//          Of course, you can also search according to the PID of the USB manufacturer's 
//          VID product (record the VID and PID of each device in advance), and specify 
//          the search number.
// ===================================================================================
uint16_t SearchTypeDevice(uint8_t type) {
  // CH554 has only one USB port, RootHubIndex = 0, just look at the lower eight bits 
  // of the return value
  uint8_t RootHubIndex;
  uint8_t HubPortIndex;
  RootHubIndex = 0;
  if((ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB) 
    && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS)) { // external HUB and enumeration is successful

    // Search each port of the external HUB
    for(HubPortIndex = 1; HubPortIndex <= ThisUsbDev.GpHUBPortNum; HubPortIndex++) {
      if(DevOnHubPort[HubPortIndex-1].DeviceType == type && DevOnHubPort[HubPortIndex-1].DeviceStatus >= ROOT_DEV_SUCCESS)
        return(((uint16_t)RootHubIndex << 8 ) | HubPortIndex);  // type matches and enumeration succeeds
    }
  }
  if((ThisUsbDev.DeviceType == type) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS))
    return((uint16_t)RootHubIndex << 8);  // type matches and enumeration is successful, on the ROOT-HUB port
  return( 0xFFFF );
}

// ===================================================================================
// NumLock lighting judgment
// Input:   key
// ===================================================================================
uint8_t SETorOFFNumLock(uint8_t *buf) {
  uint8_t tmp[]= {0x21,0x09,0x00,0x02,0x00,0x00,0x01,0x00};
  uint8_t len,s;
  if((buf[2]==0x53)&(buf[0]|buf[1]|buf[3]|buf[4]|buf[5]|buf[6]|buf[7]==0)) {
    if(HubLowSpeed) {                                 // Low-speed devices under HUB
      ((__xdata uint8_t *)pSetupReq)[0] = 0X21;					
      for(s=1; s!=sizeof(tmp)+1; s++)
        ((__xdata uint8_t *)pSetupReq)[s] = tmp[s];
    }
    else {
      for(s=0; s!=sizeof(tmp); s++)
        ((__xdata uint8_t *)pSetupReq)[s] = tmp[s];
    }
    s = HostCtrlTransfer(Com_Buffer, &len);           // execute control transfer
    if(s != ERR_SUCCESS) return(s);
  }
  return(ERR_SUCCESS);
}
#endif

// ===================================================================================
// Initialize the USB device
// ===================================================================================
#ifdef DISK_BASE_BUF_LEN
uint8_t	InitRootDevice(void) {
  uint8_t i, s, cfg, dv_cls, if_cls;
  #if DEBUG_ENABLE
  printf("Reset host port\n");
  #endif
  ResetRootHubPort();                 // after detecting device, reset USB bus of corresponding port
  for(i=0, s=0; i<100; i++) {         // wait for USB device to reset and reconnect, 100mS timeout
    DLY_ms(1);
    if(EnableRootHubPort() == ERR_SUCCESS) {  // enable port
      i = 0;
      s++;                            // timer waits for USB device to stabilize after connection
      if(s > 100) break;              // has been connected stably for 100mS
    }
  }
  if(i) {                             // device not connected after reset
    DisableRootHubPort();
    #if DEBUG_ENABLE
    printf("Disable host port because of disconnect\n");
    #endif
    return(ERR_USB_DISCON);
  }
  SetUsbSpeed(1);                     // set current USB speed
  s = CtrlGetDeviceDescr();           // get device descriptor
  if(s == ERR_SUCCESS) {
    #if DEBUG_ENABLE
    printf("Device Descriptor: ");
    for(i=0; i<((PUSB_SETUP_REQ)SetupGetDevDescr)->wLengthL; i++) 
      printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
    printf("\n");                     // show descriptor
    #endif
    dv_cls = ((PXUSB_DEV_DESCR)Com_Buffer) -> bDeviceClass;               // device class code
    s = CtrlSetUsbAddress(((PUSB_SETUP_REQ)SetupSetUsbAddr) -> wValueL);  // set USB device address
    if(s == ERR_SUCCESS) {
      s = CtrlGetConfigDescr();       // get configuration descriptor
      if(s == ERR_SUCCESS) {
        cfg = ((PXUSB_CFG_DESCR)Com_Buffer) -> bConfigurationValue;
        #if DEBUG_ENABLE
        printf("Config Descriptor: ");
        for(i=0; i<((PXUSB_CFG_DESCR)Com_Buffer)->wTotalLengthL; i++) 
          printf("0x%02X ", (uint16_t)(Com_Buffer[i]));
        printf("\n");
        #endif

        // Analyze configuration descriptor, obtain endpoint data/endpoint address/endpoint size, etc., 
        // update variables endp_addr and endp_size, etc.
        if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceClass; // interface class code
        if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_STORAGE)) { // USB storage device, basically confirmed to be a U-disk					
          //s = CtrlSetUsbConfig(cfg);                              // set USB device configuration
          //if(s == ERR_SUCCESS) {
            CH554DiskStatus = DISK_USB_ADDR;
            return(ERR_SUCCESS);
          //}
          //else return(ERR_USB_UNSUPPORT);
        }
        else return(ERR_USB_UNSUPPORT);
      }
    }
  }
  #if DEBUG_ENABLE
  printf("InitRootDev Err = %02X\n", (uint16_t)s);
  #endif
  CH554DiskStatus = DISK_CONNECT;
  SetUsbSpeed(1);                     // default is full speed
  return(s);
}
#endif

// ===================================================================================
// Initialize the USB host
// ===================================================================================
void InitUSB_Host(void) {
  uint8_t i;
  IE_USB  = 0;
//LED_CFG = 1;
//LED_RUN = 0;
  USB_CTRL    =  bUC_HOST_MODE;                 // set mode first
  UHOST_CTRL &= ~bUH_PD_DIS;                    // enable host pulldown
  USB_DEV_AD  =  0x00;
  UH_EP_MOD   =  bUH_EP_TX_EN 
              |  bUH_EP_RX_EN ;
  UH_RX_DMA   =  (uint16_t)RxBuffer;
  UH_TX_DMA   =  (uint16_t)TxBuffer;
  UH_RX_CTRL  =  0x00;
  UH_TX_CTRL  =  0x00;
  USB_CTRL    =  bUC_HOST_MODE                  // start USB host
//            |  bUC_DMA_EN                     // enable DMA
              |  bUC_INT_BUSY;                  // automatically suspend before the interrupt flag is cleared
//UHUB0_CTRL  =  0x00;
//UHUB1_CTRL  =  0x00;
//UH_SETUP    =  bUH_SOF_EN;
  USB_INT_FG  =  0xFF;                          // clear interrupt flag
  for(i=0; i!=2; i++) DisableRootHubPort();     // empty
  USB_INT_EN  =  bUIE_TRANSFER | bUIE_DETECT;
//IE_USB = 1;                                   // query mode
}
