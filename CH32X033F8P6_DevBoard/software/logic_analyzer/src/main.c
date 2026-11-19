// ===================================================================================
// Project:   8-Channel Logic Analyzer for CH32X035/X034/X033 (sigrok-pico Protocol)
// Version:   v1.0
// Year:      2026
// Author:    Stefan Wagner
// Github:    https://github.com/wagiminator
// EasyEDA:   https://easyeda.com/wagiminator
// License:   http://creativecommons.org/licenses/by-sa/3.0/
// ===================================================================================
//
// Description:
// ------------
// 8-channel logic analyzer on the CH32X033 development board. The inputs PA0..PA7 are
// sampled by DMA, which is clocked by timer TIM2, and the samples are sent run-length
// encoded via USB CDC. The device speaks the serial protocol of the sigrok-pico
// firmware (the "raspberrypi-pico" driver of libsigrok), so it can be used with
// PulseView, sigrok-cli and the Signal Analyzer web app.
//
// - Captures without trigger of up to BUF_SIZE samples are stored in RAM first and
//   sent afterwards; this allows sample rates up to SAMPLE_RATE_MAX.
// - Longer captures and captures in continuous mode (used by the host for software
//   triggering) are sent while sampling. If the USB cannot keep up, the capture is
//   aborted.
// - Sample rates are 48 MHz divided by an integer (4M, 3M, 2M, 1M, ...), up to
//   SAMPLE_RATE_MAX. Faster rates would need more DMA transfers than the bus can do;
//   if samples get lost anyway, the capture is aborted (checked with the SysTick).
// - A square wave on PC3 (TEST_FREQ) can be used to check the analyzer.
//
// Protocol (host to device, commands end with '\n'):
// --------------------------------------------------
// *            reset: stop everything (no answer)
// +            stop a running capture (no answer), device sends the byte count
// i            identify, answer: "SRPICO,A001D08,02" (0 analog, 8 digital channels)
// R<rate>      set sample rate in Hz, answer '*' if accepted
// L<count>     set number of samples, answer '*'
// D<e><ch>     enable (e=1) or disable (e=0) digital channel ch, answer '*'
// A<e><ch>     analog channel (there are none), answer '*'
// a<ch>        analog scale and offset, answer "25700x0"
// F            start capture of L samples
// C            start continuous capture until '+'
//
// Data (device to host):
// ----------------------
// - More than 4 channels: a sample is sent as bytes with bit 7 set, 7 channels each
//   (D0..D6, then D7). Repeats of the last sample: 48..79 = 1..32 times, 80..127 =
//   (n - 78) * 32 times.
// - Up to 4 channels (D0..D3): 0x80 | (r << 4) | value = r repeats of the last sample
//   (r = 0..7) and then the new value; 48..127 = (n - 47) * 8 repeats.
// - End: "$<number of data bytes>+". Abort (USB or DMA too slow): "!!!" and "$0+".
//
// References:
// -----------
// - WCH Nanjing Qinheng Microelectronics: http://wch.cn
// - sigrok-pico (serial protocol): https://github.com/pico-coder/sigrok-pico
// - libsigrok raspberrypi-pico driver: https://sigrok.org
//
// Compilation Instructions:
// -------------------------
// - Make sure GCC toolchain (gcc-riscv64-unknown-elf, newlib) and Python3 with
//   chprog (via pip) are installed. In addition, Linux requires access rights to
//   the USB bootloader.
// - Press the BOOT button on the MCU board and keep it pressed while connecting it
//   via USB to your PC.
// - Run 'make flash'.
//
// Operating Instructions:
// -----------------------
// - Connect the board via USB to your PC. It is detected as a CDC device (serial port).
//   Connect GND of the board to GND of the circuit, the signals to PA0..PA7. The
//   inputs work with 3.3V and 5V logic.
// - PulseView or sigrok-cli: choose the driver "RaspberryPI PICO" and the serial port
//   of the board (e.g. /dev/ttyACM0 or COM5), then "Scan for devices".
//   sigrok-cli example:
//   sigrok-cli -d raspberrypi-pico:conn=/dev/ttyACM0 -c samplerate=1000000
//              --samples 10000 -O vcd -o capture.vcd
// - Signal Analyzer web app (Chrome, Edge, Opera): choose the source "Pico or CH32X033"
//   and the serial port of the board.
// - The LED lights up while sampling.


