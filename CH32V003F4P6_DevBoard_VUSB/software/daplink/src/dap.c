// ===================================================================================
// CMSIS-DAP Functions (SWD only) for CH32V003                                * v1.0 *
// ===================================================================================
//
// Based on free-dap by Alex Taradov (https://github.com/ataradov/free-dap):
//
// Copyright (c) 2016, Alex Taradov <alex@taradov.com>
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
// 3. The name of the author may not be used to endorse or promote products
//    derived from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.
//
// Changes 2026 by Stefan Wagner: adapted to CH32V003, SWD only, retry counts as
// specified by CMSIS-DAP, DAP_SWD_Sequence, DAP_HostStatus LEDs, response lengths.

#include "dap.h"
#include "system.h"
#include "gpio.h"

// ===================================================================================
// Definitions
// ===================================================================================

// DAP info IDs
#define DAP_INFO_FW_VER           0x04
#define DAP_INFO_CAPABILITIES     0xF0
#define DAP_INFO_PACKET_COUNT     0xFE
#define DAP_INFO_PACKET_SIZE      0xFF

// DAP status
#define DAP_OK                    0x00
#define DAP_ERROR                 0xFF

// DAP ports
#define DAP_PORT_DISABLED         0
#define DAP_PORT_AUTODETECT       0
#define DAP_PORT_SWD              1

// DAP transfer request bits
#define DAP_TRANSFER_APnDP        (1 << 0)
#define DAP_TRANSFER_RnW          (1 << 1)
#define DAP_TRANSFER_A2           (1 << 2)
#define DAP_TRANSFER_A3           (1 << 3)
#define DAP_TRANSFER_MATCH_VALUE  (1 << 4)
#define DAP_TRANSFER_MATCH_MASK   (1 << 5)

// DAP transfer response bits
#define DAP_TRANSFER_INVALID      0
#define DAP_TRANSFER_OK           (1 << 0)
#define DAP_TRANSFER_WAIT         (1 << 1)
#define DAP_TRANSFER_FAULT        (1 << 2)
#define DAP_TRANSFER_ERROR        (1 << 3)
#define DAP_TRANSFER_MISMATCH     (1 << 4)

// DAP SWJ pins
#define DAP_SWJ_SWCLK_TCK         (1 << 0)
#define DAP_SWJ_SWDIO_TMS         (1 << 1)
#define DAP_SWJ_nRESET            (1 << 7)

// SWD sequence info
#define SWD_SEQUENCE_CLK          0x3F
#define SWD_SEQUENCE_DIN          0x80

// SWD DP registers
#define SWD_DP_W_ABORT            0x00
#define SWD_DP_R_RDBUFF           0x0C

// Number of system ticks needed by the code for half a SWD clock period
#define DAP_CLOCK_OVERHEAD        30        // clock cycles per half period without delay

// ===================================================================================
// Pin Macros
// ===================================================================================
#define SWCLK_HIGH()        PIN_high(PIN_SWCLK)
#define SWCLK_LOW()         PIN_low(PIN_SWCLK)
#define SWDIO_WRITE(b)      PIN_write(PIN_SWDIO, (b))
#define SWDIO_READ()        PIN_read(PIN_SWDIO)
#define SWDIO_OUT()         PIN_output(PIN_SWDIO)
#define SWDIO_IN()          PIN_input_PU(PIN_SWDIO)
#define NRST_WRITE(b)       PIN_write(PIN_NRST, (b))
#define NRST_READ()         PIN_read(PIN_NRST)

#ifdef PIN_LED_CON
  #define LED_CON_init()    {PIN_high(PIN_LED_CON); PIN_output(PIN_LED_CON);}
  #define LED_CON_write(b)  PIN_write(PIN_LED_CON, !(b))
#else
  #define LED_CON_init()
  #define LED_CON_write(b)
#endif

#ifdef PIN_LED_RUN
  #define LED_RUN_init()    {PIN_high(PIN_LED_RUN); PIN_output(PIN_LED_RUN);}
  #define LED_RUN_write(b)  PIN_write(PIN_LED_RUN, !(b))
#else
  #define LED_RUN_init()
  #define LED_RUN_write(b)
#endif

