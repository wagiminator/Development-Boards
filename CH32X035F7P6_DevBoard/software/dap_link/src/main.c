// ===================================================================================
// Project:   CMSIS-DAP v2 Debug Probe (DAPLink) for CH32X035
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
//            CMSIS-DAP sources (src/cmsis_dap): Apache-2.0, ARM Limited
// ===================================================================================
//
// Description:
// ------------
// CMSIS-DAP v2.1 compatible debug probe for ARM Cortex-M microcontrollers based on
// the official CMSIS-DAP firmware sources of ARM. Features:
// - SWD and JTAG (incl. SWD sequences, JTAG scan chains up to 8 devices)
// - SWO trace in UART/NRZ mode (up to 3 Mbaud), read via DAP_SWO_Data command or
//   streamed via the SWO bulk endpoint
// - UART as USB virtual COM port (CDC-ACM) or via DAP_UART commands
// - Target reset (nRESET) and JTAG test reset (nTRST)
// - Status LED(s), timestamps, WinUSB descriptors (driverless on Windows 8.1+)
// The probe works with tinyOCD (https://github.com/wagiminator/MCU-Flash-Tools),
// pyOCD, OpenOCD, probe-rs, Keil MDK and other CMSIS-DAP host tools.
//
// Pin assignments, interface options and USB descriptors are defined in config.h.
//
// References:
// -----------
// - ARM CMSIS-DAP: https://github.com/ARM-software/CMSIS-DAP
// - CMSIS-DAP documentation: https://arm-software.github.io/CMSIS-DAP
// - tinyOCD: https://github.com/wagiminator/MCU-Flash-Tools
//
// Notes:
// ------
// - The DAP packet size is 510 bytes (multiple USB packets). Responses with a length
//   that is a multiple of 64 bytes are terminated by a zero length packet, so host
//   tools that read the packet size and tools that read more bytes both work.
// - Using PC19 for SWO (default) disables the SDI debug interface of the CH32X035
//   itself. The firmware can still be updated via the USB bootloader.
// - Target supply: 3.3V only.
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with
//   chprog (via pip) are installed. In addition, Linux requires access rights to
//   the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the probe to the target board (see config.h):
//   PA5 (SWCLK/TCK), PA7 (SWDIO/TMS), PA0 (nRESET), GND, 3V3 (if the target is
//   powered by the probe); for JTAG also PA4 (TDI), PA6 (TDO), PA1 (nTRST);
//   PC19 (SWO), PA2 (UART TX -> target RX), PA3 (UART RX <- target TX).
// - Linux: allow access to the probe:
//   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1A86", ATTR{idProduct}=="0001", MODE="666"' | sudo tee /etc/udev/rules.d/99-cmsis-dap.rules
//   sudo udevadm control --reload-rules
// - Use tinyOCD: python3 tinyocd.py -f firmware.bin
// - LED: slow blinking = USB not configured, on = debugger connected,
//        short flashes = DAP commands are executed.

// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "config.h"                 // user configurations
#include "system.h"                 // system functions
#include "DAP_config.h"             // CMSIS-DAP hardware configuration
#include "usb_dap.h"                // CMSIS-DAP v2 USB transport
#include "usb_cdc.h"                // USB virtual COM port
#include "dap_swo.h"                // SWO trace
#include "dap_uart.h"               // UART

#define LED_BLINK_TIME    (F_CPU / 4)       // LED blink interval (USB not ready)
#define LED_FLASH_TIME    (F_CPU / 50)      // LED flash duration (activity)

// ===================================================================================
// Status LED
// ===================================================================================
#ifdef PIN_LED_CONNECTED
static void LED_update(void) {
  static uint32_t timer = 0;
  static uint8_t  flash = 0;
  static uint8_t  state = 0;
  uint8_t led;

  if(!DAP_usbReady()) {                     // USB not configured: slow blinking
    if((STK->CNTL - timer) > LED_BLINK_TIME) {
      timer = STK->CNTL;
      state = !state;
      DAP_pinWrite(PIN_LED_CONNECTED, state ? LED_ACTIVE_LEVEL : !LED_ACTIVE_LEVEL);
    }
    return;
  }

  if(DAP_activity) {                        // DAP command executed: start flash
    DAP_activity = 0;
    if(!flash) {
      flash = 1;
      timer = STK->CNTL;
    }
  }
  if(flash && ((STK->CNTL - timer) > LED_FLASH_TIME)) flash = 0;

  led = DAP_LED_connected ^ flash;          // flash inverts the LED
  if(led != state) {
    state = led;
    DAP_pinWrite(PIN_LED_CONNECTED, state ? LED_ACTIVE_LEVEL : !LED_ACTIVE_LEVEL);
  }
}
#else
#define LED_update()
#endif

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DAP_init();                       // init DAP pins and USB
  UART_init();                      // init UART and USB COM port
  SWO_init();                       // init SWO trace input

  // Loop
  while(1) {
    DAP_task();                     // execute DAP commands
    SWO_task();                     // stream SWO trace data
    CDC_task();                     // virtual COM port data transfer
    LED_update();                   // update status LED
  }
}
