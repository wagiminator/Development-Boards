// ===================================================================================
// Basic I2C Master Functions for STM32C0xx                                   * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "i2c.h"

// I2C timings (see RM0490, chapter 23.4.10), I2C kernel clock = PCLK = F_CPU
#if   I2C_CLKRATE <= 100000                     // Standard-mode (Sm)
  #define I2C_TREF        4000000               // max prescaled timing clock
  #define I2C_TLOW_NS     (500000000 / I2C_CLKRATE)   // SCL low  time: 50% of period
  #define I2C_THIGH_NS    (400000000 / I2C_CLKRATE)   // SCL high time: 40% of period
  #define I2C_SCLDEL_NS   1250                  // data setup time (tr + tSU;DAT)
  #define I2C_SDADEL_NS   500                   // data hold time
  #define I2C_FCPU_MIN    1000000               // min I2C kernel clock (*)
#elif I2C_CLKRATE <= 400000                     // Fast-mode (Fm)
  #define I2C_TREF        8000000
  #define I2C_TLOW_NS     (500000000 / I2C_CLKRATE)
  #define I2C_THIGH_NS    (200000000 / I2C_CLKRATE)
  #define I2C_SCLDEL_NS   500
  #define I2C_SDADEL_NS   250
  #define I2C_FCPU_MIN    4000000
#else                                           // Fast-mode Plus (Fm+)
  #define I2C_TREF        8000000
  #define I2C_TLOW_NS     (500000000 / I2C_CLKRATE)
  #define I2C_THIGH_NS    (250000000 / I2C_CLKRATE)
  #define I2C_SCLDEL_NS   250
  #define I2C_SDADEL_NS   0
  #define I2C_FCPU_MIN    17000000
  #define I2C_FMP         1                     // enable Fast-mode Plus drive
#endif

// (*) tI2CCLK < (tLOW - tfilters) / 4 and tI2CCLK < tHIGH (RM0490, chapter 23.4.4)

#define I2C_PRESC         ((F_CPU + I2C_TREF - 1) / I2C_TREF - 1)
#define I2C_TDIV          (1000000000ULL * (I2C_PRESC + 1))
#define I2C_TICKS(ns)     ((1ULL * (ns) * F_CPU + I2C_TDIV - 1) / I2C_TDIV)
#define I2C_SCLL          (I2C_TICKS(I2C_TLOW_NS)   - 1)
#define I2C_SCLH          (I2C_TICKS(I2C_THIGH_NS)  - 1)
#define I2C_SCLDEL        (I2C_TICKS(I2C_SCLDEL_NS) - 1)
#define I2C_SDADEL        ((1ULL * I2C_SDADEL_NS * F_CPU) / I2C_TDIV)

#if F_CPU < I2C_FCPU_MIN
  #warning F_CPU too low for selected I2C_CLKRATE
#endif
#if (I2C_SCLL > 255) || (I2C_SCLH > 255)
  #error I2C_CLKRATE too low
#endif

// I2C transfer state
static uint8_t I2C_addr;                          // slave address with R/W bit
static uint8_t I2C_data;                          // pending data byte (write)
static uint8_t I2C_state;                         // transfer state flags
#define I2C_ST_OPEN       0x01                    // transfer opened by I2C_start()
#define I2C_ST_FIRST      0x02                    // START condition not yet sent
#define I2C_ST_PENDING    0x04                    // data byte waiting to be sent
#define I2C_ST_RELOAD     0x08                    // last byte was read with ACK

// The I2C peripheral needs to know the number of bytes of a transfer in advance. In
// order to support the simple byte-wise API, each byte is transferred as a single-
// byte chunk in reload mode. When writing, one byte is held back until it is known
// whether it is the last one (I2C_stop() or I2C_start() for a repeated start).