// ===================================================================================
// Variables
// ===================================================================================
volatile uint8_t DAP_abort;               // abort current transfer
static uint8_t  dap_port;                 // current port (disabled or SWD)
static uint8_t  dap_idle_cycles;          // idle cycles after transfer
static uint16_t dap_retry_count;          // number of retries after WAIT response
static uint16_t dap_match_retry_count;    // number of retries for value match
static uint32_t dap_match_mask;           // mask for value match
static uint8_t  dap_swd_turnaround;       // turnaround period in clock cycles
static uint8_t  dap_swd_data_phase;       // always generate data phase
static uint32_t dap_clock_delay;          // half clock period in ticks (0: fast)

// ===================================================================================
// SWD Low-Level Functions
// ===================================================================================

// Wait half a clock period
static inline void dap_delay(void) {
  if(dap_clock_delay) {
    uint32_t start = STK->CNT;
    while((STK->CNT - start) < dap_clock_delay);
  }
}

// Generate clock cycles
static void dap_swd_clock(uint32_t cycles) {
  while(cycles--) {
    SWCLK_LOW();
    dap_delay();
    SWCLK_HIGH();
    dap_delay();
  }
}

// Write bits (LSB first)
static void dap_swd_write(uint32_t value, uint8_t size) {
  while(size--) {
    SWDIO_WRITE(value & 1);
    SWCLK_LOW();
    dap_delay();
    SWCLK_HIGH();
    dap_delay();
    value >>= 1;
  }
}

// Read bits (LSB first)
static uint32_t dap_swd_read(uint8_t size) {
  uint32_t value = 0;
  uint8_t  i;
  for(i=0; i<size; i++) {
    SWCLK_LOW();
    dap_delay();
    value |= (uint32_t)SWDIO_READ() << i;
    SWCLK_HIGH();
    dap_delay();
  }
  return value;
}

// Set SWD clock frequency
static void dap_setup_clock(uint32_t freq) {
  uint32_t ticks;
  if(!freq) freq = DAP_DEFAULT_CLOCK;
  ticks = F_CPU / 2 / freq;
  dap_clock_delay = (ticks > DAP_CLOCK_OVERHEAD) ? (ticks - DAP_CLOCK_OVERHEAD) : 0;
}

// Calculate parity of a 32-bit value
static inline uint32_t dap_parity(uint32_t value) {
  value ^= value >> 16;
  value ^= value >> 8;
  value ^= value >> 4;
  value &= 0x0F;
  return (0x6996 >> value) & 1;
}

// One SWD operation, returns ACK
static uint8_t dap_swd_operation(uint8_t req, uint32_t *data) {
  uint32_t value;
  uint8_t  ack;

  // Request: start, APnDP, RnW, A2, A3, parity, stop, park
  dap_swd_write(0x81 | (dap_parity(req) << 5) | (req << 1), 8);

  // Turnaround and acknowledge
  SWDIO_IN();
  dap_swd_clock(dap_swd_turnaround);
  ack = dap_swd_read(3);

  if(ack == DAP_TRANSFER_OK) {
    if(req & DAP_TRANSFER_RnW) {
      // Read data and parity, turnaround
      value = dap_swd_read(32);
      if(dap_parity(value) != dap_swd_read(1)) ack = DAP_TRANSFER_ERROR;
      if(data) *data = value;
      dap_swd_clock(dap_swd_turnaround);
      SWDIO_OUT();
    }
    else {
      // Turnaround, write data and parity
      dap_swd_clock(dap_swd_turnaround);
      SWDIO_OUT();
      dap_swd_write(*data, 32);
      dap_swd_write(dap_parity(*data), 1);
    }
    // Idle cycles
    SWDIO_WRITE(0);
    dap_swd_clock(dap_idle_cycles);
  }

  else if((ack == DAP_TRANSFER_WAIT) || (ack == DAP_TRANSFER_FAULT)) {
    if(dap_swd_data_phase && (req & DAP_TRANSFER_RnW)) dap_swd_clock(32 + 1);
    dap_swd_clock(dap_swd_turnaround);
    SWDIO_OUT();
    if(dap_swd_data_phase && !(req & DAP_TRANSFER_RnW)) {
      SWDIO_WRITE(0);
      dap_swd_clock(32 + 1);
    }
  }

  else {
    // Protocol error: back off data phase
    dap_swd_clock(dap_swd_turnaround + 32 + 1);
    SWDIO_OUT();
  }

  SWDIO_WRITE(1);
  return ack;
}

