// ===================================================================================
// USB Host Functions for CH554
// ===================================================================================

#pragma once
#include <stdint.h>
#include "ch554.h"
#include "usb.h"

// Each subroutine returns a status code
#define ERR_SUCCESS         0x00    // successful operation
#define ERR_USB_CONNECT     0x15    // a USB device connection event has been detected and has been connected
#define ERR_USB_DISCON      0x16    // a USB device disconnection event has been detected and has been disconnected
#define ERR_USB_BUF_OVER    0x17    // the data transmitted by USB is wrong or too much data buffer overflow
#define ERR_USB_DISK_ERR    0x1F    // The USB storage operation failed. It may be that the USB storage does not support during initialization, and the disk may be damaged or disconnected during the read and write operations.
#define ERR_USB_TRANSFER    0x20    // NAK/STALL and other error codes are in 0x20~0x2F
#define ERR_USB_UNSUPPORT   0xFB    // unsupported USB device
#define ERR_USB_UNKNOWN     0xFE    // device operation error
#define ERR_AOA_PROTOCOL    0x41    // wrong protocol version

// USB device related information table, CH554 supports up to 1 device
#define ROOT_DEV_DISCONNECT  0
#define ROOT_DEV_CONNECTED   1
#define ROOT_DEV_FAILED      2
#define ROOT_DEV_SUCCESS     3
#define DEV_TYPE_KEYBOARD    (USB_DEV_CLASS_HID | 0x20)
#define DEV_TYPE_MOUSE       (USB_DEV_CLASS_HID | 0x30)
#define DEF_AOA_DEVICE       0xF0

// Convention: USB device address allocation rules (refer to USB_DEVICE_ADDR)
// Address  Device location
// 0x02     Built-in USB device under Root-HUB or external HUB
// 0x1x     The USB device under the port x of the external HUB under the built-in Root-HUB, where x is 1~n
#define HUB_MAX_PORTS        4
#define WAIT_USB_TOUT_200US  400    // waiting for USB interrupt timeout time 200uS@Fsys=12MHz

// Array size definition
#define COM_BUF_SIZE         120    // can be dynamically modified to save memory based on the maximum descriptor size

extern __code uint8_t  SetupGetDevDescr[];      // get device descriptor
extern __code uint8_t  SetupGetCfgDescr[];      // get configuration descriptor
extern __code uint8_t  SetupSetUsbAddr[];       // set USB address
extern __code uint8_t  SetupSetUsbConfig[];     // set usb configuration
extern __code uint8_t  SetupSetUsbInterface[];  // set the USB interface configuration
extern __code uint8_t  SetupClrEndpStall[];     // clear endpoint STALL
#ifndef DISK_BASE_BUF_LEN
extern __code uint8_t  SetupGetHubDescr[];      // get HUB descriptor
extern __code uint8_t  SetupSetHIDIdle[];
extern __code uint8_t  SetupGetHIDDevReport[];  // get the HID device report descriptor
extern __code uint8_t  XPrinterReport[];        // printer class commands
#endif
extern __xdata uint8_t  UsbDevEndp0Size;        // maximum packet size for endpoint 0 of a USB device

extern __code uint8_t  GetProtocol[];           // AOA obtains the protocol version
extern __code uint8_t  TouchAOAMode[];          // start accessory mode
extern __code uint8_t  Sendlen[];               // AOA related array definition
extern __code uint8_t  StringID[];              // string ID, string information related to mobile APP
extern __code uint8_t  SetStringID[];           // apply index string command

#ifndef DISK_BASE_BUF_LEN
typedef struct {
  uint8_t   DeviceStatus;     // device status, 0-no device, 1-device but not yet initialized, 2-device but initialization enumeration failed, 3-device and initialization enumeration succeeded
  uint8_t   DeviceAddress;    // the USB address assigned to the device
  uint8_t   DeviceSpeed;      // 0 means low speed, non-zero means full speed
  uint8_t   DeviceType;       // device type
  uint16_t  DeviceVID;
  uint16_t  DevicePID;
  uint8_t   GpVar[4];         // general variable, store the endpoint
  uint8_t   GpHUBPortNum;     // general variable, if it is a HUB, it indicates the number of HUB ports
} _RootHubDev;

typedef struct {
  uint8_t   DeviceStatus;     // device status, 0-no device, 1-device but not yet initialized, 2-device but initialization enumeration failed, 3-device and initialization enumeration succeeded
  uint8_t   DeviceAddress;    // the USB address assigned to the device
  uint8_t   DeviceSpeed;      // 0 means low speed, non-zero means full speed
  uint8_t   DeviceType;       // device type
  uint16_t  DeviceVID;
  uint16_t  DevicePID;
  uint8_t   GpVar[4];         // common variable
} _DevOnHubPort;              // assumption: no more than 1 external HUB, each external HUB does not exceed HUB_MAX_PORTS ports (don’t care if there are more)

extern __xdata _RootHubDev ThisUsbDev;
extern __xdata _DevOnHubPort DevOnHubPort[HUB_MAX_PORTS]; // assumption: no more than 1 external HUB, each external HUB does not exceed HUB_MAX_PORTS ports (don’t care if there are more)
extern uint8_t Set_Port;
#endif