// ===================================================================================
// Libraries, Definitions and Macros
// ===================================================================================
#include "config.h"                 // user configurations
#include "system.h"                 // system functions
#include "gpio.h"                   // GPIO functions
#include "usb_cdc.h"                // USB CDC serial functions

// Device identification (sigrok-pico protocol version 2)
#define NUM_A_CHAN          0       // number of analog channels
#define NUM_D_CHAN          8       // number of digital channels
#define ID_STR              "SRPICO,A001D08,02"

// LED (active low)
#define LED_on()            PIN_low(PIN_LED)
#define LED_off()           PIN_high(PIN_LED)

// States
enum { LA_IDLE, LA_BUFFER, LA_STREAM };

// Checks
#if (F_CPU / SAMPLE_RATE_MIN) > 65536
  #error SAMPLE_RATE_MIN too low for the timer
#endif
#if BUF_SIZE > 65535
  #error BUF_SIZE too large for the DMA
#endif

// ===================================================================================
// Sampling Functions (TIM2 update event triggers DMA1 channel 2: GPIOA->INDR to RAM)
// ===================================================================================
uint8_t SMP_buffer[BUF_SIZE] __attribute__((aligned(4)));
volatile uint32_t SMP_laps;         // completed passes through the buffer
volatile uint8_t  SMP_done;         // buffer filled (single pass)
volatile uint32_t SMP_tend;         // SysTick when the buffer was filled (single pass)
uint32_t SMP_div;                   // sample period in clock cycles
uint32_t SMP_t0;                    // SysTick at the start of the check window
uint32_t SMP_w0;                    // samples written at the start of the check window
uint8_t  SMP_running;               // 1: timer and DMA are sampling

// DMA interrupt: end of buffer reached
void DMA1_Channel2_IRQHandler(void) __attribute__((interrupt));
void DMA1_Channel2_IRQHandler(void) {
  SMP_tend = STK->CNTL;             // time of the last sample
  DMA1->INTFCR = DMA_CGIF2;         // clear interrupt flags
  SMP_laps++;                       // one more pass (circular mode)
  SMP_done = 1;                     // buffer full (single pass)
}

// Setup timer and DMA and start sampling with a period of div clock cycles
// circular = 0: fill the buffer once with count samples
// circular = 1: fill the buffer again and again
void SMP_start(uint32_t div, uint16_t count, uint8_t circular) {
  // Setup timer TIM2 (clock = F_CPU)
  RCC->APB1PCENR     |= RCC_TIM2EN;
  TIM2->CTLR1         = 0;                    // stop timer
  TIM2->DMAINTENR     = 0;                    // no DMA requests yet
  TIM2->PSC           = 0;                    // no prescaler
  TIM2->ATRLR         = div - 1;              // period
  TIM2->CNT           = 0;                    // start at 0
  TIM2->SWEVGR        = TIM_UG;               // load registers
  TIM2->INTFR         = 0;                    // clear flags

  // Setup DMA1 channel 2 (TIM2 update): GPIOA->INDR (low byte) to buffer
  RCC->AHBPCENR      |= RCC_DMA1EN;
  DMA1_Channel2->CFGR  = 0;                   // disable channel
  DMA1->INTFCR         = DMA_CGIF2;           // clear flags
  DMA1_Channel2->PADDR = (uint32_t)&GPIOA->INDR;
  DMA1_Channel2->MADDR = (uint32_t)SMP_buffer;
  DMA1_Channel2->CNTR  = count;
  SMP_laps = 0; SMP_done = 0;
  DMA1_Channel2->CFGR  = DMA_CFGR1_MINC       // increment memory address
                       | DMA_CFGR1_PL         // very high priority
                       | DMA_CFGR1_TCIE       // interrupt at the end of the buffer
                       | (circular ? DMA_CFGR1_CIRC : 0)
                       | DMA_CFGR1_EN;        // enable channel (8-bit transfers)
  NVIC_EnableIRQ(DMA1_Channel2_IRQn);

  // Start sampling
  SMP_div     = div;
  SMP_w0      = 0;
  SMP_running = 1;
  TIM2->DMAINTENR = TIM_UDE;                  // update event triggers DMA
  SMP_t0          = STK->CNTL;                // start of the time check
  TIM2->CTLR1     = TIM_CEN;                  // start timer
}

