// ===================================================================================
// SSD1306 128x64 Pixels I2C OLED Continuous DMA Refresh Functions            * v0.1 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "oled_dma.h"

// ===================================================================================
// I2C DMA Functions
// ===================================================================================

// Init I2C
void I2C_init(void) {
  // Setup GPIO pins
  #if I2C_MAP == 0
    // Set pin PC1 (SDA) and PC2 (SCL) to output, open-drain, 10MHz, multiplex
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
    GPIOC->CFGLR = (GPIOC->CFGLR & ~(((uint32_t)0b1111<<(1<<2)) | ((uint32_t)0b1111<<(2<<2))))
                                 |  (((uint32_t)0b1101<<(1<<2)) | ((uint32_t)0b1101<<(2<<2)));
  #elif I2C_MAP == 1
    // Set pin PD0 (SDA) and PD1 (SCL) to output, open-drain, 10MHz, multiplex
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPDEN;
    AFIO->PCFR1    |= 1<<1;
    GPIOD->CFGLR = (GPIOD->CFGLR & ~(((uint32_t)0b1111<<(0<<2)) | ((uint32_t)0b1111<<(1<<2))))
                                 |  (((uint32_t)0b1101<<(0<<2)) | ((uint32_t)0b1101<<(1<<2)));
  #elif I2C_MAP == 2
    // Set pin PC6 (SDA) and PC5 (SCL) to output, open-drain, 10MHz, multiplex
    RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN;
    AFIO->PCFR1    |= 1<<22;
    GPIOC->CFGLR = (GPIOC->CFGLR & ~(((uint32_t)0b1111<<(6<<2)) | ((uint32_t)0b1111<<(5<<2))))
                                 |  (((uint32_t)0b1101<<(6<<2)) | ((uint32_t)0b1101<<(5<<2)));
  #else
    #warning Wrong I2C REMAP
  #endif

  // Setup and enable I2C
  RCC->APB1PCENR |= RCC_I2C1EN;                   // enable I2C module clock
  I2C1->CTLR2     = 4;                            // set input clock rate
  I2C1->CKCFGR    = (F_CPU / (3 * I2C_CLKRATE))   // set clock division factor 1:2
                  | I2C_CKCFGR_FS;                // enable fast mode (400kHz)
  I2C1->CTLR1     = I2C_CTLR1_PE;                 // enable I2C

  // Setup DMA Channel 5
  RCC->AHBPCENR |= RCC_DMA1EN;                    // enable DMA module clock
  DMA1_Channel6->PADDR = (uint32_t)&I2C1->DATAR;  // peripheral address
  DMA1_Channel6->CFGR  = DMA_CFG6_MINC            // increment memory address
                       | DMA_CFG6_CIRC            // circular mode
                       | DMA_CFG6_DIR             // memory to I2C
                       | DMA_CFG6_TCIE;           // transfer complete interrupt enable
}

// Start I2C transmission (addr must contain R/W bit)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
void I2C_start(uint8_t addr) {
  while(I2C1->STAR2 & I2C_STAR2_BUSY);            // wait until bus ready
  I2C1->CTLR1 |= I2C_CTLR1_START                  // set START condition
               | I2C_CTLR1_ACK;                   // set ACK
  while(!(I2C1->STAR1 & I2C_STAR1_SB));           // wait for START generated
  I2C1->DATAR = addr;                             // send slave address + R/W bit
  while(!(I2C1->STAR1 & I2C_STAR1_ADDR));         // wait for address transmitted
  uint16_t reg = I2C1->STAR2;                     // clear flags
}
#pragma GCC diagnostic pop

// Send data byte via I2C bus
void I2C_write(uint8_t data) {
  while(!(I2C1->STAR1 & I2C_STAR1_TXE));          // wait for last byte transmitted
  I2C1->DATAR = data;                             // send data byte
}

// Stop I2C transmission
void I2C_stop(void) {
  while(!(I2C1->STAR1 & I2C_STAR1_BTF));          // wait for last byte transmitted
  I2C1->CTLR1 |= I2C_CTLR1_STOP;                  // set STOP condition
}

// Start sending data buffer continuously via I2C bus using DMA
void I2C_writeBuffer(uint8_t* buf, uint16_t len) {
  DMA1_Channel6->CNTR  = len;                     // number of bytes to be transfered
  DMA1_Channel6->MADDR = (uint32_t)buf;           // memory address
  DMA1_Channel6->CFGR |= DMA_CFG6_EN;             // enable DMA channel
  I2C1->CTLR2         |= I2C_CTLR2_DMAEN;         // enable DMA request
}

// ===================================================================================
// SSD1306 128x64 Pixels OLED Functions
// ===================================================================================

uint8_t OLED_buffer[OLED_WIDTH * OLED_HEIGHT / 8];// screen buffer

