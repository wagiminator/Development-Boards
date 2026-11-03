// ===================================================================================
// Project:   DAPLink (CMSIS-DAP v1, SWD) Debug Probe for CH32V003
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
// The CH32V003 identifies itself as a CMSIS-DAP v1 compliant debug probe (DAPLink)
// with SWD protocol support. It can be used to program and debug ARM-based micro-
// controllers (e.g. STM32, PY32, SAMD) with tinyOCD, OpenOCD, pyOCD and other tools
// that support CMSIS-DAP. As it is a Human Interface Device (HID), no driver
// installation is required. The SWD protocol implementation is based on Alex
// Taradov's free-dap and ARM's CMSIS-DAP reference implementation. Software USB is
// provided by CNLohr's rv003usb.
//
// Since the CH32V003 only supports low-speed USB, the 64-byte HID reports are
// transferred in 8-byte packets. The DAP requests are sent by the host via HID
// SET_REPORT control transfers, the responses via the interrupt IN endpoint.
//
// References:
// -----------
// - ARM CMSIS-DAP:         https://github.com/ARM-software/CMSIS-DAP
// - Alex Taradov free-dap: https://github.com/ataradov/free-dap
// - CNLohr ch32v003fun:    https://github.com/cnlohr/ch32v003fun
// - CNLohr rv003usb:       https://github.com/cnlohr/rv003usb
// - picoDAP:               https://github.com/wagiminator/CH552-picoDAP
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Wiring:
// -------
// CH32V003 DevBoard       ARM target
// PC4  -----------------> nRESET (optional, open-drain)
// PC5  -----------------> SWCLK
// PC6  <----------------> SWDIO
// 3V3  ------------------ VCC    (target must run on 3.3V or use a level shifter!)
// GND  ------------------ GND
//
// Status LEDs (active low): PC0 (on board) lights up while connected, an optional
// LED on PD0 shows the target running state (set by the host via DAP_HostStatus).
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with rvprog
//   are installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Adjust the pin definitions and USB settings in config.h if necessary.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It should be detected as a HID device
//   named "CH32V003 CMSIS-DAP".
// - Connect the target board to the SWD pins as shown above.
// - Run your debug software, e.g. 'python3 tinyocd.py -p cmsis-dap -f firmware.bin'.
// - Linux users need access rights to the device, e.g.:
//   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1209", ATTR{idProduct}=="c55d", MODE="666"' | sudo tee /etc/udev/rules.d/99-CH32V003-DAP.rules
//   echo 'KERNEL=="hidraw*", ATTRS{idVendor}=="1209", ATTRS{idProduct}=="c55d", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-CH32V003-DAP.rules
//   sudo udevadm control --reload-rules


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                   // system functions
#include "usb_dap.h"                  // CMSIS-DAP via USB HID

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DLY_ms(1);                          // wait a bit for USB
  DAP_USB_init();                     // init DAP and USB

  // Loop
  while(1) {
    DAP_USB_poll();                   // execute pending DAP requests
  }
}
