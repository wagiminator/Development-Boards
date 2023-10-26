// ===================================================================================
// Project:   PWM Demo for CH32V003
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
// Fade built-in LED using PWM.
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
#include <system.h>         // system functions
#include <gpio.h>           // GPIO functions

#define PIN_LED   PC0       // define LED pin

// ===================================================================================
// PWM Functions
// ===================================================================================

// Start PWM for pin PC0 using TIM2, CH3
void PWM_PC0_start(void) {
  RCC->APB1PCENR |= RCC_TIM2EN;     // enable timer module
  RCC->APB2PCENR |= RCC_IOPCEN      // enable I/O Port C
                 |  RCC_AFIOEN;     // enable auxiliary I/O functions
  GPIOC->CFGLR   |= 0b1001;         // set pin PC0 to output, push-pull, 100Mhz, auxiliary
  TIM2->CCER     |= TIM_CC3E;       // enable channel 3 output
  TIM2->CHCTLR2  |= TIM_OC3M;       // set PWM mode 2
  TIM2->CTLR1    |= TIM_CEN;        // enable timer
}

// Set PWM duty cycle for pin PC0
void PWM_PC0_setDuty(uint16_t duty) {
  TIM2->CH3CVR = duty;
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup
  PWM_PC0_start();                   // start PWM on pin PC0

  // Loop
  while(1) {
    uint16_t i;
    while(++i) {                    // increase PWM duty cycle value
      PWM_PC0_setDuty(i);           // write PWM duty cycle value
      DLY_us(20);                   // delay a bit
    }
    while(--i) {                    // decrease PWM duty cycle value
      PWM_PC0_setDuty(i);           // write PWM duty cycle value
      DLY_us(20);                   // delay a bit
    }
  }
}
