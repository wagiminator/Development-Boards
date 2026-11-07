// ===================================================================================
// USB HID Host Functions for CH554                                           * v1.0 *
// ===================================================================================

#include <stdio.h>
#include "delay.h"
#include "usb_hid_host.h"

#define HOST_BUF_SIZE   64
#define HOST_NONE       0xFF

__xdata uint8_t HOST_buffer[HOST_BUF_SIZE];   // last received report
__xdata HOST_MOUSE_REPORT HOST_mouse;                 // last mouse report
uint8_t HOST_kbdLEDs = 0;                     // state of keyboard LEDs
uint8_t HOST_kbdModifiers = 0;                // state of modifier keys

static uint8_t HOST_kbdLoc   = HOST_NONE;     // location of keyboard (0: root, n: hub port)
static uint8_t HOST_mouseLoc = HOST_NONE;     // location of mouse
static uint8_t HOST_hidLoc   = HOST_NONE;     // location of other HID device
static uint8_t HOST_hubSig   = 0;             // signature of hub port states
static __xdata uint8_t HOST_lastKeys[6];              // last pressed keys

// ===================================================================================
// Keyboard Usage ID to ASCII Conversion Tables (US Layout), Usage 0x1E .. 0x38
// ===================================================================================
__code char HOST_keyNormal[] = "1234567890\r\x1b\b\t -=[]\\#;'`,./";
__code char HOST_keyShift[]  = "!@#$%^&*()\r\x1b\b\t _+{}|~:\"~<>?";
__code char HOST_keyPad[]    = "/*-+\r1234567890.";

// ===================================================================================
// Internal Helper Functions
// ===================================================================================

// Get location (0: root, n: hub port n) of device type, HOST_NONE if not found
static uint8_t HOST_search(uint8_t type) {
  uint16_t loc = SearchTypeDevice(type);
  return (loc == 0xFFFF) ? HOST_NONE : (uint8_t)loc;
}

// Signature of the states of devices connected to an external hub
static uint8_t HOST_signature(void) {
  uint8_t i, sig = ThisUsbDev.DeviceStatus;
  if(ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB) {
    for(i=0; i<HUB_MAX_PORTS; i++)
      sig += (DevOnHubPort[i].DeviceStatus + 1) * (i + 3) + DevOnHubPort[i].DeviceType;
  }
  return sig;
}

// Execute class request without data stage (OUT) on device at location loc
static uint8_t HOST_classRequest(uint8_t loc, uint8_t req, uint8_t val, uint8_t len, __xdata uint8_t *buf) {
  uint8_t s, rlen;
  SelectHubPort(loc);
  pSetupReq->bRequestType = 0x21;   // class request, interface, host to device
  pSetupReq->bRequest     = req;
  pSetupReq->wValueL      = val;
  pSetupReq->wValueH      = (req == 0x09) ? 0x02 : 0x00;   // SET_REPORT: output report
  pSetupReq->wIndexL      = 0;      // interface 0
  pSetupReq->wIndexH      = 0;
  pSetupReq->wLengthL     = len;
  pSetupReq->wLengthH     = 0;
  s = HostCtrlTransfer(buf, &rlen);
  SetUsbSpeed(1);                   // default is full speed
  return s;
}

// Read interrupt IN endpoint k of device at location loc, returns length
static uint8_t HOST_readEP(uint8_t loc, uint8_t k) {
  uint8_t endp, s, i, len = 0;
  if(loc == HOST_NONE || k > 3) return 0;
  endp = loc ? DevOnHubPort[loc-1].GpVar[k] : ThisUsbDev.GpVar[k];
  if(!(endp & USB_ENDP_ADDR_MASK)) return 0;
  SelectHubPort(loc);
  s = USBHostTransact(USB_PID_IN << 4 | (endp & 0x7F), (endp & 0x80) ? bUH_R_TOG | bUH_T_TOG : 0, 0);
  if(s == ERR_SUCCESS) {
    endp ^= 0x80;                   // flip sync flag
    if(loc) DevOnHubPort[loc-1].GpVar[k] = endp;
    else    ThisUsbDev.GpVar[k] = endp;
    len = USB_RX_LEN;
    if(len > HOST_BUF_SIZE) len = HOST_BUF_SIZE;
    for(i=0; i<len; i++) HOST_buffer[i] = RxBuffer[i];
  }
  SetUsbSpeed(1);                   // default is full speed
  return len;
}

