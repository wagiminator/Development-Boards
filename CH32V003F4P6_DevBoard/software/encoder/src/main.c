// ===================================================================================
// Project:   Rotary Encoder with Timer Demo for CH32V003
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
// Reads rotary encoder utilizing timer and sends value via UART.
// Pins: PC6 -> ENC_A, PC7 -> ENC_B, PD5 -> RXD USB2Serial.
//
// References:
// -----------
// - CNLohr ch32v003fun: https://github.com/cnlohr/ch32v003fun
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with PyUSB
//   is installed. In addition, Linux requires access rights to WCH-LinkE programmer.
// - Connect the WCH-LinkE programmer to the MCU board.
// - Run 'make flash'.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include <encoder_tim.h>                  // rotary encoder with timer functions
#include <debug_serial.h>                 // serial debug functions

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  DEBUG_init();                           // init debug (pin PD5, 8N1, BAUD 115200)
  ENC1_init();                            // init rotary encoder 1
  uint16_t lastcount = ENC1_get();        // preload last encoder count value

  while(1) {
    uint16_t count = ENC1_get();          // get current encoder count value
    if(count != lastcount) {              // changed ?
      DEBUG_printf("Value:%6d\n", count); // print new value
      lastcount = count;                  // update last encoder count value
    }
    DLY_ms(50);                           // not too fast
  }
}
