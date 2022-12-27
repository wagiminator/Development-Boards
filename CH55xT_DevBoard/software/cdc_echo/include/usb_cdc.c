// ===================================================================================
// USB CDC Functions for CH55x
// ===================================================================================

#include "ch554.h"
#include "usb.h"
#include "usb_descr.h"
#include "delay.h"

#define LINE_CODING_SIZE 7

extern __xdata uint8_t Ep0Buffer[];
extern __xdata uint8_t Ep2Buffer[];

// Initialize for baudrate 57600, 1 stopbit, No parity, eight data bits
__xdata uint8_t LineCoding[LINE_CODING_SIZE]={0x00,0xe1,0x00,0x00,0x00,0x00,0x08};

volatile __xdata uint8_t USBByteCountEP2 = 0;   // bytes of received data on USB endpoint
volatile __xdata uint8_t USBBufOutPointEP2 = 0; // data pointer for fetching
volatile __xdata uint8_t controlLineState = 0;
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

// Send a string via CDC
void CDC_print(char* str) {
  uint16_t waitWriteCount;
  if(controlLineState > 0) {
    while(*str) {
      waitWriteCount = 0;
      while(UpPoint2BusyFlag) {
        waitWriteCount++;
        _delay_us(5);   
        if(waitWriteCount >= 50000) return;
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

// Reset CDC parameters
void resetCDCParameters(void) {
  USBByteCountEP2 = 0;                          // bytes of received data on USB endpoint
  UpPoint2BusyFlag = 0;
}

// Set line coding handler
void setLineCodingHandler(void) {
  uint8_t i;
  for(i=0; i<((LINE_CODING_SIZE<=USB_RX_LEN)?LINE_CODING_SIZE:USB_RX_LEN); i++) {
    LineCoding[i] = Ep0Buffer[i];
  }
}

// Get line coding handler
uint16_t getLineCodingHandler(void) {
  uint8_t i;
  for(i=0; i<LINE_CODING_SIZE; i++) {
    Ep0Buffer[i] = LineCoding[i];
  }
  return LINE_CODING_SIZE;
}

// Set control line state handler
void setControlLineStateHandler(void) {
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
