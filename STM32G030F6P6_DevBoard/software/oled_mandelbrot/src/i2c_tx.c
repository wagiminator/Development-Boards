// ===================================================================================
// Basic I2C Master Functions for STM32G0xx (write only)                      * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_tx.h"

// I2C timings
#define I2C_TICKS_HIGH        (((F_CPU *  9) / 25) / I2C_CLKRATE)
#define I2C_TICKS_LOW         (((F_CPU * 16) / 25) / I2C_CLKRATE)

// Init I2C
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP < 2
    // Setup pin PA9 (SCL) and pin PA10 (SDA)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b10<<(9<<1)) | ((uint32_t)0b10<<(10<<1)) );
    GPIOA->OTYPER  |=                   ( ((uint32_t)0b1 <<(9<<0)) | ((uint32_t)0b1 <<(10<<0)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b01<<(9<<1)) | ((uint32_t)0b01<<(10<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)0xf <<(1<<2)) | ((uint32_t)0xf <<( 2<<2)) ))
                                     |  ( ((uint32_t)0x6 <<(1<<2)) | ((uint32_t)0x6 <<( 2<<2)) );
    #if I2C_MAP == 1
      // Remap pins PA11/PA12 to PA9/PA10
      RCC->APBENR2  |= RCC_APBENR2_SYSCFGEN;
      SYSCFG->CFGR1 |= SYSCFG_CFGR1_PA11_RMP | SYSCFG_CFGR1_PA12_RMP;
    #endif
  #elif I2C_MAP == 2
    // Setup pin PB6 (SCL) and pin PB7 (SDA)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1)) ))
                                     |  ( ((uint32_t)0b10<<(6<<1)) | ((uint32_t)0b10<<(7<<1)) );
    GPIOB->OTYPER  |=                   ( ((uint32_t)0b1 <<(6<<0)) | ((uint32_t)0b1 <<(7<<0)) );
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~( ((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1)) ))
                                     |  ( ((uint32_t)0b01<<(6<<1)) | ((uint32_t)0b01<<(7<<1)) );
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~( ((uint32_t)15  <<(6<<2)) | ((uint32_t)15  <<(7<<2)) ))
                                     |  ( ((uint32_t)6   <<(6<<2)) | ((uint32_t)6   <<(7<<2)) );
  #elif I2C_MAP == 3
    // Setup pin PB8 (SCL) and pin PB9 (SDA)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(8<<1)) | ((uint32_t)0b11<<(9<<1)) ))
                                     |  ( ((uint32_t)0b10<<(8<<1)) | ((uint32_t)0b10<<(9<<1)) );
    GPIOB->OTYPER  |=                   ( ((uint32_t)0b1 <<(8<<0)) | ((uint32_t)0b1 <<(9<<0)) );
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~( ((uint32_t)0b11<<(8<<1)) | ((uint32_t)0b11<<(9<<1)) ))
                                     |  ( ((uint32_t)0b01<<(8<<1)) | ((uint32_t)0b01<<(9<<1)) );
    GPIOB->AFR[1]   = (GPIOB->AFR[1] & ~( ((uint32_t)0xf <<(0<<2)) | ((uint32_t)0xf <<(1<<2)) ))
                                     |  ( ((uint32_t)0x6 <<(0<<2)) | ((uint32_t)0x6 <<(1<<2)) );
  #else
    #warning No automatic pin mapping for I2C1
  #endif

  // Setup and enable I2C
  RCC->APBENR1 |= RCC_APBENR1_I2C1EN;   // enable I2C module clock
  I2C1->TIMINGR = (I2C_TICKS_HIGH<<8)
                | (I2C_TICKS_LOW <<0);  // set timings
  I2C1->CR1 = I2C_CR1_PE;               // enable
}

// Start I2C transmission (addr must contain R/W bit)
void I2C_start(uint8_t addr) {
  I2C1->CR2 = ((uint32_t)0xff<<16) | addr | I2C_CR2_START;
}

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  while(!(I2C1->ISR & I2C_ISR_TXE));
  I2C1->TXDR = data;
}

// Stop I2C transmission
void I2C_stop(void) {
  while(!(I2C1->ISR & I2C_ISR_TXE));
  I2C1->CR2 |= I2C_CR2_STOP;
}

// Write data buffer via I2C bus and stop
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  while(len--) I2C_write(*buf++);           // write buffer
  I2C_stop();                               // stop transmission
}