// ===================================================================================
// Init USB Host
// ===================================================================================
void HOST_init(void) {
  InitUSB_Host();
  FoundNewDev = 0;
}

// ===================================================================================
// Handle Connection/Disconnection and Enumeration of Devices
// ===================================================================================
uint8_t HOST_update(void) {
  uint8_t s, i, changed = 0;
  if(UIF_DETECT) {                  // USB host detection interrupt?
    UIF_DETECT = 0;                 // clear interrupt flag
    s = AnalyzeRootHub();           // analyze ROOT-HUB status
    if(s == ERR_USB_CONNECT) FoundNewDev = 1;
    else if(s == ERR_USB_DISCON) changed = 1;
  }
  if(FoundNewDev) {                 // new device plugged in?
    FoundNewDev = 0;
    DLY_ms(100);                    // wait to eliminate plugging jitter
    EnumAllRootDevice();            // enumerate device
    changed = 1;
  }
  EnumAllHubPort();                 // enumerate devices behind an external hub
  s = HOST_signature();
  if(s != HOST_hubSig) {
    HOST_hubSig = s;
    changed = 1;
  }
  if(changed) {
    HOST_kbdLoc = HOST_mouseLoc = HOST_hidLoc = HOST_NONE;
    for(i=0; i<6; i++) HOST_lastKeys[i] = 0;
  }
  return changed;
}

// ===================================================================================
// Search Devices
// ===================================================================================
uint8_t HOST_findKeyboard(void) {
  HOST_kbdLoc = HOST_search(DEV_TYPE_KEYBOARD);
  if(HOST_kbdLoc == HOST_NONE) return 0;
  HOST_classRequest(HOST_kbdLoc, 0x0B, 0, 0, 0);   // SET_PROTOCOL: boot protocol
  HOST_setKbdLEDs(HOST_kbdLEDs);                   // restore LED state
  return 1;
}

uint8_t HOST_findMouse(void) {
  HOST_mouseLoc = HOST_search(DEV_TYPE_MOUSE);
  if(HOST_mouseLoc == HOST_NONE) return 0;
  HOST_classRequest(HOST_mouseLoc, 0x0B, 0, 0, 0); // SET_PROTOCOL: boot protocol
  return 1;
}

uint8_t HOST_findHID(void) {
  HOST_hidLoc = HOST_search(USB_DEV_CLASS_HID);
  if(HOST_hidLoc == HOST_NONE) HOST_hidLoc = HOST_search(DEV_TYPE_KEYBOARD);
  if(HOST_hidLoc == HOST_NONE) HOST_hidLoc = HOST_search(DEV_TYPE_MOUSE);
  return (HOST_hidLoc != HOST_NONE);
}

// ===================================================================================
// Keyboard Functions
// ===================================================================================
void HOST_setKbdLEDs(uint8_t leds) {
  __xdata uint8_t buf[2];
  HOST_kbdLEDs = leds;
  if(HOST_kbdLoc == HOST_NONE) return;
  buf[0] = leds; buf[1] = 0;
  HOST_classRequest(HOST_kbdLoc, 0x09, 0, 1, buf); // SET_REPORT (output report)
}

char HOST_readKey(void) {
  uint8_t i, j, key, shift, len;
  char c = 0;
  len = HOST_readEP(HOST_kbdLoc, 0);
  if(len < 8) return 0;
  HOST_kbdModifiers = HOST_buffer[0];
  shift = (HOST_buffer[0] & 0x22) ? 1 : 0;      // left or right shift pressed?
  for(i=2; i<8; i++) {
    key = HOST_buffer[i];
    if(key < 0x04) continue;                    // no key or error
    for(j=0; j<6; j++) if(HOST_lastKeys[j] == key) break;
    if(j < 6) continue;                         // key was already pressed
    if(c) continue;                             // only one character per report
    if(key == 0x53) HOST_setKbdLEDs(HOST_kbdLEDs ^ HOST_LED_NUM);
    else if(key == 0x39) HOST_setKbdLEDs(HOST_kbdLEDs ^ HOST_LED_CAPS);
    else if(key == 0x47) HOST_setKbdLEDs(HOST_kbdLEDs ^ HOST_LED_SCROLL);
    else if(key <= 0x1D) {                      // letters
      c = 'a' + key - 0x04;
      if(shift ^ ((HOST_kbdLEDs & HOST_LED_CAPS) ? 1 : 0)) c -= 0x20;
    }
    else if(key <= 0x38) c = shift ? HOST_keyShift[key - 0x1E] : HOST_keyNormal[key - 0x1E];
    else if((key >= 0x54) && (key <= 0x63)) {   // keypad
      if((key <= 0x58) || (HOST_kbdLEDs & HOST_LED_NUM)) c = HOST_keyPad[key - 0x54];
    }
  }
  for(i=0; i<6; i++) HOST_lastKeys[i] = HOST_buffer[i+2];
  return c;
}