// Wait for ISR flag; on NACK (the peripheral then sends STOP automatically) close
// the transfer and return 0
static uint8_t I2C_wait(uint32_t flag) {
  uint32_t isr;
  do isr = I2C1->ISR;
  while(!(isr & (flag | I2C_ISR_NACKF)));         // wait for flag or NACK
  if(!(isr & flag)) {                             // NACK received?
    while(!(I2C1->ISR & I2C_ISR_STOPF));          // wait for automatic STOP
    I2C1->ICR = I2C_ICR_NACKCF | I2C_ICR_STOPCF;  // clear flags
    I2C1->ISR = I2C_ISR_TXE;                      // flush TX data register
    I2C_state = 0;                                // close transfer
    return 0;
  }
  return 1;
}

// Program next chunk with nbytes data bytes, send (repeated) START if necessary
static uint8_t I2C_program(uint8_t nbytes, uint32_t mode) {
  if(I2C_state & I2C_ST_FIRST) {                  // first chunk?
    I2C_state &= ~I2C_ST_FIRST;
    I2C1->CR2 = ((uint32_t)nbytes << I2C_CR2_NBYTES_Pos) | mode
              | (I2C_addr & 0xfe)                 // slave address
              | ((I2C_addr & 1) ? I2C_CR2_RD_WRN : 0) // transfer direction
              | I2C_CR2_START;                    // send (repeated) START
    return 1;
  }
  if(!I2C_wait(I2C_ISR_TCR)) return 0;            // wait for last chunk completed
  I2C1->CR2 = (I2C1->CR2 & ~(I2C_CR2_NBYTES | I2C_CR2_RELOAD | I2C_CR2_AUTOEND))
            | ((uint32_t)nbytes << I2C_CR2_NBYTES_Pos) | mode;
  return 1;
}

// Send pending data byte as a single-byte chunk
static void I2C_sendPending(uint32_t mode) {
  I2C_state &= ~I2C_ST_PENDING;
  if(I2C_program(1, mode) && I2C_wait(I2C_ISR_TXIS)) I2C1->TXDR = I2C_data;
}

// Finish current transfer (send pending byte, wait for transfer complete)
static void I2C_finish(void) {
  if(I2C_state & I2C_ST_PENDING) I2C_sendPending(0);      // send last byte
  else if(I2C_state & I2C_ST_FIRST) I2C_program(0, 0);    // only address, no data
  else if(I2C_state & I2C_ST_RELOAD) {                    // last byte read with ACK?
    I2C_state &= ~I2C_ST_RELOAD;                          // -> read one more byte
    if(I2C_program(1, 0) && I2C_wait(I2C_ISR_RXNE)) (void)I2C1->RXDR; // with NACK
  }
  if(I2C_state & I2C_ST_OPEN) I2C_wait(I2C_ISR_TC); // wait for transfer complete
}

// Start I2C transmission (addr must contain R/W bit)
void I2C_start(uint8_t addr) {
  if(I2C_state & I2C_ST_OPEN) I2C_finish();       // repeated start -> finish transfer
  else while(I2C_busy());                         // wait until bus is free
  I2C_addr  = addr;                               // store address
  I2C_state = I2C_ST_OPEN | I2C_ST_FIRST;         // open transfer
}

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  if(!(I2C_state & I2C_ST_OPEN)) return;          // no open transfer (or NACK)
  if(I2C_state & I2C_ST_PENDING) I2C_sendPending(I2C_CR2_RELOAD); // send previous byte
  if(!(I2C_state & I2C_ST_OPEN)) return;          // NACK received
  I2C_data   = data;                              // hold back this byte
  I2C_state |= I2C_ST_PENDING;
}

