// ===================================================================================
// USART Driver with DMA Ring Buffers for CH32X035                            * v1.0 *
// ===================================================================================
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "usart_dma.h"

// ===================================================================================
// DMA Channels and Interrupt Dispatch
// ===================================================================================
#define DMA_CH(n)       ((DMA_Channel_TypeDef*)(DMA1_Channel1_BASE + 0x14 * ((n) - 1)))
#define DMA_TCIF(n)     ((uint32_t)0x2 << (((n) - 1) << 2))
#define DMA_GIF(n)      ((uint32_t)0xF << (((n) - 1) << 2))

static USARTX_t* USARTX_owner[9];       // instance using DMA channel n

// DMA channel interrupt handler (transfer complete)
static void USARTX_dmaIRQ(uint8_t ch) {
  USARTX_t* u = USARTX_owner[ch];
  DMA1->INTFCR = DMA_GIF(ch);           // clear flags
  if(!u) return;
  if(ch == u->rxch) {
    u->rxwraps++;                       // RX buffer cycle completed
    return;
  }
  // TX transfer completed: start next block if pending
  u->txtail  += u->txnum;
  u->txnum    = 0;
  u->txactive = 0;
  if(u->txenabled && (u->txhead != u->txtail)) {
    uint32_t idx = u->txtail & (u->txsize - 1);
    uint32_t cnt = u->txhead - u->txtail;
    if(cnt > (uint32_t)(u->txsize - idx)) cnt = u->txsize - idx;
    u->txnum    = cnt;
    u->txactive = 1;
    u->txdma->CFGR &= ~DMA_CFGR1_EN;
    u->txdma->MADDR = (uint32_t)&u->txbuf[idx];
    u->txdma->CNTR  = cnt;
    u->txdma->CFGR |=  DMA_CFGR1_EN;
  }
}

#define USARTX_DMA_HANDLER(n) \
  void DMA1_Channel##n##_IRQHandler(void) __attribute__((interrupt)); \
  void DMA1_Channel##n##_IRQHandler(void) { USARTX_dmaIRQ(n); }

USARTX_DMA_HANDLER(1)
USARTX_DMA_HANDLER(2)
USARTX_DMA_HANDLER(3)
USARTX_DMA_HANDLER(4)
USARTX_DMA_HANDLER(5)
USARTX_DMA_HANDLER(6)
USARTX_DMA_HANDLER(7)
USARTX_DMA_HANDLER(8)

static const IRQn_Type USARTX_dmaIRQn[9] = {
  0, DMA1_Channel1_IRQn, DMA1_Channel2_IRQn, DMA1_Channel3_IRQn, DMA1_Channel4_IRQn,
  DMA1_Channel5_IRQn, DMA1_Channel6_IRQn, DMA1_Channel7_IRQn, DMA1_Channel8_IRQn
};

// ===================================================================================
// Setup and Configuration
// ===================================================================================

// Setup instance for USART n (1..4) with pin remap value (pins are set by caller)
void USARTX_setup(USARTX_t* u, uint8_t n, uint8_t remap) {
  uint32_t shift, mask;
  RCC->AHBPCENR  |= RCC_DMA1EN;
  RCC->APB2PCENR |= RCC_AFIOEN;
  switch(n) {
    case 1:  u->usart = USART1; u->rxch = 5; u->txch = 4; shift = 5;  mask = 3;
             RCC->APB2PCENR |= RCC_USART1EN; break;
    case 2:  u->usart = USART2; u->rxch = 6; u->txch = 7; shift = 7;  mask = 7;
             RCC->APB1PCENR |= RCC_USART2EN; break;
    case 3:  u->usart = USART3; u->rxch = 3; u->txch = 2; shift = 10; mask = 3;
             RCC->APB1PCENR |= RCC_USART3EN; break;
    default: u->usart = USART4; u->rxch = 8; u->txch = 1; shift = 12; mask = 7;
             RCC->APB1PCENR |= RCC_USART4EN; break;
  }
  AFIO->PCFR1 = (AFIO->PCFR1 & ~(mask << shift)) | (((uint32_t)remap & mask) << shift);

  u->rxdma = DMA_CH(u->rxch);
  u->txdma = DMA_CH(u->txch);
  u->rxwraps = 0; u->rxtail = 0; u->rxstop = 0; u->txhead = 0; u->txtail = 0;
  u->txnum = 0; u->txactive = 0; u->rxenabled = 0; u->txenabled = 0; u->errors = 0;
  u->baudrate = 0;

  // RX DMA: peripheral -> memory, circular, transfer complete interrupt
  USARTX_owner[u->rxch] = u;
  u->rxdma->CFGR  = 0;
  u->rxdma->PADDR = (uint32_t)&u->usart->DATAR;
  NVIC_EnableIRQ(USARTX_dmaIRQn[u->rxch]);

  // TX DMA: memory -> peripheral, single block, transfer complete interrupt
  if(u->txbuf) {
    USARTX_owner[u->txch] = u;
    u->txdma->CFGR  = 0;
    u->txdma->PADDR = (uint32_t)&u->usart->DATAR;
    NVIC_EnableIRQ(USARTX_dmaIRQn[u->txch]);
  }

  u->usart->CTLR3 = (u->txbuf ? USART_CTLR3_DMAT : 0) | USART_CTLR3_DMAR;
}

