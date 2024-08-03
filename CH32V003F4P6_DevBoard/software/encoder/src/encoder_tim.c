// ===================================================================================
// Basic Rotary Encoder Functions using Timer for CH32V003                    * v1.0 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "encoder_tim.h"

// ===================================================================================
// Rotary Encoder 1 using Timer1
// ===================================================================================

// Init rotary encoder 1 pins and setup timer1 in encoder mode
void ENC1_init(void) {
  // Setup pins
  #if ENC1_MAP == 0
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPDEN // enable I/O Port A and D
                  | RCC_AFIOEN              // enable auxiliary I/O functions
                  | RCC_TIM1EN;             // enable timer1
  GPIOA->CFGLR    = (GPIOA->CFGLR & ~((uint32_t)0xf<<4)) | ((uint32_t)0x8<<4);
  GPIOA->BSHR     = ((uint32_t)1<<1);       // set PA1 to input pullup
  GPIOD->CFGLR    = (GPIOD->CFGLR & ~((uint32_t)0xf<<8)) | ((uint32_t)0x8<<8);
  GPIOD->BSHR     = ((uint32_t)1<<2);       // set PD2 to input pullup
  AFIO->PCFR1    &= ~((uint32_t)0b11<<6);
  #elif ENC1_MAP == 1
  RCC->APB2PCENR |= RCC_IOPCEN              // enable I/O Port C
                  | RCC_AFIOEN              // enable auxiliary I/O functions
                  | RCC_TIM1EN;             // enable timer1
  GPIOC->CFGLR    = (GPIOC->CFGLR & ~(((uint32_t)0xf<<24) | ((uint32_t)0xf<<28)))
                                  |  (((uint32_t)0x8<<24) | ((uint32_t)0x8<<28));
  GPIOC->BSHR     = ((uint32_t)1<<6) | ((uint32_t)1<<7);   // set PC6/PC7 to input pullup
  AFIO->PCFR1     = (AFIO->PCFR1 & ~((uint32_t)0b11<<6)) | ((uint32_t)0b01<<6);
  #elif ENC1_MAP == 2
  RCC->APB2PCENR |= RCC_IOPCEN              // enable I/O Port C
                  | RCC_AFIOEN              // enable auxiliary I/O functions
                  | RCC_TIM1EN;             // enable timer1
  GPIOC->CFGLR    = (GPIOC->CFGLR & ~(((uint32_t)0xf<<16) | ((uint32_t)0xf<<28)))
                                  |  (((uint32_t)0x8<<16) | ((uint32_t)0x8<<28));
  GPIOC->BSHR     = ((uint32_t)1<<4) | ((uint32_t)1<<7);   // set PC4/PC7 to input pullup
  AFIO->PCFR1    |= ((uint32_t)0b11<<6);
  #else
    #warning Wrong ENC1 REMAP
  #endif

  // Setup timer
  TIM1->SMCFGR    = (uint16_t)0b011;        // set encoder mode 3
  TIM1->CTLR1     = TIM_CEN;                // enable/start timer2
}

// Set rotary encoder 1 current and maximum count value
void ENC1_set(uint16_t cur, uint16_t max) {
  TIM1->CNT    = cur;                       // set current count value (timer2 counter)
  TIM1->ATRLR  = max;                       // set max count value (timer2 auto-reload)
  TIM1->SWEVGR = TIM_UG;                    // re-initialize timer
}

// Read rotary encoder 1 current count value
uint16_t ENC1_get(void) {
  return TIM1->CNT;                         // read current counter value
}

// ===================================================================================
// Rotary Encoder 2 using Timer2
// ===================================================================================

// Init rotary encoder 2 pins and setup timer2 in encoder mode
void ENC2_init(void) {
  // Setup pins
  #if ENC2_MAP == 0
  RCC->APB2PCENR |= RCC_IOPDEN              // enable I/O Port D
                  | RCC_AFIOEN;             // enable auxiliary I/O functions
  GPIOD->CFGLR    = (GPIOD->CFGLR & ~((uint32_t)0xff<<12)) | ((uint32_t)0x88<<12);
  GPIOD->BSHR     = ((uint32_t)0x3<<3);     // set PD3/PD4 to input pullup
  AFIO->PCFR1    &= ~((uint32_t)0b11<<8);
  #elif ENC2_MAP == 1
  RCC->APB2PCENR |= RCC_IOPCEN              // enable I/O Port C
                  | RCC_AFIOEN;             // enable auxiliary I/O functions
  GPIOC->CFGLR    = (GPIOC->CFGLR & ~(((uint32_t)0xf<<8) | ((uint32_t)0xf<<20)))
                                  |  (((uint32_t)0x8<<8) | ((uint32_t)0x8<<20));
  GPIOC->BSHR     = ((uint32_t)1<<2) | ((uint32_t)1<<5);   // set PC5/PC2 to input pullup
  AFIO->PCFR1     = (AFIO->PCFR1 & ~((uint32_t)0b11<<8)) | ((uint32_t)0b01<<8);
  #elif ENC2_MAP == 2
  RCC->APB2PCENR |= RCC_IOPCEN | RCC_IOPDEN // enable I/O Port C and D
                  | RCC_AFIOEN;             // enable auxiliary I/O functions
  GPIOC->CFGLR    = (GPIOC->CFGLR & ~((uint32_t)0xf<<4)) | ((uint32_t)0x8<<4);
  GPIOC->BSHR     = ((uint32_t)1<<1);       // set PC1 to input pullup
  GPIOD->CFGLR    = (GPIOD->CFGLR & ~((uint32_t)0xf<<12)) | ((uint32_t)0x8<<12);
  GPIOD->BSHR     = ((uint32_t)1<<3);       // set PD3 to input pullup
  AFIO->PCFR1     = (AFIO->PCFR1 & ~((uint32_t)0b11<<8)) | ((uint32_t)0b10<<8);
  #elif ENC2_MAP == 3
  RCC->APB2PCENR |= RCC_IOPCEN              // enable I/O Port C
                  | RCC_AFIOEN;             // enable auxiliary I/O functions
  GPIOC->CFGLR    = (GPIOC->CFGLR & ~(((uint32_t)0xf<<4) | ((uint32_t)0xf<<28)))
                                  |  (((uint32_t)0x8<<4) | ((uint32_t)0x8<<28));
  GPIOC->BSHR     = ((uint32_t)1<<1) | ((uint32_t)1<<7);   // set PC1/PC7 to input pullup
  AFIO->PCFR1    |= ((uint32_t)0b11<<8);
  #else
    #warning Wrong ENC2 REMAP
  #endif

  // Setup timer
  RCC->APB1PCENR |= RCC_TIM2EN;             // enable timer2 module
  TIM2->SMCFGR    = (uint16_t)0b011;        // set encoder mode 3
  TIM2->CTLR1     = TIM_CEN;                // enable/start timer2
}

// Set rotary encoder 2 current and maximum count value
void ENC2_set(uint16_t cur, uint16_t max) {
  TIM2->CNT    = cur;                       // set current count value (timer2 counter)
  TIM2->ATRLR  = max;                       // set max count value (timer2 auto-reload)
  TIM2->SWEVGR = TIM_UG;                    // re-initialize timer
}

// Read rotary encoder 2 current count value
uint16_t ENC2_get(void) {
  return TIM2->CNT;                         // read current counter value
}