// Read data byte via I2C bus (ack=0 for last byte, ack>0 if more bytes to follow)
uint8_t I2C_read(uint8_t ack) {
  if(!(I2C_state & I2C_ST_OPEN)) return 0xff;     // no open transfer (or NACK)
  if(ack) I2C_state |=  I2C_ST_RELOAD;            // ACK -> reload mode
  else    I2C_state &= ~I2C_ST_RELOAD;            // NACK after last byte
  if(!I2C_program(1, ack ? I2C_CR2_RELOAD : 0)) return 0xff;
  if(!I2C_wait(I2C_ISR_RXNE)) return 0xff;        // wait for data byte received
  return I2C1->RXDR;                              // return received data byte
}

// Stop I2C transmission
void I2C_stop(void) {
  if(I2C_state & I2C_ST_OPEN) {                   // open transfer?
    I2C_finish();                                 // finish transfer
    if(I2C_state & I2C_ST_OPEN) {                 // no NACK?
      I2C1->CR2 |= I2C_CR2_STOP;                  // send STOP condition
      while(!(I2C1->ISR & I2C_ISR_STOPF));        // wait for STOP sent
      I2C1->ICR  = I2C_ICR_STOPCF;                // clear STOP flag
    }
  }
  I2C_state = 0;                                  // close transfer
}