// Configure baudrate and frame format
// databits: 7 or 8, parity: 0=none, 1=even, 2=odd, stopbits: 1 or 2
uint8_t USARTX_config(USARTX_t* u, uint32_t baud, uint8_t databits, uint8_t parity,
                      uint8_t stopbits) {
  uint8_t  err = USARTX_OK;
  uint16_t ctlr1 = USART_CTLR1_UE;
  uint32_t brr;

  // Check parameters
  if(parity > 2) err |= USARTX_CFG_PARITY;
  if((stopbits != 1) && (stopbits != 2)) err |= USARTX_CFG_STOPBITS;
  if(databits == 8) {
    if(parity) ctlr1 |= USART_CTLR1_M;              // 9-bit word incl. parity
  }
  else if(databits == 7) {
    if(!parity) err |= USARTX_CFG_DATABITS;         // 7N not supported
  }
  else err |= USARTX_CFG_DATABITS;
  if(parity) ctlr1 |= USART_CTLR1_PCE | ((parity == 2) ? USART_CTLR1_PS : 0);
  if(!baud) err |= USARTX_CFG_BAUDRATE;
  else {
    brr = (F_CPU + (baud >> 1)) / baud;             // 16x oversampling
    if((brr < 16) || (brr > 0xFFFF)) err |= USARTX_CFG_BAUDRATE;
  }
  if(err) return err;

  // Configure USART (keep enabled directions)
  if(u->rxenabled) ctlr1 |= USART_CTLR1_RE;
  if(u->txenabled) ctlr1 |= USART_CTLR1_TE;
  u->usart->CTLR1 = 0;
  u->usart->BRR   = brr;
  u->usart->CTLR2 = (stopbits == 2) ? 0x2000 : 0;
  u->usart->CTLR1 = ctlr1;
  u->baudrate = F_CPU / brr;
  return USARTX_OK;
}

// ===================================================================================
// Receive Functions
// ===================================================================================

// Start receiving (buffer is cleared)
void USARTX_rxEnable(USARTX_t* u) {
  u->rxdma->CFGR &= ~DMA_CFGR1_EN;
  DMA1->INTFCR    = DMA_GIF(u->rxch);
  u->rxwraps      = 0;
  u->rxtail       = 0;
  u->rxstop       = 0;
  u->errors       = 0;
  u->rxdma->MADDR = (uint32_t)u->rxbuf;
  u->rxdma->CNTR  = u->rxsize;
  u->rxdma->CFGR  = DMA_CFGR1_MINC | DMA_CFGR1_CIRC | DMA_CFGR1_TCIE | DMA_CFGR1_EN;
  (void)u->usart->STATR;                            // clear error flags
  (void)u->usart->DATAR;
  u->usart->CTLR1 |= USART_CTLR1_RE;
  u->rxenabled = 1;
}

// Stop receiving (received data stays readable)
void USARTX_rxDisable(USARTX_t* u) {
  if(!u->rxenabled) return;
  u->usart->CTLR1 &= ~USART_CTLR1_RE;
  u->rxstop = USARTX_rxHead(u);
  u->rxdma->CFGR  &= ~DMA_CFGR1_EN;
  u->rxenabled = 0;
}

// Discard received data
void USARTX_rxFlush(USARTX_t* u) {
  u->rxtail = USARTX_rxHead(u);
}

// Absolute number of received bytes
uint32_t USARTX_rxHead(USARTX_t* u) {
  uint32_t w, c;
  if(!u->rxenabled) return u->rxstop;
  __disable_irq();
  w = u->rxwraps;
  c = u->rxdma->CNTR;
  if(DMA1->INTFR & DMA_TCIF(u->rxch)) {             // wrap not yet counted by IRQ
    c = u->rxdma->CNTR;
    w++;
  }
  __enable_irq();
  return w * u->rxsize + (u->rxsize - c);
}

// Number of bytes available in RX buffer (on overrun the oldest data is dropped)
uint32_t USARTX_rxAvailable(USARTX_t* u) {
  uint32_t head = USARTX_rxHead(u);
  uint32_t cnt  = head - u->rxtail;
  uint32_t max  = u->rxsize - 32;                   // margin for concurrent writes
  if(cnt > max) {
    u->errors |= USARTX_ERR_LOST;
    u->rxtail  = head - max;
    cnt = max;
  }
  return cnt;
}

