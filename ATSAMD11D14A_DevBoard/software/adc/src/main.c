// ===================================================================================
// Project:   Example for ATSAMD11D14A
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// Reads the supply voltage, the internal temperature sensor and the analog value
// on pin PA2 once a second and sends the results via USB CDC (virtual
// serial port).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-arm-none-eabi) and Python3 with tinyocd (via pip)
//   are installed. If necessary, grant access permissions for the SWD programmer.
// - Connect the SWD programmer (e.g. CMSIS-DAP) to the SWD pins of the board.
// - Run 'make flash'.
// - Alternatively, if the SAM-BA bootloader is installed, set LDSCRIPT and ISPTOOL
//   in the makefile accordingly (see readme) and upload the firmware via USB.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a CDC device.
// - Open a serial monitor and select the correct serial port (BAUD rate doesn't
//   matter).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions (ADC)
#include "usb_cdc.h"                // USB CDC serial functions

#define PIN_ADC   PA2               // pin for ADC measurement

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  CDC_init();                       // init USB CDC
  ADC_init();                       // init and calibrate ADC

  // Loop
  while(1) {
    ADC_input(PIN_ADC);                             // set ADC input pin
    CDC_printf("PIN:  %u\n", ADC_read());           // read and send pin value
    CDC_printf("VDD:  %u mV\n", ADC_read_VDD());    // read and send supply voltage
    CDC_printf("TEMP: %d C\n\n", ADC_read_TEMP());  // read and send temperature
    CDC_flush();                                    // flush transmit buffer
    DLY_ms(1000);                                   // wait a second
  }
}