// Init I2C
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP == 0
    // Setup pin PA9 (SCL) and pin PA10 (SDA)
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1))))
                      |  (((uint32_t)0b10<<(9<<1)) | ((uint32_t)0b10<<(10<<1)));
    GPIOA->OTYPER  |=  (((uint32_t)1<<9) | ((uint32_t)1<<10));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1))))
                      |  (((uint32_t)0b01<<(9<<1)) | ((uint32_t)0b01<<(10<<1)));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~(((uint32_t)0xf<<(1<<2)) | ((uint32_t)0xf<<(2<<2))))
                      |  (((uint32_t)0x6<<(1<<2)) | ((uint32_t)0x6<<(2<<2)));
  #elif I2C_MAP == 1
    // Setup pin PA9 (SCL) and pin PA10 (SDA), remapped to PA11/PA12 by SYSCFG
    RCC->IOPENR    |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1))))
                      |  (((uint32_t)0b10<<(9<<1)) | ((uint32_t)0b10<<(10<<1)));
    GPIOA->OTYPER  |=  (((uint32_t)1<<9) | ((uint32_t)1<<10));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~(((uint32_t)0b11<<(9<<1)) | ((uint32_t)0b11<<(10<<1))))
                      |  (((uint32_t)0b01<<(9<<1)) | ((uint32_t)0b01<<(10<<1)));
    GPIOA->AFR[1]   = (GPIOA->AFR[1] & ~(((uint32_t)0xf<<(1<<2)) | ((uint32_t)0xf<<(2<<2))))
                      |  (((uint32_t)0x6<<(1<<2)) | ((uint32_t)0x6<<(2<<2)));
    RCC->APBENR2   |= RCC_APBENR2_SYSCFGEN;
    SYSCFG->CFGR1  |= SYSCFG_CFGR1_PA11_RMP | SYSCFG_CFGR1_PA12_RMP;
  #elif I2C_MAP == 2
    // Setup pin PB6 (SCL) and pin PB7 (SDA)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~(((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1))))
                      |  (((uint32_t)0b10<<(6<<1)) | ((uint32_t)0b10<<(7<<1)));
    GPIOB->OTYPER  |=  (((uint32_t)1<<6) | ((uint32_t)1<<7));
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~(((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1))))
                      |  (((uint32_t)0b01<<(6<<1)) | ((uint32_t)0b01<<(7<<1)));
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~(((uint32_t)0xf<<(6<<2)) | ((uint32_t)0xf<<(7<<2))))
                      |  (((uint32_t)0x6<<(6<<2)) | ((uint32_t)0x6<<(7<<2)));
  #elif I2C_MAP == 3
    // Setup pin PB8 (SCL) and pin PB9 (SDA)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~(((uint32_t)0b11<<(8<<1)) | ((uint32_t)0b11<<(9<<1))))
                      |  (((uint32_t)0b10<<(8<<1)) | ((uint32_t)0b10<<(9<<1)));
    GPIOB->OTYPER  |=  (((uint32_t)1<<8) | ((uint32_t)1<<9));
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~(((uint32_t)0b11<<(8<<1)) | ((uint32_t)0b11<<(9<<1))))
                      |  (((uint32_t)0b01<<(8<<1)) | ((uint32_t)0b01<<(9<<1)));
    GPIOB->AFR[1]   = (GPIOB->AFR[1] & ~(((uint32_t)0xf<<(0<<2)) | ((uint32_t)0xf<<(1<<2))))
                      |  (((uint32_t)0x6<<(0<<2)) | ((uint32_t)0x6<<(1<<2)));
  #elif I2C_MAP == 4
    // Setup pin PB7 (SCL) and pin PC14 (SDA)
    RCC->IOPENR    |= RCC_IOPENR_GPIOBEN | RCC_IOPENR_GPIOCEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~((uint32_t)0b11<<(7<<1)))
                      |  ((uint32_t)0b10<<(7<<1));
    GPIOB->OTYPER  |=  ((uint32_t)1<<7);
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~((uint32_t)0b11<<(7<<1)))
                      |  ((uint32_t)0b01<<(7<<1));
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~((uint32_t)0xf<<(7<<2)))
                      |  ((uint32_t)0xe<<(7<<2));
    GPIOC->MODER    = (GPIOC->MODER  & ~((uint32_t)0b11<<(14<<1)))
                      |  ((uint32_t)0b10<<(14<<1));
    GPIOC->OTYPER  |=  ((uint32_t)1<<14);
    GPIOC->PUPDR    = (GPIOC->PUPDR  & ~((uint32_t)0b11<<(14<<1)))
                      |  ((uint32_t)0b01<<(14<<1));
    GPIOC->AFR[1]   = (GPIOC->AFR[1] & ~((uint32_t)0xf<<(6<<2)))
                      |  ((uint32_t)0xe<<(6<<2));
  #else
    #warning No automatic pin mapping for I2C1
  #endif

  // Setup and enable I2C
  RCC->APBENR1  |=  RCC_APBENR1_I2C1EN;           // enable I2C module clock
  RCC->APBRSTR1 |=  RCC_APBRSTR1_I2C1RST;         // reset I2C module
  RCC->APBRSTR1 &= ~RCC_APBRSTR1_I2C1RST;
  RCC->CCIPR    &= ~RCC_CCIPR_I2C1SEL;            // I2C kernel clock = PCLK
  #ifdef I2C_FMP
  RCC->APBENR2  |=  RCC_APBENR2_SYSCFGEN;         // enable SYSCFG module clock
  SYSCFG->CFGR1 |=  SYSCFG_CFGR1_I2C1_FMP;        // enable Fast-mode Plus drive
  #endif
  I2C1->TIMINGR  = ((uint32_t)I2C_PRESC  << I2C_TIMINGR_PRESC_Pos)
                 | ((uint32_t)I2C_SCLDEL << I2C_TIMINGR_SCLDEL_Pos)
                 | ((uint32_t)I2C_SDADEL << I2C_TIMINGR_SDADEL_Pos)
                 | ((uint32_t)I2C_SCLH   << I2C_TIMINGR_SCLH_Pos)
                 | ((uint32_t)I2C_SCLL   << I2C_TIMINGR_SCLL_Pos);
  I2C1->CR1      = I2C_CR1_PE;                    // enable I2C
  I2C_state      = 0;                             // no transfer open
}

// Send data buffer via I2C bus and stop
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  while(len--) I2C_write(*buf++);                 // write buffer
  I2C_stop();                                     // stop transmission
}

// Read data via I2C bus to buffer and stop
void I2C_readBuffer(uint8_t* buf, uint16_t len) {
  while(len--) *buf++ = I2C_read(len > 0);        // read buffer
  I2C_stop();                                     // stop transmission
}