// ===================================================================================
// Mouse Functions
// ===================================================================================
uint8_t HOST_readMouse(void) {
  uint8_t len = HOST_readEP(HOST_mouseLoc, 0);
  if(len < 3) return 0;
  HOST_mouse.buttons = HOST_buffer[0];
  HOST_mouse.x       = (int8_t)HOST_buffer[1];
  HOST_mouse.y       = (int8_t)HOST_buffer[2];
  HOST_mouse.wheel   = (len > 3) ? (int8_t)HOST_buffer[3] : 0;
  return 1;
}

// ===================================================================================
// Generic HID Functions
// ===================================================================================
uint8_t HOST_readHID(uint8_t ep) {
  return HOST_readEP(HOST_hidLoc, ep);
}

// ===================================================================================
// Print Device Information
// ===================================================================================
static void HOST_printDevice(uint8_t loc) {
  uint8_t i, status, addr, speed, type;
  __xdata uint16_t vid, pid, len;
  if(loc) {
    status = DevOnHubPort[loc-1].DeviceStatus;  addr = DevOnHubPort[loc-1].DeviceAddress;
    speed  = DevOnHubPort[loc-1].DeviceSpeed;   type = DevOnHubPort[loc-1].DeviceType;
    vid    = DevOnHubPort[loc-1].DeviceVID;     pid  = DevOnHubPort[loc-1].DevicePID;
    printf("Hub port %d: ", (int)loc);
  }
  else {
    status = ThisUsbDev.DeviceStatus;  addr = ThisUsbDev.DeviceAddress;
    speed  = ThisUsbDev.DeviceSpeed;   type = ThisUsbDev.DeviceType;
    vid    = ThisUsbDev.DeviceVID;     pid  = ThisUsbDev.DevicePID;
    printf("Root port: ");
  }
  if(status == ROOT_DEV_DISCONNECT) { printf("no device\n"); return; }
  if(status != ROOT_DEV_SUCCESS)    { printf("enumeration failed\n"); return; }
  printf("VID=%04X PID=%04X Addr=%d %s-speed ", vid, pid, (int)addr, speed ? "full" : "low");
  switch(type) {
    case DEV_TYPE_KEYBOARD:     printf("Keyboard\n"); break;
    case DEV_TYPE_MOUSE:        printf("Mouse\n"); break;
    case USB_DEV_CLASS_HID:     printf("HID device\n"); break;
    case USB_DEV_CLASS_HUB:     printf("Hub\n"); break;
    case USB_DEV_CLASS_PRINTER: printf("Printer\n"); break;
    case USB_DEV_CLASS_STORAGE: printf("Mass storage\n"); break;
    case DEF_AOA_DEVICE:        printf("Android accessory\n"); break;
    default:                    printf("Class 0x%02X\n", (int)type); break;
  }
  SelectHubPort(loc);
  if(CtrlGetDeviceDescr() == ERR_SUCCESS) {
    printf("  Device descriptor:");
    for(i=0; i<18; i++) printf(" %02X", (int)Com_Buffer[i]);
    printf("\n");
  }
  if(CtrlGetConfigDescr() == ERR_SUCCESS) {
    len = ((PXUSB_CFG_DESCR)Com_Buffer)->wTotalLength;
    if(len > COM_BUF_SIZE) len = COM_BUF_SIZE;
    printf("  Config descriptor:");
    for(i=0; i<len; i++) {
      if(!(i & 15)) printf("\n   ");
      printf(" %02X", (int)Com_Buffer[i]);
    }
    printf("\n");
  }
  SetUsbSpeed(1);                   // default is full speed
}

void HOST_printInfo(void) {
  uint8_t i;
  printf("\n--- USB devices ---\n");
  HOST_printDevice(0);
  if((ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB) && (ThisUsbDev.DeviceStatus == ROOT_DEV_SUCCESS)) {
    for(i=1; i<=ThisUsbDev.GpHUBPortNum && i<=HUB_MAX_PORTS; i++) HOST_printDevice(i);
  }
}
