// ===================================================================================
// Basic I2C Master Functions with DMA for PY32F0xx                           * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c_dma.h"

// Read/write flag
uint8_t I2C_rwflag;

// Init I2C
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP == 0
    // Setup pin PA2 (SDA) and pin PA3 (SCL), alternate function with open-drain an pullup
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(2<<1)) | ((uint32_t)0b11<<(3<<1)) ))
                                     |  ( ((uint32_t)0b10<<(2<<1)) | ((uint32_t)0b10<<(3<<1)) );
    GPIOA->OTYPER  |=                   ( ((uint32_t)0b1 <<(2<<0)) | ((uint32_t)0b1 <<(3<<0)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(2<<1)) | ((uint32_t)0b11<<(3<<1)) ))
                                     |  ( ((uint32_t)0b01<<(2<<1)) | ((uint32_t)0b01<<(3<<1)) );
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)15  <<(2<<2)) | ((uint32_t)15  <<(3<<2)) ))
                                     |  ( ((uint32_t)12  <<(2<<2)) | ((uint32_t)12  <<(3<<2)) );
  #elif I2C_MAP == 1
    // Setup pin PA7 (SDA) and pin PA8 (SCL), alternate function with open-drain an pullup
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(7<<1)) | ((uint32_t)0b11<<(8<<1)) ))
                                     |  ( ((uint32_t)0b10<<(7<<1)) | ((uint32_t)0b10<<(8<<1)) );
    GPIOA->OTYPER  |=                   ( ((uint32_t)0b1 <<(7<<0)) | ((uint32_t)0b1 <<(8<<0)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(7<<1)) | ((uint32_t)0b11<<(8<<1)) ))
                                     |  ( ((uint32_t)0b01<<(7<<1)) | ((uint32_t)0b01<<(8<<1)) );
    GPIOA->AFR[0]   = (GPIOA->AFR[0] & ~( ((uint32_t)15  <<(7<<2))                            ))
                                     |  ( ((uint32_t)12  <<(7<<2))                            );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~(                            ((uint32_t)15  <<(0<<2)) ))
                                     |  (                            ((uint32_t)12  <<(0<<2)) );
  #elif I2C_MAP == 2
    // Setup pin PA9 (SDA) and pin PA10 (SCL), alternate function with open-drain an pullup
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b10<<(9<<1)) | ((uint32_t)0b10<<(10<<1)) );
    GPIOA->OTYPER  |=                   ( ((uint32_t)0b1 <<(9<<0)) | ((uint32_t)0b1 <<(10<<0)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1)) ))
                                     |  ( ((uint32_t)0b01<<(9<<1)) | ((uint32_t)0b01<<(10<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)15  <<(1<<2)) | ((uint32_t)15  <<( 2<<2)) ))
                                     |  ( ((uint32_t)12  <<(1<<2)) | ((uint32_t)12  <<( 2<<2)) );
  #elif I2C_MAP == 3
    // Setup pin PA12 (SDA) and pin PA11 (SCL), alternate function with open-drain an pullup
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~( ((uint32_t)0b11<<(12<<1)) | ((uint32_t)0b11<<(11<<1)) ))
                                     |  ( ((uint32_t)0b10<<(12<<1)) | ((uint32_t)0b10<<(11<<1)) );
    GPIOA->OTYPER  |=                   ( ((uint32_t)0b1 <<(12<<0)) | ((uint32_t)0b1 <<(11<<0)) );
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~( ((uint32_t)0b11<<(12<<1)) | ((uint32_t)0b11<<(11<<1)) ))
                                     |  ( ((uint32_t)0b01<<(12<<1)) | ((uint32_t)0b01<<(11<<1)) );
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~( ((uint32_t)15  <<( 4<<2)) | ((uint32_t)15  <<( 3<<2)) ))
                                     |  ( ((uint32_t)6   <<( 4<<2)) | ((uint32_t)6   <<( 3<<2)) );
  #elif I2C_MAP == 4
    // Setup pin PB7 (SDA) and pin PB6 (SCL), alternate function with open-drain an pullup
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(7<<1)) | ((uint32_t)0b11<<(6<<1)) ))
                                     |  ( ((uint32_t)0b10<<(7<<1)) | ((uint32_t)0b10<<(6<<1)) );
    GPIOB->OTYPER  |=                   ( ((uint32_t)0b1 <<(7<<0)) | ((uint32_t)0b1 <<(6<<0)) );
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~( ((uint32_t)0b11<<(7<<1)) | ((uint32_t)0b11<<(6<<1)) ))
                                     |  ( ((uint32_t)0b01<<(7<<1)) | ((uint32_t)0b01<<(6<<1)) );
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~( ((uint32_t)15  <<(7<<2)) | ((uint32_t)15  <<(6<<2)) ))
                                     |  ( ((uint32_t)6   <<(7<<2)) | ((uint32_t)6   <<(6<<2)) );
  #elif I2C_MAP == 5
    // Setup pin PB7 (SDA) and pin PB8 (SCL), alternate function with open-drain an pullup
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~( ((uint32_t)0b11<<(7<<1)) | ((uint32_t)0b11<<(8<<1)) ))
                                     |  ( ((uint32_t)0b10<<(7<<1)) | ((uint32_t)0b10<<(8<<1)) );
    GPIOB->OTYPER  |=                   ( ((uint32_t)0b1 <<(7<<0)) | ((uint32_t)0b1 <<(8<<0)) );
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~( ((uint32_t)0b11<<(7<<1)) | ((uint32_t)0b11<<(8<<1)) ))
                                     |  ( ((uint32_t)0b01<<(7<<1)) | ((uint32_t)0b01<<(8<<1)) );
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~( ((uint32_t)15  <<(7<<2))                            ))
                                     |  ( ((uint32_t)6   <<(7<<2))                            );
    GPIOB->AFR[1]   = (GPIOB->AFR[1] & ~(                            ((uint32_t)15  <<(0<<2)) ))
                                     |  (                            ((uint32_t)6   <<(0<<2)) );
  #elif I2C_MAP == 6
    // Setup pin PF0 (SDA) and pin PF1 (SCL), alternate function with open-drain an pullup
    RCC->IOPENR    |= RCC_IOPENR_GPIOFEN;
    GPIOF->MODER    = (GPIOF->MODER  & ~( ((uint32_t)0b11<<(0<<1)) | ((uint32_t)0b11<<(1<<1)) ))
                                     |  ( ((uint32_t)0b10<<(0<<1)) | ((uint32_t)0b10<<(1<<1)) );
    GPIOF->OTYPER  |=                   ( ((uint32_t)0b1 <<(0<<0)) | ((uint32_t)0b1 <<(1<<0)) );
    GPIOF->PUPDR    = (GPIOF->PUPDR  & ~( ((uint32_t)0b11<<(0<<1)) | ((uint32_t)0b11<<(1<<1)) ))
                                     |  ( ((uint32_t)0b01<<(0<<1)) | ((uint32_t)0b01<<(1<<1)) );
    GPIOF->AFR[0]   = (GPIOF->AFR[0] & ~( ((uint32_t)15  <<(0<<2)) | ((uint32_t)15  <<(1<<2)) ))
                                     |  ( ((uint32_t)12  <<(0<<2)) | ((uint32_t)12  <<(1<<2)) );
  #else
    #warning No automatic pin mapping for I2C
  #endif

  // Setup and enable I2C
  RCC->APBENR1 |= RCC_APBENR1_I2CEN;              // enable I2C module clock
  I2C1->CR1     = I2C_CR1_SWRST;                  // reset I2C module
  I2C1->CR1     = 0;
  I2C1->CR2     = 4;                              // set input clock rate
  #if I2C_CLKRATE > 100000
  I2C1->CCR     = (F_CPU / (3 * I2C_CLKRATE))     // set clock division factor
                | I2C_CCR_FS;                     // enable fast mode (400kHz)
  I2C1->TRISE   = 16;                             // set maximum rise time (300ns)
  #else
  I2C1->CCR     = (F_CPU / (2 * I2C_CLKRATE));    // set clock and standard mode (100kHz)
  I2C1->TRISE   = 16;                             // set maximum rise time (1000ns)
  #endif
  I2C1->CR1     = I2C_CR1_PE;                     // enable I2C

  // Setup DMA
  RCC->AHBENR   |= RCC_AHBENR_DMAEN;              // enable DMA module clock
  RCC->APBENR2  |= RCC_APBENR2_SYSCFGEN;          // enable SYSCFG module clock
  SYSCFG->CFGR3 |= (uint32_t)0b01001 << I2C_DMA_POS; // set I2C1 TX as trigger
  I2C_DMA_CHAN->CPAR  = (uint32_t)&I2C1->DR;      // peripheral address
  I2C_DMA_CHAN->CCR   = DMA_CCR_MINC              // increment memory address
                      | DMA_CCR_DIR               // memory to I2C
                      | DMA_CCR_TCIE;             // transfer complete interrupt enable
  DMA1->IFCR = (uint32_t)0b1111 << I2C_DMA_SHIFT; // clear interrupt flags
  NVIC_EnableIRQ(I2C_DMA_IRQn);                   // enable the DMA IRQ
}

