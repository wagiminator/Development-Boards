// ===================================================================================
// Basic NeoPixel Functions using Hardware-SPI for CH32V203                   * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_spi.h"

// ===================================================================================
// SPI Parameters and Variables
// ===================================================================================

// SPI module
#if NEO_MAP < 2
  #define NEO_SPI     SPI1
#else
  #define NEO_SPI     SPI2
#endif

// Define SPI prescaler (SPI clock = F_CPU / (2 << SPI_PRESC)) and bit patterns
// (MSB first) for "0"-bit and "1"-bit depending on system frequency:
// - 6.0MHz SPI: 166.7ns per SPI bit, "0": 333ns HIGH, "1": 833ns HIGH, 1333ns total
// - 9.0MHz SPI: 111.1ns per SPI bit, "0": 333ns HIGH, "1": 667ns HIGH,  889ns total
// - 7.5MHz SPI: 133.3ns per SPI bit, "0": 400ns HIGH, "1": 800ns HIGH, 1067ns total
#if   F_CPU == 144000000
  #define SPI_PRESC   3
  #define NEO_BIT0    0x70
  #define NEO_BIT1    0x7E
#elif F_CPU == 120000000
  #define SPI_PRESC   3
  #define NEO_BIT0    0x70
  #define NEO_BIT1    0x7E
#elif F_CPU == 96000000
  #define SPI_PRESC   3
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#elif F_CPU == 72000000
  #define SPI_PRESC   2
  #define NEO_BIT0    0x70
  #define NEO_BIT1    0x7E
#elif F_CPU == 48000000
  #define SPI_PRESC   2
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#elif F_CPU == 24000000
  #define SPI_PRESC   1
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#elif F_CPU == 12000000
  #define SPI_PRESC   0
  #define NEO_BIT0    0x60
  #define NEO_BIT1    0x7C
#else
  #error Unsupported system frequency for NeoPixels!
#endif

uint8_t NEO_buffer[3 * NEO_COUNT];          // pixel buffer

// ===================================================================================
// Init SPI for Neopixels
// ===================================================================================
void NEO_init(void) {
  // Set GPIO pin (alternate function, push-pull, 50MHz)
  #if NEO_MAP == 0
    // Setup GPIO pin PA7 (MOSI)
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPAEN | RCC_SPI1EN;
    AFIO->PCFR1    &= ~AFIO_PCFR1_SPI1_REMAP;
    GPIOA->CFGLR    = (GPIOA->CFGLR & ~((uint32_t)0b1111<<(7<<2))) | ((uint32_t)0b1011<<(7<<2));
  #elif NEO_MAP == 1
    // Setup GPIO pin PB5 (MOSI)
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN | RCC_SPI1EN;
    AFIO->PCFR1    |= AFIO_PCFR1_SPI1_REMAP;
    GPIOB->CFGLR    = (GPIOB->CFGLR & ~((uint32_t)0b1111<<(5<<2))) | ((uint32_t)0b1011<<(5<<2));
  #elif NEO_MAP == 2
    // Setup GPIO pin PB15 (MOSI)
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPBEN;
    RCC->APB1PCENR |= RCC_SPI2EN;
    GPIOB->CFGHR    = (GPIOB->CFGHR & ~((uint32_t)0b1111<<(7<<2))) | ((uint32_t)0b1011<<(7<<2));
  #else
    #error Wrong NEO_MAP
  #endif

  // Setup and enable SPI master, TX only, standard configuration
  NEO_SPI->CTLR1 = (SPI_PRESC << 3)         // set prescaler
                 | SPI_CTLR1_MSTR           // master configuration
                 | SPI_CTLR1_BIDIMODE       // one-line mode
                 | SPI_CTLR1_BIDIOE         // transmit only
                 | SPI_CTLR1_SSM            // software control of NSS
                 | SPI_CTLR1_SSI            // set internal NSS high
                 | SPI_CTLR1_SPE;           // enable SPI
}

// ===================================================================================
// Send one Data Byte to Neopixels
// ===================================================================================
void NEO_sendByte(uint8_t data) {
  uint8_t i;
  for(i=8; i; i--, data<<=1) {              // 8 bits, MSB first
    while(!(NEO_SPI->STATR & SPI_STATR_TXE)); // wait for transmit buffer empty
    NEO_SPI->DATAR = (data & 0x80) ? NEO_BIT1 : NEO_BIT0;
  }
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  while(!(NEO_SPI->STATR & SPI_STATR_TXE));   // wait for last byte in shift register
  while(NEO_SPI->STATR & SPI_STATR_BSY);      // wait for last byte transmitted
  NEO_latch();
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint16_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) *ptr++ = 0;
  NEO_update();
}

// ===================================================================================
// Write Color to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  uint8_t *ptr;
  ptr = NEO_buffer + (3 * pixel);
  #if defined (NEO_GRB)
    *ptr++ = g; *ptr++ = r; *ptr = b;
  #elif defined (NEO_RGB)
    *ptr++ = r; *ptr++ = g; *ptr = b;
  #else
    #error Wrong or missing NeoPixel type definition!
  #endif
}

// ===================================================================================
// Write Hue Value (0..191) and Brightness (0..2) to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeHue(uint8_t pixel, uint8_t hue, uint8_t bright) {
  uint8_t phase = hue >> 6;
  uint8_t step  = (hue & 63) << bright;
  uint8_t nstep = (63 << bright) - step;
  switch(phase) {
    case 0:   NEO_writeColor(pixel, nstep,  step,     0); break;
    case 1:   NEO_writeColor(pixel,     0, nstep,  step); break;
    case 2:   NEO_writeColor(pixel,  step,     0, nstep); break;
    default:  break;
  }
}

// ===================================================================================
// Clear Single Pixel in Buffer
// ===================================================================================
void NEO_clearPixel(uint8_t pixel) {
  NEO_writeColor(pixel, 0, 0, 0);
}