// Stop sampling
void SMP_stop(void) {
  TIM2->CTLR1         = 0;                    // stop timer
  TIM2->DMAINTENR     = 0;                    // no more DMA requests
  DMA1_Channel2->CFGR &= ~DMA_CFGR1_EN;       // disable DMA channel
  SMP_running = 0;
}

// Check that the DMA served every request of the timer: written samples (read after
// the time now) must match the time since the start. If the bus is too busy for the
// sample rate, the DMA skips requests and the samples would be further apart than
// the host assumes. Returns 1 if samples were lost.
uint8_t SMP_lost(uint32_t now, uint32_t written) {
  uint32_t t   = now - SMP_t0;
  uint32_t due = t / SMP_div;                 // samples the timer has asked for
  uint32_t got = written - SMP_w0;            // samples the DMA has written
  if(t >= (1UL << 30)) {                      // long captures: start a new window
    SMP_t0 = now;                             // (the SysTick difference must not
    SMP_w0 = written;                         // overflow)
  }
  return due > got + (got >> 5) + 32;         // 3% and 32 samples of tolerance
}

// Number of samples written in circular mode so far
uint32_t SMP_written(void) {
  uint32_t laps, pos;
  do {
    laps = SMP_laps;
    pos  = BUF_SIZE - DMA1_Channel2->CNTR;
  } while(laps != SMP_laps);
  if(pos >= BUF_SIZE) pos = 0;
  return laps * BUF_SIZE + pos;
}

// Number of samples written in single pass mode so far
uint32_t SMP_count(uint32_t count) {
  return count - DMA1_Channel2->CNTR;
}

// ===================================================================================
// Test Signal on PC3 (TIM1 CH4, remap 3)
// ===================================================================================
void TEST_init(void) {
  #if TEST_FREQ > 0
  uint32_t period = F_CPU / TEST_FREQ;
  uint32_t psc    = (period - 1) >> 16;
  uint32_t top    = period / (psc + 1) - 1;
  RCC->APB2PCENR |= RCC_AFIOEN | RCC_IOPCEN | RCC_TIM1EN;
  AFIO->PCFR1     = (AFIO->PCFR1 & ~AFIO_PCFR1_TIM1_REMAP)
                  | AFIO_PCFR1_TIM1_REMAP_1 | AFIO_PCFR1_TIM1_REMAP_0;
  PIN_alternate(PIN_TEST);
  TIM1->PSC       = psc;
  TIM1->ATRLR     = top;
  TIM1->CH4CVR    = (top + 1) >> 1;           // 50% duty cycle
  TIM1->CHCTLR2   = TIM_OC4M_2 | TIM_OC4M_1 | TIM_OC4PE;  // PWM mode 1
  TIM1->CCER      = TIM_CC4E;                 // enable output CH4
  TIM1->BDTR      = TIM_MOE;                  // main output enable
  TIM1->SWEVGR    = TIM_UG;                   // load registers
  TIM1->CTLR1     = TIM_ARPE | TIM_CEN;       // start timer
  #endif
}

// ===================================================================================
// Encoder (run-length encoding of the sigrok-pico protocol)
// ===================================================================================
uint32_t ENC_bytes;                 // data bytes sent
uint32_t ENC_rle;                   // repeats of the last sample not sent yet
uint8_t  ENC_last;                  // last sample
uint8_t  ENC_first;                 // 1: no sample sent yet
uint8_t  ENC_d4;                    // 1: 4-channel mode (D0..D3 only)
uint8_t  ENC_bps;                   // bytes per sample (7 channels each)
uint8_t  ENC_mask;                  // enabled channels

