// ===================================================================================
// Project:   USBasp AVR ISP/TPI Programmer for CH32V003
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
// The CH32V003 identifies itself as a USBasp, an ISP/TPI programmer for AVR micro-
// controllers. It works with AVRDUDE (avrdude -c usbasp) and the Arduino IDE (Tools
// -> Programmer -> USBasp). The firmware is based on Thomas Fischl's original USBasp
// implementation and Ralph Doncaster's improvements, including the automatic SCK
// selection (auto-clock): the programmer starts with 1.5MHz and reduces the SCK
// frequency step by step until the target answers, so no slow-clock jumper is
// needed. The hardware SPI of the CH32V003 is used for SCK frequencies from 187.5kHz
// to 3MHz, software SPI for the lower frequencies down to 500Hz. Software USB is
// provided by CNLohr's rv003usb.
// The implementation includes Windows Compatible ID (WCID) descriptors, so Windows
// loads the WinUSB driver automatically without the need to install a driver.
//
// References:
// -----------
// - Thomas Fischl USBasp:  https://www.fischl.de/usbasp/
// - Ralph Doncaster:       https://github.com/nerdralph/usbasp
// - CNLohr ch32v003fun:    https://github.com/cnlohr/ch32v003fun
// - CNLohr rv003usb:       https://github.com/cnlohr/rv003usb
// - picoASP:               https://github.com/wagiminator/AVR-Programmer
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Wiring:
// -------
// CH32V003 DevBoard        AVR target (ICSP header)
// PC4  -----------------> RESET
// PC5  -----------------> SCK   (TPI: TPICLK)
// PC6  -----------------> MOSI  (TPI: TPIDATA)
// PC7  <----------------- MISO
// 3V3  ------------------ VCC   (target must run on 3.3V or use a level shifter!)
// GND  ------------------ GND
//
// Status LEDs (active low): PC0 (on board) lights up while programming, an optional
// LED on PD0 lights up as soon as the device is enumerated by the host.
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
// - Connect the board via USB to your PC. It should be detected as USBasp.
// - Connect the target board to the ISP pins as shown above.
// - Run AVRDUDE, e.g. 'avrdude -c usbasp -p m328p -U flash:w:firmware.hex'.
// - Linux users need access rights to the USBasp, e.g.:
//   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="16c0", ATTR{idProduct}=="05dc", MODE="666"' | sudo tee /etc/udev/rules.d/99-USBasp.rules
//   sudo udevadm control --reload-rules
// - Windows should load the WinUSB driver automatically (WCID). If not, install it
//   with Zadig (https://zadig.akeo.ie/).


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "system.h"                   // system functions
#include "usb_asp.h"                  // USBasp functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DLY_ms(1);                          // wait a bit for USB
  ASP_init();                         // init USBasp

  // Loop
  while(1) {
    ASP_poll();                       // execute pending USBasp requests
    ASP_LED_USB_write(ASP_USB_connected()); // USB status LED
  }
}
