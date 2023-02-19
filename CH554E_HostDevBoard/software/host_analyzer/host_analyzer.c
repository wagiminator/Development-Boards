/********************************** (C) COPYRIGHT *******************************
* File Name          : USBHostHUB_KM.C
* Author             : WCH
* Version            : V2.0
* Date               : 2018/07/24
*******************************************************************************/

// Modified by Stefan Wagner 2023
// Transfers information on connected USB devices via USB serial.
// Use a serial monitor @ 9600 BAUD

#include <debug.h>
#include <delay.h>
#include <system.h>
#include <usb_host.h>
#include <string.h>

__code uint8_t  SetupGetDevDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof( USB_DEV_DESCR ), 0x00 };
__code uint8_t  SetupGetCfgDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00 };
__code uint8_t  SetupSetUsbAddr[] = { USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00 };
__code uint8_t  SetupSetUsbConfig[] = { USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
__code uint8_t  SetupSetUsbInterface[] = { USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
__code uint8_t  SetupClrEndpStall[] = { USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
__code uint8_t  SetupGetHubDescr[] = { HUB_GET_HUB_DESCRIPTOR, HUB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_HUB, 0x00, 0x00, sizeof( USB_HUB_DESCR ), 0x00 };
__code uint8_t  SetupSetHIDIdle[]= { 0x21,HID_SET_IDLE,0x00,0x00,0x00,0x00,0x00,0x00 };
__code uint8_t  SetupGetHIDDevReport[] = { 0x81, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_REPORT, 0x00, 0x00, 0xFF, 0x00 };
__code uint8_t  XPrinterReport[] = { 0xA1, 0, 0x00, 0, 0x00, 0x00, 0xF1, 0x03 };
__code uint8_t  GetProtocol[] = { 0xc0,0x33,0x00,0x00,0x00,0x00,0x02,0x00 };
__code uint8_t  TouchAOAMode[] = { 0x40,0x35,0x00,0x00,0x00,0x00,0x00,0x00 };

__code uint8_t  Sendlen[]= {0,4,16,35,39,53,67};
__code uint8_t  StringID[] = {
  'W','C','H',0x00,                                                                               // manufacturer name
  'W','C','H','U','A','R','T','D','e','m','o',0x00,                                               // model name
  0x57,0x43,0x48,0x20,0x41,0x63,0x63,0x65,0x73,0x73,0x6f,0x72,0x79,0x20,0x54,0x65,0x73,0x74,0x00, // description
  '1','.','0',0x00,                                                                               // version
  0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x63,0x68,0x2e,0x63,0x6e,0x00,                          // URI
  0x57,0x43,0x48,0x41,0x63,0x63,0x65,0x73,0x73,0x6f,0x72,0x79,0x31,0x00                           // serial number
};
__code uint8_t  SetStringID[]= {
  0x40,0x34,0x00,0x00,0x00,0x00,0x04,0x00,
  0x40,0x34,0x00,0x00,0x01,0x00,12,0x00,
  0x40,0x34,0x00,0x00,0x02,0x00,19,0x00,
  0x40,0x34,0x00,0x00,0x03,0x00,4,0x00,
  0x40,0x34,0x00,0x00,0x04,0x00,0x0E,0x00,
  0x40,0x34,0x00,0x00,0x05,0x00,0x0E,0x00
};

__xdata __at (0x0000) uint8_t RxBuffer[MAX_PACKET_SIZE];  // IN, must even address
__xdata __at (0x0040) uint8_t TxBuffer[MAX_PACKET_SIZE];  // OUT, must even address

__xdata uint8_t UsbDevEndp0Size;                          // maximum packet size for endpoint 0 of a USB device
__xdata _RootHubDev ThisUsbDev;                           // ROOT
__xdata _DevOnHubPort DevOnHubPort[HUB_MAX_PORTS];        // assumption: no more than 1 external HUB, each external HUB does not exceed HUB_MAX_PORTS ports (don’t care if there are more)

__bit RootHubId;                                          // currently operating root-hub port number: 0=HUB0, 1=HUB1
__bit FoundNewDev;

uint8_t Set_Port = 0;

void main(void) {
  // Variables
  uint8_t i, s, k, len, endp;
  uint16_t loc;

  // Setup
  CLK_config();
  DLY_ms(50);
  DEBUG_init();
  printf("Start @ChipID=%02X\n", (uint16_t)CHIP_ID);
  InitUSB_Host();
  FoundNewDev = 0;
  printf("Wait Device In\n");

  // Loop
  while(1) {
    s = ERR_SUCCESS;
    if(UIF_DETECT) {                                      // handle if there is a USB host detection interrupt
      UIF_DETECT = 0;                                     // clear interrupt flag
      s = AnalyzeRootHub();                               // analyze ROOT-HUB status
      if (s == ERR_USB_CONNECT) FoundNewDev = 1;
    }
    if(FoundNewDev) {                                     // there is a new USB device plugged in...
      FoundNewDev = 0;
      //DLY_ms(200);                                      // wait to eliminate (un)plugging jitter
      s = EnumAllRootDevice();                            // enumerate USB devices of all ROOT-HUB ports
      if(s != ERR_SUCCESS) {
        printf("EnumAllRootDev err = %02X\n", (uint16_t)s);					
      }
    }

    // If CH554 is connected to a HUB, first enumerate all secondary USB devices 
    // behind the external HUB under the ROOT-HUB port
    s = EnumAllHubPort();
    if(s != ERR_SUCCESS) {                                // maybe the HUB is disconnected
      printf("EnumAllHubPort err = %02X\n", (uint16_t)s);
    }

    // Operating a mouse
    loc = SearchTypeDevice(DEV_TYPE_MOUSE);               // search for port number of specified type of device on each port of ROOT-HUB and external HUB
    if(loc != 0xFFFF) {                                   // found a mouse (how to deal with two mice?)
      loc = (uint8_t)loc;                                 // CH554 has only one USB, only the lower eight bits are required
      //printf("Query Mouse @%04X\n", loc);
      SelectHubPort(loc);                                 // select to operate designated ROOT-HUB port, set current USB speed and USB address of operated device
      endp = loc ? DevOnHubPort[loc-1].GpVar[0] : ThisUsbDev.GpVar[0];  // address of interrupt endpoint, bit 7 is used for synchronization flag bit
      if(endp & USB_ENDP_ADDR_MASK) {                     // endpoint valid
        s = USBHostTransact(USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0); // CH554 transmit transaction, get data, NAK does not retry
        if(s == ERR_SUCCESS) {
          endp ^= 0x80;                                   // flip sync flag
          if(loc) DevOnHubPort[loc-1].GpVar[0] = endp;    // save synchronization flag
          else ThisUsbDev.GpVar[0] = endp;
          len = USB_RX_LEN;                               // received data length
          if(len) {
            printf("Mouse @%02X data: ", loc);
            for(i=0; i<len; i++) {
              printf("0x%02X ", (uint16_t)(RxBuffer[i]));
            }
            printf("\n");
          }
        }
        else if(s != (USB_PID_NAK | ERR_USB_TRANSFER)) {
          printf("Mouse error %02x\n", (uint16_t)s);      // may be disconnected
        }
      }
      else {
        printf("Mouse has no interrupt endpoint\n");
      }
      SetUsbSpeed(1);                                     // default is full speed
    }

    // Operating a keyboard
    loc = SearchTypeDevice(DEV_TYPE_KEYBOARD);            // search for port number of specified type of device on each port of ROOT-HUB and external HUB
    if(loc != 0xFFFF) {                                   // found a keyboard (how to deal with two keyboards?)
      loc = (uint8_t)loc;                                 // CH554 has only one USB, only the lower eight bits are required
      //printf("Query Keyboard @%04X\n", loc);
      SelectHubPort(loc);                                 // select to operate designated ROOT-HUB port, set current USB speed and USB address of operated device
      endp = loc ? DevOnHubPort[loc-1].GpVar[0] : ThisUsbDev.GpVar[0];  // address of interrupt endpoint, bit 7 is used for synchronization flag bit
      //printf("%02X  ", endp);
      if(endp & USB_ENDP_ADDR_MASK) {                     // endpoint valid
        s = USBHostTransact(USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0);// CH554 transmit transaction, get data, NAK does not retry
        if(s == ERR_SUCCESS) {
          endp ^= 0x80;                                   // flip sync flag
          if(loc) DevOnHubPort[loc-1].GpVar[0] = endp;    // save synchronization flag
          else ThisUsbDev.GpVar[0] = endp;
          len = USB_RX_LEN;                               // received data length
          if(len) {
            SETorOFFNumLock(RxBuffer);
            printf("Keyboard @%02X data: ", loc);
            for(i=0; i<len; i++) {
              printf("0x%02X ", (uint16_t)(RxBuffer[i]));
            }
            printf("\n");
          }
        }
        else if(s != (USB_PID_NAK | ERR_USB_TRANSFER)) {
         printf("Keyboard error %02x\n", (uint16_t)s);    // may be disconnected
        }
      }
      else {
        printf("Keyboard has no interrupt endpoint\n");
      }
      SetUsbSpeed(1);                                     // default is full speed
    }

    // Operating a USB printer
    if(TIN0 == 0) {                                       // P10 is low, start printing
      memset(TxBuffer, 0, sizeof(TxBuffer));
      TxBuffer[0]=0x1B; TxBuffer[1]=0x40; TxBuffer[2]=0x1D; TxBuffer[3]=0x55;
      TxBuffer[4]=0x42; TxBuffer[5]=0x02; TxBuffer[6]=0x18; TxBuffer[7]=0x1D;
      TxBuffer[8]=0x76; TxBuffer[9]=0x30; TxBuffer[10]=0x00;TxBuffer[11]=0x30;
      TxBuffer[12]=0x00;TxBuffer[13]=0x18;
      loc = SearchTypeDevice(USB_DEV_CLASS_PRINTER);      // search for port number of specified type of device on each port of ROOT-HUB and external HUB
      if(loc != 0xFFFF) {                                 // found a printer (how to deal with two printers?)
        loc = (uint8_t)loc;                               // CH554 has only one USB, only the lower eight bits are required
        printf("Query Printer @%02X\n", loc);
        SelectHubPort(loc);                               // select to operate designated ROOT-HUB port, set current USB speed and USB address of operated device
        endp = loc ? DevOnHubPort[loc-1].GpVar[0] : ThisUsbDev.GpVar[0];  // address of interrupt endpoint, bit 7 is used for synchronization flag bit
        printf("Endpoint: %02X  ",endp);  
        if(endp & USB_ENDP_ADDR_MASK) {                   // endpoint valid
          UH_TX_LEN = 64;                                 // default state of no data is IN
          s = USBHostTransact(USB_PID_OUT << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0xffff); // CH554 transmit transaction, get data, NAK does not retry
          if(s == ERR_SUCCESS) {
            endp ^= 0x80;                                 // flip sync flag
            memset(TxBuffer, 0, sizeof(TxBuffer));
            UH_TX_LEN = 64;                               // default state of no data is IN
            s = USBHostTransact(USB_PID_OUT << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0xffff); // CH554 transmit transaction, get data, NAK does not retry
          }
          else if(s != (USB_PID_NAK | ERR_USB_TRANSFER)) printf("Printer error %02x\n", (uint16_t)s); // may be disconnected
        }
      }
    }				

    // Operating a HID Composite Device
    loc = SearchTypeDevice(USB_DEV_CLASS_HID);            // search for port number of specified type of device on each port of ROOT-HUB and external HUB
    if(loc != 0xFFFF) {                                   // found it
      //printf("Query HID device @%04X\n", loc);	
      loc = (uint8_t)loc;                                 // CH554 has only one USB, only the lower eight bits are required
      for(k=0; k!=4; k++) {
        // Is the endpoint valid?
        endp = loc ? DevOnHubPort[loc-1].GpVar[k] : ThisUsbDev.GpVar[k];  // address of interrupt endpoint, bit 7 is used for synchronization flag bit
        if((endp & USB_ENDP_ADDR_MASK) == 0) break;
        //printf("endp: %02X\n", (uint16_t)endp);
        SelectHubPort(loc);                               // select to operate designated ROOT-HUB port, set current USB speed and USB address of operated device
        s = USBHostTransact(USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0); // CH554 transmit transaction, get data, NAK does not retry
        if(s == ERR_SUCCESS) {
          endp ^= 0x80;                                   // flip sync flag
          if(loc) DevOnHubPort[loc-1].GpVar[k] = endp;    // save synchronization flag
          else ThisUsbDev.GpVar[k] = endp;
          len = USB_RX_LEN;                               // received data length
          if(len) {
            printf("Endpoint: %02X\n", (uint16_t)endp);
            printf("HID device @%04X data: ", loc);
            for(i=0; i<len; i++) {
              printf("0x%02X ", (uint16_t)(RxBuffer[i]));
            }
            printf("\n");
          }
        }
        else if(s != (USB_PID_NAK | ERR_USB_TRANSFER)) {
          printf("HID device error %02x\n", (uint16_t)s); // may be disconnected
        }
      }								
      SetUsbSpeed(1);                                     // default is full speed
    }            					

    // Operating manufacturer's device, possibly a mobile phone, it will try to start in AOA mode first
    loc = SearchTypeDevice(DEF_AOA_DEVICE);               // find AOA
    if(loc != 0xFFFF) {                                   // found it
      loc = (uint8_t)loc;                                 // usb_host.c only supports Android operation under ROOTHUB, no need to analyze loc
      endp = ThisUsbDev.GpVar[0];                         // prepare to send IN packets to upload endpoint
      if((endp & USB_ENDP_ADDR_MASK) != 0) {              // endpoint valid
        SelectHubPort(0);                                 // select to operate designated ROOT-HUB port, set current USB speed and USB address of operated device
        s = USBHostTransact(USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0); // CH554 transmit transaction, get data, NAK does not retry
        if(s == ERR_SUCCESS) {
          endp ^= 0x80;                                   // flip sync flag
          ThisUsbDev.GpVar[0] = endp;                     // save synchronization flag
          len = USB_RX_LEN;                               // received data length
          for(i=0; i<len; i++) {
            printf("0x%02X ", (uint16_t)(RxBuffer[i]));
          }
          printf("\n");					
          if(len) {
            memcpy(TxBuffer, RxBuffer, len);              // return
            endp = ThisUsbDev.GpVar[2];                   // downlink endpoint sends OUT packets
            UH_TX_LEN = len; 
            s = USBHostTransact(USB_PID_OUT << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0xffff); // unlimited download retries
            if(s == ERR_SUCCESS) {
              endp ^= 0x80;                               // flip sync flag
              ThisUsbDev.GpVar[2] = endp;                 // save synchronization flag
              printf("Send back\n");
            }
          }
        }
        else if(s != (USB_PID_NAK | ERR_USB_TRANSFER)) {
          printf("Transmit error %02x\n", (uint16_t)s);   // may be disconnected
        }					
      }					
      SetUsbSpeed(1);                                     // default is full speed
    } 
  }
}