// Send one data byte
static inline void ENC_out(uint8_t b) {
  CDC_write(b);
  ENC_bytes++;
}

// Prepare encoder for a new capture
void ENC_init(uint8_t mask) {
  uint8_t i, n = 0;
  for(i=0; i<NUM_D_CHAN; i++) if((mask >> i) & 1) n++;
  ENC_mask  = mask;
  ENC_d4    = !(mask & 0xF0);
  ENC_bps   = (n + 6) / 7;
  ENC_bytes = 0;
  ENC_rle   = 0;
  ENC_first = 1;
}

// Send a complete sample
void ENC_full(uint8_t v) {
  uint8_t i;
  if(ENC_d4) ENC_out(0x80 | v);
  else for(i=0; i<ENC_bps; i++) {
    ENC_out(0x80 | (v & 0x7F));
    v >>= 7;
  }
}

// Send the repeats of the last sample
void ENC_flushRLE(void) {
  if(ENC_d4) {
    if(ENC_rle > 7) {                         // multiples of 8 (8..632)
      ENC_out(((ENC_rle & 0x3F8) >> 3) + 47);
      ENC_rle &= 7;
    }
    if(ENC_rle) {                             // 1..7: r-1 repeats and the value again
      ENC_out(0x80 | ENC_last | ((ENC_rle - 1) << 4));
      ENC_rle = 0;
    }
  }
  else {
    while(ENC_rle >= 1568) {                  // 1568 repeats
      ENC_out(127);
      ENC_rle -= 1568;
    }
    if(ENC_rle > 32) {                        // multiples of 32
      uint32_t k = ENC_rle >> 5;
      ENC_out(k + 78);
      ENC_rle -= k << 5;
    }
    if(ENC_rle) {                             // 1..32
      ENC_out(47 + ENC_rle);
      ENC_rle = 0;
    }
  }
}

// The sample value changed: send repeats and the new value
void ENC_change(uint8_t v) {
  if(ENC_d4) {
    if(ENC_rle > 7) {
      ENC_out(((ENC_rle & 0x3F8) >> 3) + 47);
      ENC_rle &= 7;
    }
    ENC_out(0x80 | v | (ENC_rle << 4));       // repeats of the last value, then v
  }
  else {
    ENC_flushRLE();
    ENC_full(v);
  }
  ENC_last = v;
  ENC_rle  = 0;
}

// Encode n samples from buffer p
void ENC_run(const uint8_t* p, uint32_t n) {
  uint8_t  v, last, mask = ENC_mask;
  uint32_t rle;
  uint32_t rleMax = ENC_d4 ? 640 : 1568;
  if(!n) return;
  if(ENC_first) {                             // the first sample is always sent
    ENC_first = 0;
    ENC_last  = *p++ & mask;
    ENC_full(ENC_last);
    n--;
  }
  last = ENC_last; rle = ENC_rle;
  while(n--) {
    v = *p++ & mask;
    if(v == last) {                           // same value: count repeats
      if(++rle == rleMax) {                   // longest run: send it
        ENC_out(127);
        rle = 0;
      }
      continue;
    }
    ENC_rle = rle;                            // value changed
    ENC_change(v);
    last = v; rle = 0;
  }
  ENC_last = last; ENC_rle = rle;
}

// ===================================================================================
// USB Output Functions
// ===================================================================================

// Send everything that is in the CDC buffer and wait until it is gone
void OUT_flush(void) {
  while(!CDC_ready());
  CDC_flush();
  while(!CDC_ready());
}

// Send a string (not counted as data)
void OUT_str(const char* s) {
  while(*s) CDC_write(*s++);
}

// Send a decimal number (not counted as data)
void OUT_dec(uint32_t n) {
  char buf[11];
  uint8_t i = 0;
  do { buf[i++] = '0' + n % 10; n /= 10; } while(n);
  while(i) CDC_write(buf[--i]);
}