// Start I2C transmission (addr must contain R/W bit)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
void I2C_start(uint8_t addr) {
  while(I2C1->SR2 & I2C_SR2_BUSY);                // wait until bus ready
  I2C1->CR1 |= I2C_CR1_START                      // set START condition
             | I2C_CR1_ACK;                       // set ACK
  while(!(I2C1->SR1 & I2C_SR1_SB));               // wait for START generated
  I2C1->DR = addr;                                // send slave address + R/W bit
  while(!(I2C1->SR1 & I2C_SR1_ADDR));             // wait for address transmitted
  uint16_t reg = I2C1->SR2;                       // clear flags
  I2C_rwflag = addr & 1;                          // set read/write flag
}
#pragma GCC diagnostic pop

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  while(!(I2C1->SR1 & I2C_SR1_TXE));              // wait for last byte transmitted
  I2C1->DR = data;                                // send data byte
}

// Read data byte via I2C bus (ack=0 for last byte, ack>0 if more bytes to follow)
uint8_t I2C_read(uint8_t ack) {
  if(!ack) {                                      // last byte?
    I2C1->CR1 &= ~I2C_CR1_ACK;                    // -> set NAK
    I2C1->CR1 |=  I2C_CR1_STOP;                   // -> set STOP condition
  }
  while(!(I2C1->SR1 & I2C_SR1_RXNE));             // wait for data byte received
  return I2C1->DR;                                // return received data byte
}

