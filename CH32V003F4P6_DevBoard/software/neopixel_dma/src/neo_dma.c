// ===================================================================================
// Basic NeoPixel Functions using Hardware-SPI and DMA for CH32V003           * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "neo_dma.h"

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

uint32_t NEO_buffer[3 * NEO_COUNT];           // pixel buffer
uint8_t  NEO_TX_flag = 0;                     // transfer running flag
volatile uint32_t NEO_latch_end;              // latch timer

// ===================================================================================
// Init SPI with DMA for Neopixels
// ===================================================================================
void NEO_init(void) {
  // Enable GPIO and SPI module clock
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN | RCC_SPI1EN;
  
  // Setup GPIO pin PC6 (MOSI)
  GPIOC->CFGLR = (GPIOC->CFGLR & ~((uint32_t)0b1111<<(6<<2))) | ((uint32_t)0b1011<<(6<<2));

  // Setup and enable SPI master, TX only, standard configuration
  SPI1->CTLR1 = (SPI_PRESC << 3)              // set prescaler
              | SPI_CTLR1_MSTR                // master configuration
              | SPI_CTLR1_BIDIMODE            // one-line mode
              | SPI_CTLR1_BIDIOE              // transmit only
              | SPI_CTLR1_SSM                 // software control of NSS
              | SPI_CTLR1_SSI                 // set internal NSS high
              | SPI_CTLR1_SPE;                // enable SPI

  // Setup DMA Channel 3
  RCC->AHBPCENR |= RCC_DMA1EN;                    // enable DMA module clock
  DMA1_Channel3->PADDR = (uint32_t)&SPI1->DATAR;  // peripheral address
  DMA1_Channel3->MADDR = (uint32_t)NEO_buffer;    // memory address
  DMA1_Channel3->CFGR  = DMA_CFGR3_MINC           // increment memory address
                       | DMA_CFGR3_DIR            // memory to I2C
                       | DMA_CFGR3_TCIE;          // transfer complete interrupt enable
  DMA1->INTFCR         = DMA_CGIF3;               // clear interrupt flags
  NVIC_EnableIRQ(DMA1_Channel3_IRQn);             // enable the DMA IRQ
}

// ===================================================================================
// Latch the Data
// ===================================================================================
void NEO_latch(void) {
  if(NEO_TX_flag) {
    while(SPI1->STATR & SPI_STATR_BSY);       // wait for end of last transmission
    while(((int32_t)(STK->CNT - NEO_latch_end)) < 0); // wait for end of latch
    NEO_TX_flag = 0;                          // clear transmission flag
  }
  else DLY_us(NEO_LATCH_TIME);                // or just delay the latch time
}

// ===================================================================================
// Start writing Buffer to Pixels via DMA
// ===================================================================================
void NEO_update(void) {
  NEO_latch();                                // make sure last data was latched
  DMA1_Channel3->CNTR  = 12 * NEO_COUNT;      // number of bytes to be transfered
  DMA1_Channel3->CFGR |= DMA_CFGR3_EN;        // enable DMA channel
  SPI1->CTLR2         |= SPI_CTLR2_TXDMAEN;   // enable DMA request
  NEO_TX_flag = 1;                            // set transmission flag
}

// ===================================================================================
// Interrupt Service Routine
// ===================================================================================
void DMA1_Channel3_IRQHandler(void) __attribute__((interrupt));
void DMA1_Channel3_IRQHandler(void) {
  SPI1->CTLR2         &= ~SPI_CTLR2_TXDMAEN;  // disable DMA request
  DMA1_Channel3->CFGR &= ~DMA_CFGR3_EN;       // disable DMA channel
  DMA1->INTFCR         =  DMA_CGIF3;          // clear interrupt flags    
  NEO_latch_end = STK->CNT + ((NEO_LATCH_TIME + 5) * DLY_US_TIME);  // end of latch
}

// ===================================================================================
// Clear all Pixels
// ===================================================================================
void NEO_clearAll(void) {
  uint8_t i;
  uint32_t *ptr;
  ptr = NEO_buffer;
  for(i=3*NEO_COUNT; i; i--) *ptr++ = 0x44444444;
  NEO_update();
}

// ===================================================================================
// Convert a Color Byte into a SPI Bit Mask
// ===================================================================================
uint32_t NEO_SPI_mask(uint8_t data) {
  uint8_t i;
  uint32_t result = 0;                        // bit mask for SPI transmission
  for(i=4; i; i--, data>>=2) {
    result <<= 8;
    if(data & 0x01) result |= 0x06;           // 667us high for "1"-bit
    else            result |= 0x04;           // 333us high for "0"-bit
    if(data & 0x02) result |= 0x60;           // 667us high for "1"-bit
    else            result |= 0x40;           // 333us high for "0"-bit
  }  
  return result;
}

// ===================================================================================
// Write Color to a Single Pixel in Buffer
// ===================================================================================
void NEO_writeColor(uint8_t pixel, uint8_t r, uint8_t g, uint8_t b) {
  uint32_t *ptr;
  ptr = NEO_buffer + (3 * pixel);
  #if defined (NEO_GRB)
    *ptr++ = NEO_SPI_mask(g);
    *ptr++ = NEO_SPI_mask(r);
    *ptr   = NEO_SPI_mask(b);
  #elif defined (NEO_RGB)
    *ptr++ = NEO_SPI_mask(r);
    *ptr++ = NEO_SPI_mask(g);
    *ptr   = NEO_SPI_mask(b);
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