// Acknowledge a command
void OUT_ack(void) {
  CDC_write('*');
  CDC_flush();
}

// ===================================================================================
// Logic Analyzer (sigrok-pico protocol)
// ===================================================================================
uint32_t LA_rate  = 5000;           // sample rate in Hz
uint32_t LA_limit = 10;             // number of samples
uint8_t  LA_dmask = 0;              // enabled digital channels
uint8_t  LA_cont  = 0;              // 1: continuous mode
uint8_t  LA_state = LA_IDLE;        // state
uint8_t  LA_plus  = 0;              // host asked to stop
uint32_t LA_read;                   // samples processed so far (stream)
uint32_t LA_pending;                // samples processed since the last flush

// Stop capture without sending anything
void LA_reset(void) {
  SMP_stop();
  LA_state = LA_IDLE;
  LA_plus  = 0;
  LED_off();
}

// Start a capture
void LA_start(void) {
  uint32_t div = F_CPU / LA_rate;
  ENC_init(LA_dmask);
  LA_plus = 0;
  LA_read = 0;
  LA_pending = 0;
  LED_on();
  if(!LA_cont && LA_limit <= BUF_SIZE) {      // short capture: into RAM first
    LA_state = LA_BUFFER;
    SMP_start(div, LA_limit, 0);
  }
  else {                                      // long capture: send while sampling
    LA_state = LA_STREAM;
    SMP_start(div, BUF_SIZE, 1);
  }
}

// End of capture: send the rest and the byte count
void LA_finish(void) {
  SMP_stop();
  ENC_flushRLE();
  OUT_flush();
  DLY_ms(10);                                 // give the host time for the data
  CDC_write('$'); OUT_dec(ENC_bytes); CDC_write('+');
  OUT_flush();
  LA_state = LA_IDLE;
  LA_plus  = 0;
  LED_off();
}

// Abort: the USB could not keep up
void LA_abort(void) {
  SMP_stop();
  OUT_flush();
  DLY_ms(200);
  OUT_str("!!!"); OUT_flush();
  DLY_ms(200);
  OUT_str("$0+"); OUT_flush();
  LA_state = LA_IDLE;
  LA_plus  = 0;
  LED_off();
}

// Capture into RAM: send when the buffer is full (or when the host stops it)
void LA_buffer(void) {
  if(SMP_done) {
    SMP_stop();
    if(SMP_lost(SMP_tend, LA_limit)) {        // DMA too slow for the sample rate
      LA_abort();
      return;
    }
    ENC_run(SMP_buffer, LA_limit);
    LA_finish();
  }
  else if(LA_plus) {
    SMP_stop();
    ENC_run(SMP_buffer, SMP_count(LA_limit));
    LA_finish();
  }
}

// Capture while sending
void LA_stream(void) {
  uint32_t now     = STK->CNTL;
  uint32_t written = SMP_written();
  int32_t  avail;
  uint32_t n, idx, first;

  // DMA too slow for the sample rate (e.g. with much USB traffic): samples lost
  if(SMP_running && SMP_lost(now, written)) {
    LA_abort();
    return;
  }

  // Fixed number of samples: stop sampling when all of them are in the buffer
  if(!LA_cont && written >= LA_limit) {
    SMP_stop();
    written = LA_limit;
  }

  // New samples in the buffer?
  avail = (int32_t)(written - LA_read);
  if(avail > (int32_t)(BUF_SIZE - 2048)) {    // samples about to be overwritten: too slow
    LA_abort();
    return;
  }
  if(avail > 0) {
    n = avail > 1024 ? 1024 : avail;          // in chunks, to stay responsive
    idx = LA_read % BUF_SIZE;
    first = BUF_SIZE - idx;                   // samples up to the end of the buffer
    if(n <= first) ENC_run(&SMP_buffer[idx], n);
    else {
      ENC_run(&SMP_buffer[idx], first);
      ENC_run(SMP_buffer, n - first);
    }
    LA_read += n;
    if(!LA_cont && LA_read >= LA_limit) {     // all samples sent
      LA_finish();
      return;
    }
    LA_pending += n;
    if((n < 1024) && (LA_pending >= LA_rate / 32)) {  // caught up and ~30ms passed:
      ENC_flushRLE();                         // send what we have, so the host
      CDC_flush();                            // sees the samples coming in
      LA_pending = 0;
    }
  }
  if(LA_plus) LA_finish();                    // host has enough
}