// OLED initialisation sequence
const uint8_t OLED_INIT_CMD[] = {
  0xA8, 0x3F,                                     // set multiplex ratio  
  0x8D, 0x14,                                     // set DC-DC enable  
  0x20, 0x00,                                     // set horizontal addressing mode
  0x21, 0x00, 0x7F,                               // set start and end column
  0x22, 0x00, 0x3F,                               // set start and end page
  0xDA, 0x12,                                     // set com pins
  0xA1, 0xC8,                                     // flip screen
  0xAF                                            // display on
};

// Init OLED
void OLED_init(void) {
  I2C_init();                                     // initialize I2C first
  I2C_start(OLED_ADDR);                           // start transmission to OLED
  I2C_write(OLED_CMD_MODE);                       // set command mode
  for(uint8_t i=0; i<sizeof(OLED_INIT_CMD); i++)
    I2C_write(OLED_INIT_CMD[i]);                  // send the command bytes
  I2C_stop();                                     // stop transmission

  I2C_start(OLED_ADDR);                           // start transmission to OLED
  I2C_write(OLED_DAT_MODE);                       // set command mode
  I2C_writeBuffer(OLED_buffer, sizeof(OLED_buffer)); // send screen buffer using DMA
}

// ===================================================================================
// Graphics Functions
// ===================================================================================

void OLED_clear(void) {
  uint32_t* ptr = (uint32_t*)&OLED_buffer;
  uint32_t  cnt = sizeof(OLED_buffer) >> 2;
  while(cnt--) *ptr++ = (uint32_t)0;
}

uint8_t OLED_getPixel(int16_t xpos, int16_t ypos) {
  if((xpos < 0) || (xpos >= OLED_WIDTH) || (ypos < 0) || (ypos >= OLED_HEIGHT)) return 0;
  return((OLED_buffer[((uint16_t)ypos >> 3) * 128 + xpos] >> (ypos & 7)) & 1);
}

void OLED_setPixel(int16_t xpos, int16_t ypos, uint8_t color) {
  if((xpos < 0) || (xpos >= OLED_WIDTH) || (ypos < 0) || (ypos >= OLED_HEIGHT)) return;
  if(color) OLED_buffer[((uint16_t)ypos >> 3) * 128 + xpos] |=  ((uint8_t)1 << (ypos & 7));
  else      OLED_buffer[((uint16_t)ypos >> 3) * 128 + xpos] &= ~((uint8_t)1 << (ypos & 7));
}

void OLED_drawVLine(int16_t x, int16_t y, int16_t h, uint8_t color) {
  for(int16_t i=y; i<y+h; i++) OLED_setPixel(x, i, color);
}

void OLED_drawHLine(int16_t x, int16_t y, int16_t w, uint8_t color) {
  for(int16_t i=x; i<x+w; i++) OLED_setPixel(i, y, color);
}

void OLED_drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
  uint8_t steep = abs(y1 - y0) > abs(x1 - x0);
  if(steep) {
    swap(x0, y0);
    swap(x1, y1);
  }

  if(x0 > x1) {
    swap(x0, x1);
    swap(y0, y1);
  }

  int16_t dx, dy;
  dx = x1 - x0;
  dy = abs(y1 - y0);

  int16_t err = dx / 2;
  int16_t ystep;

  if(y0 < y1) ystep = 1;
  else ystep = -1;

  for(; x0<=x1; x0++) {
    if(steep) OLED_setPixel(y0, x0, color);
    else      OLED_setPixel(x0, y0, color);
    err -= dy;
    if(err < 0) {
      y0  += ystep;
      err += dx;
    }
  }
}

void OLED_drawCircle(int16_t x0, int16_t y0, int16_t r, uint8_t color) {
  int16_t f = 1 - r;
  int16_t ddF_x = 1;
  int16_t ddF_y = -2 * r;
  int16_t x = 0;
  int16_t y = r;

  OLED_setPixel(x0  , y0+r, color);
  OLED_setPixel(x0  , y0-r, color);
  OLED_setPixel(x0+r, y0  , color);
  OLED_setPixel(x0-r, y0  , color);

  while(x < y) {
    if(f >= 0) {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;

    OLED_setPixel(x0 + x, y0 + y, color);
    OLED_setPixel(x0 - x, y0 + y, color);
    OLED_setPixel(x0 + x, y0 - y, color);
    OLED_setPixel(x0 - x, y0 - y, color);
    OLED_setPixel(x0 + y, y0 + x, color);
    OLED_setPixel(x0 - y, y0 + x, color);
    OLED_setPixel(x0 + y, y0 - x, color);
    OLED_setPixel(x0 - y, y0 - x, color);
  }
}

void OLED_drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
  OLED_drawHLine(x    , y,     w, color);
  OLED_drawHLine(x    , y+h-1, w, color);
  OLED_drawVLine(x    , y,     h, color);
  OLED_drawVLine(x+w-1, y,     h, color);
}

void OLED_fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
  for(int16_t i=x; i<x+w; i++) OLED_drawVLine(i, y, h, color);
}
