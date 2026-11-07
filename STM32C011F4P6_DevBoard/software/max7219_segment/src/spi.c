// ===================================================================================
// Basic SPI Master Functions for STM32C0xx                                   * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include"spi.h"

#define SPI_DR_8BIT *((volatile uint8_t*) &(SPI1->DR))  // for 8-bit data transfer

void SPI_init(void) {
  // Set GPIO pins
  #if SPI_MAP == 0
    // Setup GPIO pin PA5 (SCK), PA6 (MISO), PA7 (MOSI)
    RCC->IOPENR   |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(5<<1)) | ((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(7<<1))))
                      |  (((uint32_t)0b10<<(5<<1)) | ((uint32_t)0b10<<(6<<1)) | ((uint32_t)0b10<<(7<<1)));
    GPIOA->OTYPER  &= ~(((uint32_t)1<<5) | ((uint32_t)1<<7));
    GPIOA->OSPEEDR |= (((uint32_t)0b11<<(5<<1)) | ((uint32_t)0b11<<(7<<1)));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(6<<1)))
                      |  ((uint32_t)0b01<<(6<<1));
    GPIOA->AFR[0]  &= ~(((uint32_t)0xf<<(5<<2)) | ((uint32_t)0xf<<(6<<2)) | ((uint32_t)0xf<<(7<<2)));
  #elif SPI_MAP == 1
    // Setup GPIO pin PA1 (SCK), PA6 (MISO), PA2 (MOSI)
    RCC->IOPENR   |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(1<<1)) | ((uint32_t)0b11<<(6<<1)) | ((uint32_t)0b11<<(2<<1))))
                      |  (((uint32_t)0b10<<(1<<1)) | ((uint32_t)0b10<<(6<<1)) | ((uint32_t)0b10<<(2<<1)));
    GPIOA->OTYPER  &= ~(((uint32_t)1<<1) | ((uint32_t)1<<2));
    GPIOA->OSPEEDR |= (((uint32_t)0b11<<(1<<1)) | ((uint32_t)0b11<<(2<<1)));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(6<<1)))
                      |  ((uint32_t)0b01<<(6<<1));
    GPIOA->AFR[0]  &= ~(((uint32_t)0xf<<(1<<2)) | ((uint32_t)0xf<<(6<<2)) | ((uint32_t)0xf<<(2<<2)));
  #elif SPI_MAP == 2
    // Setup GPIO pin PA5 (SCK), PA11 (MISO), PA12 (MOSI)
    RCC->IOPENR   |= RCC_IOPENR_GPIOAEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(5<<1)) | ((uint32_t)0b11<<(11<<1)) | ((uint32_t)0b11<<(12<<1))))
                      |  (((uint32_t)0b10<<(5<<1)) | ((uint32_t)0b10<<(11<<1)) | ((uint32_t)0b10<<(12<<1)));
    GPIOA->OTYPER  &= ~(((uint32_t)1<<5) | ((uint32_t)1<<12));
    GPIOA->OSPEEDR |= (((uint32_t)0b11<<(5<<1)) | ((uint32_t)0b11<<(12<<1)));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(11<<1)))
                      |  ((uint32_t)0b01<<(11<<1));
    GPIOA->AFR[0]  &= ~((uint32_t)0xf<<(5<<2));
    GPIOA->AFR[1]  &= ~(((uint32_t)0xf<<(3<<2)) | ((uint32_t)0xf<<(4<<2)));
  #elif SPI_MAP == 3
    // Setup GPIO pin PB3 (SCK), PB4 (MISO), PB5 (MOSI)
    RCC->IOPENR   |= RCC_IOPENR_GPIOBEN;
    GPIOB->MODER    = (GPIOB->MODER  & ~(((uint32_t)0b11<<(3<<1)) | ((uint32_t)0b11<<(4<<1)) | ((uint32_t)0b11<<(5<<1))))
                      |  (((uint32_t)0b10<<(3<<1)) | ((uint32_t)0b10<<(4<<1)) | ((uint32_t)0b10<<(5<<1)));
    GPIOB->OTYPER  &= ~(((uint32_t)1<<3) | ((uint32_t)1<<5));
    GPIOB->OSPEEDR |= (((uint32_t)0b11<<(3<<1)) | ((uint32_t)0b11<<(5<<1)));
    GPIOB->PUPDR    = (GPIOB->PUPDR  & ~((uint32_t)0b11<<(4<<1)))
                      |  ((uint32_t)0b01<<(4<<1));
    GPIOB->AFR[0]  &= ~(((uint32_t)0xf<<(3<<2)) | ((uint32_t)0xf<<(4<<2)) | ((uint32_t)0xf<<(5<<2)));
  #elif SPI_MAP == 4
    // Setup GPIO pin PA5 (SCK), PA6 (MISO), PB6 (MOSI)
    RCC->IOPENR   |= RCC_IOPENR_GPIOAEN | RCC_IOPENR_GPIOBEN;
    GPIOA->MODER    = (GPIOA->MODER  & ~(((uint32_t)0b11<<(5<<1)) | ((uint32_t)0b11<<(6<<1))))
                      |  (((uint32_t)0b10<<(5<<1)) | ((uint32_t)0b10<<(6<<1)));
    GPIOA->OTYPER  &= ~((uint32_t)1<<5);
    GPIOA->OSPEEDR |= ((uint32_t)0b11<<(5<<1));
    GPIOA->PUPDR    = (GPIOA->PUPDR  & ~((uint32_t)0b11<<(6<<1)))
                      |  ((uint32_t)0b01<<(6<<1));
    GPIOA->AFR[0]  &= ~(((uint32_t)0xf<<(5<<2)) | ((uint32_t)0xf<<(6<<2)));
    GPIOB->MODER    = (GPIOB->MODER  & ~((uint32_t)0b11<<(6<<1)))
                      |  ((uint32_t)0b10<<(6<<1));
    GPIOB->OTYPER  &= ~((uint32_t)1<<6);
    GPIOB->OSPEEDR |= ((uint32_t)0b11<<(6<<1));
    GPIOB->AFR[0]   = (GPIOB->AFR[0] & ~((uint32_t)0xf<<(6<<2)))
                      |  ((uint32_t)0x8<<(6<<2));
  #else
    #warning No automatic pin mapping for SPI
  #endif

  // Setup and enable SPI master, standard configuration
  RCC->APBENR2 |= RCC_APBENR2_SPI1EN;
  SPI1->CR2     = SPI_CR2_FRXTH                 // set RXNE flag after 8-bit
                | (7 << SPI_CR2_DS_Pos);        // 8-bit data size
  SPI1->CR1     = (SPI_PRESC << 3)              // set prescaler
                | SPI_CR1_MSTR                  // master configuration
                | SPI_CR1_SSM                   // software control of NSS
                | SPI_CR1_SSI;                  // set internal NSS high
  SPI1->CR1    |= SPI_CR1_SPE;                  // enable SPI
}

// Transfer one data byte (read and write)
uint8_t SPI_transfer(uint8_t data) {
  SPI_DR_8BIT = data;                 // send data byte
  while(!SPI_available());            // wait for data byte received
  return(SPI_DR_8BIT);                // return received data byte
}