// One SWD transfer with retries on WAIT response
static uint8_t dap_swd_transfer_word(uint8_t req, uint32_t *data) {
  uint8_t  ack;
  uint16_t retry = dap_retry_count;
  req &= (DAP_TRANSFER_APnDP | DAP_TRANSFER_RnW | DAP_TRANSFER_A2 | DAP_TRANSFER_A3);
  do {
    ack = dap_swd_operation(req, data);
  } while((ack == DAP_TRANSFER_WAIT) && retry-- && !DAP_abort);
  return ack;
}

// ===================================================================================
// Helper Functions
// ===================================================================================
static inline uint32_t get32(const uint8_t *p) {
  return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static inline void put32(uint8_t *p, uint32_t v) {
  p[0] = v; p[1] = v >> 8; p[2] = v >> 16; p[3] = v >> 24;
}

// Wait microseconds
static void dap_delay_us(uint32_t us) {
  while(us--) DLY_us(1);
}

// ===================================================================================
// DAP Commands (each returns the number of response bytes after the command ID)
// ===================================================================================

// DAP_Info
static uint8_t DAP_Info(const uint8_t *req, uint8_t *res) {
  uint8_t i;
  switch(req[0]) {
    case DAP_INFO_FW_VER:
      for(i=0; DAP_FW_VERSION[i]; i++) res[i + 1] = DAP_FW_VERSION[i];
      res[i + 1] = 0;
      res[0] = i + 1;
      return i + 2;
    case DAP_INFO_CAPABILITIES:
      res[0] = 1;
      res[1] = DAP_PORT_SWD;                // SWD only
      return 2;
    case DAP_INFO_PACKET_COUNT:
      res[0] = 1;
      res[1] = DAP_PACKET_COUNT;
      return 2;
    case DAP_INFO_PACKET_SIZE:
      res[0] = 2;
      res[1] = DAP_PACKET_SIZE & 0xFF;
      res[2] = DAP_PACKET_SIZE >> 8;
      return 3;
    default:
      res[0] = 0;                           // information not available
      return 1;
  }
}

// DAP_HostStatus (status LEDs)
static uint8_t DAP_HostStatus(const uint8_t *req, uint8_t *res) {
  if(req[0] == 0)      { LED_CON_write(req[1] & 1); }
  else if(req[0] == 1) { LED_RUN_write(req[1] & 1); }
  res[0] = DAP_OK;
  return 1;
}

// DAP_Connect
static uint8_t DAP_Connect(const uint8_t *req, uint8_t *res) {
  uint8_t port = req[0];
  if(port == DAP_PORT_AUTODETECT) port = DAP_PORT_SWD;
  if(port == DAP_PORT_SWD) {
    SWDIO_WRITE(1);                         // SWDIO output high
    SWDIO_OUT();
    SWCLK_HIGH();                           // SWCLK output high
    PIN_output(PIN_SWCLK);
    NRST_WRITE(1);                          // nRESET released (open-drain)
    PIN_output_OD(PIN_NRST);
    LED_CON_write(1);
    dap_port = DAP_PORT_SWD;
  }
  else dap_port = DAP_PORT_DISABLED;        // JTAG is not supported
  res[0] = dap_port;
  return 1;
}

// DAP_Disconnect (all pins to high impedance)
static uint8_t DAP_Disconnect(uint8_t *res) {
  PIN_input(PIN_SWCLK);
  PIN_input_PU(PIN_SWDIO);
  PIN_input(PIN_NRST);
  LED_CON_write(0);
  LED_RUN_write(0);
  dap_port = DAP_PORT_DISABLED;
  res[0] = DAP_OK;
  return 1;
}

// DAP_TransferConfigure
static uint8_t DAP_TransferConfigure(const uint8_t *req, uint8_t *res) {
  dap_idle_cycles       = req[0];
  dap_retry_count       = (uint16_t)req[1] | ((uint16_t)req[2] << 8);
  dap_match_retry_count = (uint16_t)req[3] | ((uint16_t)req[4] << 8);
  res[0] = DAP_OK;
  return 1;
}

// DAP_Transfer
static uint8_t DAP_Transfer(const uint8_t *req, uint8_t *res) {
  uint8_t  req_count, resp_count = 0, request, ack = DAP_TRANSFER_INVALID;
  uint8_t  posted_read = 0, verify_write = 0;
  const uint8_t *req_data = &req[2];
  uint8_t  *resp_data = &res[2];
  uint8_t  *resp_end  = res + DAP_PACKET_SIZE - 1;
  uint32_t data = 0, match_value;
  uint16_t retry;

  req_count = req[1];                       // req[0]: DAP index (ignored)
  if(dap_port != DAP_PORT_SWD) req_count = 0;

  while(req_count && !DAP_abort) {
    verify_write = 0;
    request = *req_data++;

    // Get result of a posted read
    if(posted_read) {
      if((request & DAP_TRANSFER_APnDP) && (request & DAP_TRANSFER_RnW) && !(request & DAP_TRANSFER_MATCH_VALUE)) {
        ack = dap_swd_transfer_word(request, &data);  // read previous data, post next read
      }
      else {
        ack = dap_swd_transfer_word(SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW, &data);
        posted_read = 0;
      }
      if(ack != DAP_TRANSFER_OK) break;
      if(resp_data + 4 > resp_end) { ack = DAP_TRANSFER_ERROR; break; }   // response full
      put32(resp_data, data);
      resp_data += 4;
    }

    if(request & DAP_TRANSFER_RnW) {
      if(request & DAP_TRANSFER_MATCH_VALUE) {
        // Read register until its value matches
        match_value = get32(req_data);
        req_data += 4;
        if(request & DAP_TRANSFER_APnDP) {
          ack = dap_swd_transfer_word(request, 0);    // post AP read
          if(ack != DAP_TRANSFER_OK) break;
        }
        retry = dap_match_retry_count;
        do {
          ack = dap_swd_transfer_word(request, &data);
        } while((ack == DAP_TRANSFER_OK) && ((data & dap_match_mask) != match_value) && retry-- && !DAP_abort);
        if((data & dap_match_mask) != match_value) ack |= DAP_TRANSFER_MISMATCH;
        if(ack != DAP_TRANSFER_OK) break;
      }
      else if(request & DAP_TRANSFER_APnDP) {
        // Read AP register (posted read)
        if(resp_data + 4 > resp_end) { ack = DAP_TRANSFER_ERROR; break; }   // response full
        if(!posted_read) {
          ack = dap_swd_transfer_word(request, 0);
          if(ack != DAP_TRANSFER_OK) break;
          posted_read = 1;
        }
      }
      else {
        // Read DP register
        if(resp_data + 4 > resp_end) { ack = DAP_TRANSFER_ERROR; break; } // response full
        ack = dap_swd_transfer_word(request, &data);
        if(ack != DAP_TRANSFER_OK) break;
        put32(resp_data, data);
        resp_data += 4;
      }
    }
    else {
      // Write register
      data = get32(req_data);
      req_data += 4;
      if(request & DAP_TRANSFER_MATCH_MASK) {
        ack = DAP_TRANSFER_OK;
        dap_match_mask = data;
      }
      else {
        ack = dap_swd_transfer_word(request, &data);
        if(ack != DAP_TRANSFER_OK) break;
        verify_write = 1;
      }
    }
    req_count--;
    resp_count++;
  }

  // Get result of last posted read or check last write
  if(ack == DAP_TRANSFER_OK) {
    if(posted_read) {
      ack = dap_swd_transfer_word(SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW, &data);
      put32(resp_data, data);
      resp_data += 4;
    }
    else if(verify_write) {
      ack = dap_swd_transfer_word(SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW, 0);
    }
  }

  res[0] = resp_count;
  res[1] = ack;
  return resp_data - res;
}

// DAP_TransferBlock
static uint8_t DAP_TransferBlock(const uint8_t *req, uint8_t *res) {
  uint16_t req_count, resp_count = 0, i, transfers;
  uint8_t  request, ack = DAP_TRANSFER_INVALID;
  const uint8_t *req_data = &req[4];
  uint8_t  *resp_data = &res[3];
  uint32_t data;

  req_count = (uint16_t)req[1] | ((uint16_t)req[2] << 8);   // req[0]: DAP index
  request   = req[3];

  if((dap_port == DAP_PORT_SWD) && req_count) {
    if(request & DAP_TRANSFER_RnW) {
      // Read register block (AP reads are posted)
      transfers = (request & DAP_TRANSFER_APnDP) ? (req_count + 1) : req_count;
      for(i=0; i<transfers; i++) {
        if(i == req_count) request = SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW;
        ack = dap_swd_transfer_word(request, &data);
        if(ack != DAP_TRANSFER_OK) break;
        if((i == 0) && (request & DAP_TRANSFER_APnDP)) continue;
        if(resp_data + 4 > res + DAP_PACKET_SIZE - 1) break;   // response full
        put32(resp_data, data);
        resp_data += 4;
        resp_count++;
      }
    }
    else {
      // Write register block
      for(i=0; i<req_count; i++) {
        data = get32(req_data);
        req_data += 4;
        ack = dap_swd_transfer_word(request, &data);
        if(ack != DAP_TRANSFER_OK) break;
        resp_count++;
      }
      if(ack == DAP_TRANSFER_OK) ack = dap_swd_transfer_word(SWD_DP_R_RDBUFF | DAP_TRANSFER_RnW, 0);
    }
  }

  res[0] = resp_count;
  res[1] = resp_count >> 8;
  res[2] = ack;
  return resp_data - res;
}

// DAP_WriteABORT
static uint8_t DAP_WriteABORT(const uint8_t *req, uint8_t *res) {
  uint32_t data = get32(&req[1]);         // req[0]: DAP index
  if(dap_port == DAP_PORT_SWD) {
    dap_swd_transfer_word(SWD_DP_W_ABORT, &data);
    res[0] = DAP_OK;
  }
  else res[0] = DAP_ERROR;
  return 1;
}

// DAP_Delay
static uint8_t DAP_Delay(const uint8_t *req, uint8_t *res) {
  dap_delay_us((uint16_t)req[0] | ((uint16_t)req[1] << 8));
  res[0] = DAP_OK;
  return 1;
}

// DAP_ResetTarget (no device specific reset sequence implemented)
static uint8_t DAP_ResetTarget(uint8_t *res) {
  res[0] = DAP_OK;
  res[1] = 0;
  return 2;
}

// DAP_SWJ_Pins
static uint8_t DAP_SWJ_Pins(const uint8_t *req, uint8_t *res) {
  uint8_t  value  = req[0];
  uint8_t  select = req[1];
  uint32_t wait   = get32(&req[2]);       // timeout in microseconds
  uint32_t start;

  if(select & DAP_SWJ_SWCLK_TCK) { if(value & DAP_SWJ_SWCLK_TCK) SWCLK_HIGH(); else SWCLK_LOW(); }
  if(select & DAP_SWJ_SWDIO_TMS) SWDIO_WRITE((value & DAP_SWJ_SWDIO_TMS) != 0);
  if(select & DAP_SWJ_nRESET)    NRST_WRITE((value & DAP_SWJ_nRESET) != 0);

  // Wait until the selected pins have the requested values or timeout
  if(wait) {
    if(wait > 3000000) wait = 3000000;
    start = STK->CNT;
    while((STK->CNT - start) < wait * DLY_US_TIME) {
      if((select & DAP_SWJ_SWCLK_TCK) && (((value & DAP_SWJ_SWCLK_TCK) != 0) != PIN_read(PIN_SWCLK))) continue;
      if((select & DAP_SWJ_SWDIO_TMS) && (((value & DAP_SWJ_SWDIO_TMS) != 0) != SWDIO_READ())) continue;
      if((select & DAP_SWJ_nRESET)    && (((value & DAP_SWJ_nRESET)    != 0) != NRST_READ()))  continue;
      break;
    }
  }

  res[0] = (PIN_read(PIN_SWCLK) ? DAP_SWJ_SWCLK_TCK : 0)
         | (SWDIO_READ()        ? DAP_SWJ_SWDIO_TMS : 0)
         | (NRST_READ()         ? DAP_SWJ_nRESET    : 0);
  return 1;
}

// DAP_SWJ_Clock
static uint8_t DAP_SWJ_Clock(const uint8_t *req, uint8_t *res) {
  dap_setup_clock(get32(req));
  res[0] = DAP_OK;
  return 1;
}

// DAP_SWJ_Sequence
static uint8_t DAP_SWJ_Sequence(const uint8_t *req, uint8_t *res) {
  uint16_t size = req[0] ? req[0] : 256;  // number of bits (0: 256)
  const uint8_t *data = &req[1];
  uint8_t  sz;
  while(size) {
    sz = (size > 8) ? 8 : size;
    dap_swd_write(*data++, sz);
    size -= sz;
  }
  res[0] = DAP_OK;
  return 1;
}

// DAP_SWD_Configure
static uint8_t DAP_SWD_Configure(const uint8_t *req, uint8_t *res) {
  dap_swd_turnaround = (req[0] & 3) + 1;
  dap_swd_data_phase = (req[0] & 4) ? 1 : 0;
  res[0] = DAP_OK;
  return 1;
}

// DAP_SWD_Sequence
static uint8_t DAP_SWD_Sequence(const uint8_t *req, uint8_t *res) {
  uint8_t sequence_count = *req++;
  uint8_t info, count, bits;
  uint8_t *resp_data = &res[1];
  uint32_t value;

  res[0] = DAP_OK;
  while(sequence_count--) {
    info  = *req++;
    count = info & SWD_SEQUENCE_CLK;
    if(!count) count = 64;
    if(info & SWD_SEQUENCE_DIN) {
      SWDIO_IN();                           // capture SWDIO
      while(count) {
        bits  = (count > 8) ? 8 : count;
        value = dap_swd_read(bits);
        *resp_data++ = value;
        count -= bits;
      }
    }
    else {
      SWDIO_OUT();                          // output SWDIO
      while(count) {
        bits = (count > 8) ? 8 : count;
        dap_swd_write(*req++, bits);
        count -= bits;
      }
    }
  }
  SWDIO_OUT();                              // leave SWDIO as output
  return resp_data - res;
}

// ===================================================================================
// DAP Main Functions
// ===================================================================================

// Init DAP
void DAP_init(void) {
  LED_CON_init();
  LED_RUN_init();
  dap_port              = DAP_PORT_DISABLED;
  DAP_abort             = 0;
  dap_match_mask        = 0;
  dap_idle_cycles       = 0;
  dap_retry_count       = 100;
  dap_match_retry_count = 100;
  dap_swd_turnaround    = 1;
  dap_swd_data_phase    = 0;
  dap_setup_clock(DAP_DEFAULT_CLOCK);
  PIN_input(PIN_SWCLK);                   // all pins to high impedance
  PIN_input_PU(PIN_SWDIO);
  PIN_input(PIN_NRST);
}

// Process DAP request, returns length of response (incl. command ID)
uint8_t DAP_process(const uint8_t *req, uint8_t *res) {
  uint8_t len;
  uint8_t cmd = *req++;                     // command ID
  uint8_t *rsp = res + 1;                   // response data follows command ID

  DAP_abort = 0;
  res[0] = cmd;

  switch(cmd) {
    case ID_DAP_Info:               len = DAP_Info(req, rsp);               break;
    case ID_DAP_HostStatus:         len = DAP_HostStatus(req, rsp);         break;
    case ID_DAP_Connect:            len = DAP_Connect(req, rsp);            break;
    case ID_DAP_Disconnect:         len = DAP_Disconnect(rsp);              break;
    case ID_DAP_TransferConfigure:  len = DAP_TransferConfigure(req, rsp);  break;
    case ID_DAP_Transfer:           len = DAP_Transfer(req, rsp);           break;
    case ID_DAP_TransferBlock:      len = DAP_TransferBlock(req, rsp);      break;
    case ID_DAP_TransferAbort:      rsp[0] = DAP_OK; len = 1;                break;
    case ID_DAP_WriteABORT:         len = DAP_WriteABORT(req, rsp);         break;
    case ID_DAP_Delay:              len = DAP_Delay(req, rsp);              break;
    case ID_DAP_ResetTarget:        len = DAP_ResetTarget(rsp);             break;
    case ID_DAP_SWJ_Pins:           len = DAP_SWJ_Pins(req, rsp);           break;
    case ID_DAP_SWJ_Clock:          len = DAP_SWJ_Clock(req, rsp);          break;
    case ID_DAP_SWJ_Sequence:       len = DAP_SWJ_Sequence(req, rsp);       break;
    case ID_DAP_SWD_Configure:      len = DAP_SWD_Configure(req, rsp);      break;
    case ID_DAP_SWD_Sequence:       len = DAP_SWD_Sequence(req, rsp);       break;
    default:
      res[0] = ID_DAP_Invalid;              // unsupported command
      len = 0;
      break;
  }
  return len + 1;
}
