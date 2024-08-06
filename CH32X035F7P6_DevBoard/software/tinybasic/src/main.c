// ===================================================================================
// Project:   Example for CH32X035/X034/X033
// Version:   v1.0
// Year:      2023
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// A simple BASIC interpreter based on TOYOSHIKI Tiny BASIC using USB CDC as terminal.
//
// References:
// -----------
// - Tetsuya Suzuki, TOYOSHIKI Tiny BASIC: https://github.com/rutles/ttbasic_arduino
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
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
// - Connect the device via USB to your PC.
// - Open a serial terminal and select the proper COM port.
// - Write and execute your BASIC program.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <ttbasic.h>                // TOYOSHIKI Tiny BASIC
#include <usb_cdc.h>                // USB CDC serial functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  CDC_init();                       // init USB CDC
  while(1) basic();                 // run BASIC
}
