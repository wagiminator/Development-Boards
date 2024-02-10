// ===================================================================================
// USB HID Standard Mouse Functions for CH551, CH552 and CH554                * v1.1 *
// ===================================================================================

#include "usb_mouse.h"

// ===================================================================================
// Mouse HID Report
// ===================================================================================

#define MOUSE_report        ((PHID_MOUSE_REPORT_TYPE)HID_report)
#define MOUSE_sendReport()  HID_sendReport((uint8_t*)&HID_report, sizeof(HID_report))

// HID report typedef
typedef struct _HID_MOUSE_REPORT_TYPE {
  uint8_t buttons;                    // button states
  int8_t  xmove;                      // relative movement on the x-axis
  int8_t  ymove;                      // relative movement on the y-axis
  int8_t  wmove;                      // relative movement of the wheel
} HID_MOUSE_REPORT_TYPE, *PHID_MOUSE_REPORT_TYPE;

// Initialize HID report
__xdata HID_MOUSE_REPORT_TYPE HID_report = {
  .buttons = 0,
  .xmove   = 0,
  .ymove   = 0,
  .wmove   = 0
};

// ===================================================================================
// Mouse Functions
// ===================================================================================

// Press mouse button(s)
void MOUSE_press(uint8_t buttons) {
  MOUSE_report->buttons |= buttons;   // press button(s)
  MOUSE_sendReport();                 // send HID report
}

// Release mouse button(s)
void MOUSE_release(uint8_t buttons) {
  MOUSE_report->buttons &= ~buttons;  // release button(s)
  MOUSE_sendReport();                 // send HID report
}

// Move mouse pointer
void MOUSE_move(int8_t xrel, int8_t yrel) {
  MOUSE_report->xmove = xrel;         // set relative x-movement
  MOUSE_report->ymove = yrel;         // set relative y-movement
  MOUSE_sendReport();                 // send HID report
  MOUSE_report->xmove = 0;            // reset movements
  MOUSE_report->ymove = 0;
}

// Move mouse wheel
void MOUSE_wheel(int8_t rel) {
  MOUSE_report->wmove = rel;          // set relative wheel movement
  MOUSE_sendReport();                 // send HID report
  MOUSE_report->wmove = 0;            // reset movements
}