extern __xdata uint8_t  Com_Buffer[];
extern __bit  FoundNewDev;
extern __bit  HubLowSpeed;    // Low-speed devices under the HUB need special handling

#define pSetupReq ((PXUSB_SETUP_REQ)TxBuffer)

void    DisableRootHubPort(void);         // close the ROOT-HUB port, in fact, the hardware has been automatically closed, here is just to clear some structural status
uint8_t AnalyzeRootHub(void);             // analyze the status of ROOT-HUB and handle the event of device plugging and unplugging on the ROOT-HUB port
// Return ERR_SUCCESS for no case, return ERR_USB_CONNECT for detected new connection, return ERR_USB_DISCON for detected disconnection
void    SetHostUsbAddr(uint8_t addr);     // set the address of the USB device currently operated by the USB host
void    SetUsbSpeed(uint8_t FullSpeed);   // set the current USB speed
void    ResetRootHubPort(void);           // after the device is detected, reset the bus of the corresponding port, prepare for enumerating the device, and set it to default to full speed
uint8_t EnableRootHubPort(void);          // enable the ROOT-HUB port, set the corresponding bUH_PORT_EN to 1 to open the port, and the disconnection of the device may cause the return failure
void    SelectHubPort(uint8_t HubPortIndex);  // HubPortIndex=0 select to operate the designated ROOT-HUB port, otherwise select the designated port of the external HUB to operate the designated ROOT-HUB port
uint8_t WaitUSB_Interrupt(void);          // wait for usb interrupt
// CH554 transmission transaction, input destination endpoint address/PID token, synchronization flag, NAK retry total time in 20uS (0 means no retry, 0xFFFF infinite retry), return 0 success, timeout/error retry
uint8_t USBHostTransact(uint8_t endp_pid, uint8_t tog, uint16_t timeout); // endp_pid: the upper 4 bits are the token_pid token, and the lower 4 bits are the endpoint address
uint8_t HostCtrlTransfer(__xdata uint8_t *DataBuf, uint8_t *RetLen);      // execute control transmission, the 8-byte request code is in pSetupReq, and DataBuf is an optional sending and receiving buffer
// If you need to receive and send data, then DataBuf needs to point to a valid buffer for storing subsequent data, and the total length of the actual successful sending and receiving is returned and stored in the byte variable pointed to by ReqLen
void    CopySetupReqPkg(__code uint8_t *pReqPkt); // copy control transfer request packet
uint8_t CtrlGetDeviceDescr(void);         // get the device descriptor and return it in TxBuffer
uint8_t CtrlGetConfigDescr(void);         // get the configuration descriptor and return it in TxBuffer
uint8_t CtrlSetUsbAddress(uint8_t addr);  // set USB device address
uint8_t CtrlSetUsbConfig(uint8_t cfg);    // set USB device configuration
uint8_t CtrlClearEndpStall(uint8_t endp); // clear endpoint STALL

#ifndef DISK_BASE_BUF_LEN
uint8_t CtrlSetUsbIntercace(uint8_t cfg);       // set the USB device interface
uint8_t CtrlGetHIDDeviceReport(uint8_t infc);   // HID class commands, SET_IDLE and GET_REPORT
uint8_t CtrlGetHubDescr(void);                  // get the HUB descriptor and return it in TxBuffer
uint8_t HubGetPortStatus(uint8_t HubPortIndex); // query the HUB port status and return it in TxBuffer
uint8_t HubSetPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt);   // set HUB port characteristics
uint8_t HubClearPortFeature(uint8_t HubPortIndex, uint8_t FeatureSelt); // clear HUB port characteristics
uint8_t CtrlGetXPrinterReport1(void);           // printer class commands
uint8_t AnalyzeHidIntEndp(__xdata uint8_t *buf, uint8_t HubPortIndex);  // analyze the address of the HID interrupt endpoint from the descriptor
uint8_t AnalyzeBulkEndp(__xdata uint8_t *buf, uint8_t HubPortIndex);    // parse out bulk endpoints
uint8_t TouchStartAOA(void);                    // try AOA boot
uint8_t EnumAllRootDevice(void);                // enumerate USB devices of all ROOT-HUB ports
uint8_t InitDevOnHub(uint8_t HubPortIndex);     // initialize the secondary USB device after enumerating the external HUB
uint8_t EnumHubPort(void);                      // enumerates each port of the external HUB hub on the specified ROOT-HUB port, checks whether each port has a connection or removal event and initializes the secondary USB device
uint8_t EnumAllHubPort(void);                   // enumerate all secondary USB devices behind the external HUB under the ROOT-HUB port
uint16_t SearchTypeDevice(uint8_t type);        // Search for the port number of the specified type of device on each port of ROOT-HUB and external HUB, if the output port number is 0xFFFF, it cannot be found.
                                                // The high 8 bits of the output are the ROOT-HUB port number, the low 8 bits are the port number of the external HUB, and the low 8 bits are 0, the device is directly on the ROOT-HUB port.
uint8_t SETorOFFNumLock(uint8_t *buf);
#endif

uint8_t InitRootDevice(void);                   // initialize the USB device of the specified ROOT-HUB port
void    InitUSB_Host(void);                     // initialize the USB host
