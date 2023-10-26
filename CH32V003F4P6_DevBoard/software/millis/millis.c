// ===================================================================================
// Project:   Millis Demo for CH32V003
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
// Blink built-in LED using millis counter.
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
#include <system.h>                 // system functions
#include <gpio.h>                   // GPIO functions

#define PIN_LED   PC0               // define LED pin

// ===================================================================================
// Millis Counter using SysTick
// ===================================================================================
volatile uint32_t MIL_millis = 0;   // millis counter

// Init millis counter
void MIL_init(void) {
  STK->CTLR = 0;                    // disable SysTick
  NVIC_EnableIRQ(SysTicK_IRQn);     // enable the SysTick IRQ
  STK->CMP = DLY_MS_TIME - 1;       // set interval to 1ms
  STK->CNT = 0;                     // start at zero
  STK->CTLR = STK_CTLR_STE          // enable SysTick
            | STK_CTLR_STIE         // enable SysTick compare match interrupt
            | STK_CTLR_STCLK;       // set SysTick clock to F_CPU
}

// Interrupt service routine
void SysTick_Handler(void) __attribute__((interrupt));
void SysTick_Handler(void) {
  MIL_millis++;                     // increase millis counter
  STK->CMP += DLY_MS_TIME;          // next interrupt 1ms later
  STK->SR   = 0;                    // clear interrupt flag
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  uint32_t nexttime = 0;            // next time variable
  PIN_output(PIN_LED);              // set LED pin as output
  MIL_init();                       // init millis counter
  
  // Loop
  while(1) {
    PIN_toggle(PIN_LED);            // toggle LED on/off
    while(MIL_millis < nexttime);   // wait for next time
    nexttime += 100;                // increase next timeby 100 milliseconds
  }
}
