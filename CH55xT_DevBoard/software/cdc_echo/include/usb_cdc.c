// ===================================================================================
// USB CDC Functions for CH55x
// ===================================================================================

#include "ch554.h"
#include "usb.h"
#include "usb_descr.h"
#include "delay.h"

extern __xdata uint8_t Ep0Buffer[];
extern __xdata uint8_t Ep2Buffer[];

// Initialize line coding
__xdata struct CDC_LINE_CODING_TYPE {
  uint32_t baudrate;        // baud rate
  uint8_t  stopbits;        // number of stopbits (0:1bit,1:1.5bits,2:2bits)
  uint8_t  parity;          // parity (0:none,1:odd,2:even,3:mark,4:space)
  uint8_t  databits;        // number of data bits (5,6,7,8 or 16)
} CDC_LineCoding = {
  .baudrate = 57600,        // baudrate 57600
  .stopbits = 0,            // 1 stopbit
  .parity   = 0,            // no parity
  .databits = 8             // 8 databits
};

volatile __xdata uint8_t USBByteCountEP2 = 0;   // bytes of received data on USB endpoint
volatile __xdata uint8_t USBBufOutPointEP2 = 0; // data pointer for fetching
volatile __xdata uint8_t controlLineState = 0;  // control line state
volatile __bit UpPoint2BusyFlag  = 0;           // flag of whether upload pointer is busy
__xdata uint8_t usbWritePointer = 0;

// Flush the OUT-buffer
void CDC_flush(void) {
  if(!UpPoint2BusyFlag && usbWritePointer > 0) {
    UEP2_T_LEN = usbWritePointer;                                                   
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK; // respond ACK
    UpPoint2BusyFlag = 1;
    usbWritePointer = 0;
  }
}

// Write a single character via CDC
void CDC_write(char c) {
  uint16_t waitWriteCount;
  if(controlLineState > 0) {
    while(1) {
      waitWriteCount = 50000;
      while(UpPoint2BusyFlag) {                 // wait for 250ms or give up
        _delay_us(5);
        if(!--waitWriteCount) return;
      }
      if(usbWritePointer < EP2_SIZE) {
        Ep2Buffer[EP2_SIZE + usbWritePointer++] = c;
        return;
      }
      else CDC_flush();                         // go back to first while
    }
  }
}

// Send a string via CDC
void CDC_print(char* str) {
  uint16_t waitWriteCount;
  if(controlLineState > 0) {
    while(*str) {
      waitWriteCount = 50000;
      while(UpPoint2BusyFlag) {
        waitWriteCount++;
        _delay_us(5);   
        if(!--waitWriteCount) return;
      }
      while(*str) {
        if(usbWritePointer < EP2_SIZE)
          Ep2Buffer[EP2_SIZE + usbWritePointer++] = *str++;
        else {
          CDC_flush();
          break;
        }
      }
    }
  }
}

// Send a string with newline via CDC
void CDC_println(char* str) {
  CDC_print(str);
  CDC_print("\n");
}

// Check for incoming data
uint8_t CDC_available(void) {
  return USBByteCountEP2;
}

// Read a character from the IN-buffer
char CDC_read(void) {
  char data;
  if(USBByteCountEP2 == 0) return 0;
  data = Ep2Buffer[USBBufOutPointEP2];
  USBBufOutPointEP2++;
  USBByteCountEP2--;
  if(USBByteCountEP2 == 0)
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_R_RES | UEP_R_RES_ACK;
  return data;
}

// Get DTR flag
uint8_t CDC_getDTR(void) {
  return(controlLineState & 1);
}

// Get RTS flag
uint8_t CDC_getRTS(void) {
  return(controlLineState & 2);
}

// Reset CDC parameters
void CDC_reset(void) {
  USBByteCountEP2 = 0;                          // bytes of received data on USB endpoint
  UpPoint2BusyFlag = 0;
}

// Set line coding handler
void CDC_setLineCoding(void) {
  uint8_t i;
  for(i=0; i<((sizeof(CDC_LineCoding)<=USB_RX_LEN)?sizeof(CDC_LineCoding):USB_RX_LEN); i++) {
    ((uint8_t*)&CDC_LineCoding)[i] = Ep0Buffer[i];
  }
}

// Get line coding handler
uint16_t CDC_getLineCoding(void) {
  uint8_t i;
  for(i=0; i<sizeof(CDC_LineCoding); i++) {
    Ep0Buffer[i] = ((uint8_t*)&CDC_LineCoding)[i];
  }
  return sizeof(CDC_LineCoding);
}

// Set control line state handler
void CDC_setControlLineState(void) {
  controlLineState = Ep0Buffer[2];
}

void USB_EP2_IN(void) {
  UEP2_T_LEN = 0;                                           // no data to send anymore
  UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK; // respond NAK by default
  UpPoint2BusyFlag = 0;                                     // clear busy flag
}

void USB_EP2_OUT(void) {
  if(U_TOG_OK) {                                            // discard unsynchronized packets
    USBByteCountEP2 = USB_RX_LEN;
    USBBufOutPointEP2 = 0;                                  // reset Data pointer for fetching
    if(USBByteCountEP2) 
      UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_R_RES | UEP_R_RES_NAK; // respond NAK after a packet. Let main code change response after handling.
  }
}