// ===================================================================================
// Command Interpreter
// ===================================================================================
char    CMD_buf[20];
uint8_t CMD_ptr = 0;

// Decimal number from string
uint32_t CMD_num(const char* s) {
  uint32_t n = 0;
  while(*s >= '0' && *s <= '9') n = n * 10 + (*s++ - '0');
  return n;
}

// Execute a complete command line
void CMD_exec(void) {
  uint32_t n, e;
  if(LA_state != LA_IDLE) return;             // no settings during a capture
  switch(CMD_buf[0]) {
    case 'i':                                 // identify
      OUT_str(ID_STR); CDC_flush();
      break;
    case 'R':                                 // sample rate
      n = CMD_num(&CMD_buf[1]);
      if((n >= SAMPLE_RATE_MIN) && (n <= SAMPLE_RATE_MAX) && !(F_CPU % n)) {
        LA_rate = n;
        OUT_ack();
      }
      break;
    case 'L':                                 // number of samples
      n = CMD_num(&CMD_buf[1]);
      if(n) {
        LA_limit = n;
        OUT_ack();
      }
      break;
    case 'a':                                 // analog scale and offset (uV)
      OUT_str("25700x0"); CDC_flush();
      break;
    case 'A':                                 // analog channel enable (none)
    case 'D':                                 // digital channel enable
      e = CMD_buf[1] - '0';
      n = CMD_num(&CMD_buf[2]);
      if((e <= 1) && (CMD_buf[2] >= '0') && (n <= 31)) {
        if((CMD_buf[0] == 'D') && (n < NUM_D_CHAN)) {
          if(e) LA_dmask |=  (1 << n);
          else  LA_dmask &= ~(1 << n);
        }
        OUT_ack();
      }
      break;
    case 'F':                                 // fixed number of samples
      LA_cont = 0;
      LA_start();
      break;
    case 'C':                                 // continuous until '+'
      LA_cont = 1;
      LA_start();
      break;
    default:
      break;
  }
}

// Process a received character
void CMD_process(char c) {
  if(c == '*') {                              // reset
    LA_reset();
    CMD_ptr = 0;
  }
  else if(c == '+') {                         // stop capture
    if(LA_state != LA_IDLE) LA_plus = 1;
  }
  else if((c == '\n') || (c == '\r')) {       // end of command
    CMD_buf[CMD_ptr] = 0;
    if(CMD_ptr) CMD_exec();
    CMD_ptr = 0;
  }
  else {
    if(CMD_ptr < sizeof(CMD_buf) - 1) CMD_buf[CMD_ptr++] = c;
    else CMD_ptr = 0;                         // too long: drop it
  }
}

// ===================================================================================
// Main Function
// ===================================================================================
int main(void) {
  // Setup pins
  PIN_output(PIN_LED); LED_off();
  #if INPUT_PULLDOWN > 0
  PIN_input_PD(PA0); PIN_input_PD(PA1); PIN_input_PD(PA2); PIN_input_PD(PA3);
  PIN_input_PD(PA4); PIN_input_PD(PA5); PIN_input_PD(PA6); PIN_input_PD(PA7);
  #else
  PIN_input(PA0); PIN_input(PA1); PIN_input(PA2); PIN_input(PA3);
  PIN_input(PA4); PIN_input(PA5); PIN_input(PA6); PIN_input(PA7);
  #endif

  // Setup peripherals
  TEST_init();                                // test signal on PC3
  CDC_init();                                 // init USB CDC

  // Loop
  while(1) {
    while(CDC_available()) CMD_process(CDC_read());
    if(LA_state == LA_BUFFER)      LA_buffer();
    else if(LA_state == LA_STREAM) LA_stream();
  }
}