// Stop I2C transmission
void I2C_stop(void) {
  if(!I2C_rwflag) {                               // for read operation only
    while(!(I2C1->SR1 & I2C_SR1_BTF));            // wait for last byte transmitted
    I2C1->CR1 |= I2C_CR1_STOP;                    // set STOP condition
  }
}

// Send data buffer via I2C bus using DMA
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  I2C_DMA_CHAN->CNDTR = len;                      // number of bytes to be transfered
  I2C_DMA_CHAN->CMAR  = (uint32_t)buf;            // memory address
  I2C_DMA_CHAN->CCR  |= DMA_CCR_EN;               // enable DMA channel
  I2C1->CR2          |= I2C_CR2_DMAEN;            // enable DMA request
}

// Interrupt service routine
void I2C_DMA_ISR(void) __attribute__((interrupt));
void I2C_DMA_ISR(void) {
  I2C1->CR2 &= ~I2C_CR2_DMAEN;                    // disable DMA request
  I2C_DMA_CHAN->CCR &= ~DMA_CCR_EN;               // disable DMA channel
  DMA1->IFCR = (uint32_t)0b1111 << I2C_DMA_SHIFT; // clear interrupt flags
  while(!(I2C1->SR1 & I2C_SR1_BTF));              // wait for last byte transmitted
  I2C1->CR1 |= I2C_CR1_STOP;                      // set STOP condition
}

// Read data via I2C bus to buffer and stop
void I2C_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = I2C_read(len > 0);
}