// Get pointer to contiguous received data, returns number of bytes
uint32_t USARTX_rxPeek(USARTX_t* u, uint8_t** ptr) {
  uint32_t cnt = USARTX_rxAvailable(u);
  uint32_t idx = u->rxtail & (u->rxsize - 1);
  if(cnt > (uint32_t)(u->rxsize - idx)) cnt = u->rxsize - idx;
  *ptr = &u->rxbuf[idx];
  return cnt;
}

// Remove n bytes from RX buffer
void USARTX_rxSkip(USARTX_t* u, uint32_t n) {
  u->rxtail += n;
}

// Read up to n bytes from RX buffer
uint32_t USARTX_rxRead(USARTX_t* u, uint8_t* buf, uint32_t n) {
  uint32_t cnt = USARTX_rxAvailable(u);
  uint32_t i;
  if(n > cnt) n = cnt;
  for(i=0; i<n; i++) buf[i] = u->rxbuf[(u->rxtail + i) & (u->rxsize - 1)];
  u->rxtail += n;
  return n;
}

// ===================================================================================
// Transmit Functions
// ===================================================================================

// Start TX DMA transfer if data is pending (interrupts must be disabled)
static void USARTX_txStart(USARTX_t* u) {
  uint32_t idx, cnt;
  if(u->txactive || !u->txenabled || (u->txhead == u->txtail)) return;
  idx = u->txtail & (u->txsize - 1);
  cnt = u->txhead - u->txtail;
  if(cnt > (uint32_t)(u->txsize - idx)) cnt = u->txsize - idx;
  u->txnum    = cnt;
  u->txactive = 1;
  u->txdma->CFGR &= ~DMA_CFGR1_EN;
  DMA1->INTFCR    = DMA_GIF(u->txch);
  u->txdma->MADDR = (uint32_t)&u->txbuf[idx];
  u->txdma->CNTR  = cnt;
  u->txdma->CFGR  = DMA_CFGR1_MINC | DMA_CFGR1_DIR | DMA_CFGR1_TCIE | DMA_CFGR1_EN;
}

// Enable transmitter
void USARTX_txEnable(USARTX_t* u) {
  if(!u->txbuf) return;
  u->usart->CTLR1 |= USART_CTLR1_TE;
  u->txenabled = 1;
  __disable_irq();
  USARTX_txStart(u);
  __enable_irq();
}

// Discard pending TX data
void USARTX_txFlush(USARTX_t* u) {
  if(!u->txbuf) return;
  __disable_irq();
  u->txdma->CFGR &= ~DMA_CFGR1_EN;
  DMA1->INTFCR    = DMA_GIF(u->txch);
  u->txactive = 0;
  u->txnum    = 0;
  u->txtail   = u->txhead;
  __enable_irq();
}

// Disable transmitter (pending data is discarded)
void USARTX_txDisable(USARTX_t* u) {
  USARTX_txFlush(u);
  u->txenabled = 0;
  u->usart->CTLR1 &= ~USART_CTLR1_TE;
}

// Free space in TX buffer
uint32_t USARTX_txFree(USARTX_t* u) {
  if(!u->txbuf) return 0;
  return u->txsize - (u->txhead - u->txtail);
}

// Number of bytes not yet transmitted
uint32_t USARTX_txPending(USARTX_t* u) {
  uint32_t cnt;
  if(!u->txbuf) return 0;
  __disable_irq();
  cnt = u->txhead - u->txtail;
  if(u->txactive) cnt -= u->txnum - u->txdma->CNTR;
  __enable_irq();
  return cnt;
}

// Write up to n bytes into TX buffer and start transmission
uint32_t USARTX_txWrite(USARTX_t* u, const uint8_t* buf, uint32_t n) {
  uint32_t i, free = USARTX_txFree(u);
  if(n > free) n = free;
  for(i=0; i<n; i++) u->txbuf[(u->txhead + i) & (u->txsize - 1)] = buf[i];
  __disable_irq();
  u->txhead += n;
  USARTX_txStart(u);
  __enable_irq();
  return n;
}

// ===================================================================================
// Error Flags
// ===================================================================================

// Get and clear error flags (framing and parity errors are sampled from the status
// register, the flags are cleared by the next DMA read of the data register)
uint8_t USARTX_errors(USARTX_t* u) {
  uint8_t  err;
  uint16_t stat = u->usart->STATR;
  if(stat & USART_STATR_FE)  u->errors |= USARTX_ERR_FRAMING;
  if(stat & USART_STATR_PE)  u->errors |= USARTX_ERR_PARITY;
  if(stat & USART_STATR_ORE) u->errors |= USARTX_ERR_LOST;
  err = u->errors;
  u->errors = 0;
  return err;
}
