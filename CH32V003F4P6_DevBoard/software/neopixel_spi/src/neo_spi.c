// ===================================================================================
// Basic NeoPixel Functions using Hardware-SPI for CH32V003                   * v1.1 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_spi.h"

// ===================================================================================
// SPI Parameters and Variables
// ===================================================================================

// Define SPI prescaler for 3 MHz SPI frequency -> 333.33us per bit, 750kHz byte freq
#if F_CPU == 48000000
#define SPI_PRESC   3
#elif F_CPU == 24000000
#define SPI_PRESC   2
#elif F_CPU == 12000000
#define SPI_PRESC   1
#elif F_CPU ==  6000000
#define SPI_PRESC   0
#else
#error Unsupported system frequency for NeoPixels!
#endif

uint8_t NEO_buffer[3 * NEO_COUNT];          // pixel buffer

// ===================================================================================
// Init SPI for Neopixels
// ===================================================================================
void NEO_init(void) {
  // Enable GPIO and SPI module clock
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN | RCC_SPI1EN;
  
  // Setup GPIO pin PC6 (MOSI)
  GPIOC->CFGLR = (GPIOC->CFGLR & ~((uint32_t)0b1111<<(6<<2))) | ((uint32_t)0b1011<<(6<<2));

  // Setup and enable SPI master, TX only, standard configuration
  SPI1->CTLR1 = (SPI_PRESC << 3)            // set prescaler
              | SPI_CTLR1_MSTR              // master configuration
              | SPI_CTLR1_BIDIMODE          // one-line mode
              | SPI_CTLR1_BIDIOE            // transmit only
              | SPI_CTLR1_SSM               // software control of NSS
              | SPI_CTLR1_SSI               // set internal NSS high
              | SPI_CTLR1_SPE;              // enable SPI
}

// ===================================================================================
// Send one Data Byte to Neopixels
// ===================================================================================
void NEO_sendByte(uint8_t data) {
  uint8_t i, bits;
  for(i=4; i; i--, data<<=2) {              // 4x2 bits, MSB first
    if(data & 0x80) bits  = 0x60;           // 667us high for "1"-bit
    else            bits  = 0x40;           // 333us high for "0"-bit
    if(data & 0x40) bits |= 0x06;           // 667us high for "1"-bit
    else            bits |= 0x04;           // 333us high for "0"-bit
    while(!(SPI1->STATR & SPI_STATR_TXE));  // wait for transmit buffer empty
    SPI1->DATAR = bits;                     // send 2 bits
  }
}

// ===================================================================================
// Write Buffer to Pixels
// ===================================================================================
void NEO_update(void) {
  uint8_t i;
  uint8_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) NEO_sendByte(*ptr++);
  NEO_latch();
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint8_t i;
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
