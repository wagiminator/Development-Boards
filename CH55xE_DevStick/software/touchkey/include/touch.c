// ===================================================================================
// Touch Key Functions for CH551, CH552 and CH554
// ===================================================================================
//
// Simple touch key control functions without using baseline detection and interrupts.
//
// The following must be defined in config.h:
// TOUCH_TH_LOW  - key pressed detection threshold  - low  hysteresis value
// TOUCH_TH_HIGH - key released detection threshold - high hysteresis value
// Use touchraw firmware to find out threshold values.
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#include "touch.h"
#include "ch554.h"

// Store last pressed values
__idata uint8_t TOUCH_last = 0;

// Get one sample
uint16_t TOUCH_h_sample(uint8_t channel) {
  TKEY_CTRL = (TKEY_CTRL & 0xF8 | channel) + 1;   // start sampling channel
  while(~TKEY_CTRL & bTKC_IF);                    // wait for sampling complete
  return TKEY_DAT;                                // return sampling value
}

// Read touch key state (returns 0 = released, 1 = pressed)
__bit TOUCH_h_read(uint8_t channel) {
  uint16_t sample = TOUCH_h_sample(channel);
  if(TOUCH_last & (1 << channel)) {               // last time pressed?
    if(sample > TOUCH_TH_HIGH) {                  // but now released?
      TOUCH_last &= ~(1 << channel);              // clear last pressed flag
      return 0;                                   // return 'released'
    }
    return 1;                                     // return 'pressed'
  }
  else {                                          // last time released?
    if(sample < TOUCH_TH_LOW) {                   // but now pressed?
      TOUCH_last |= (1 << channel);               // set last pressed flag
      return 1;                                   // return 'pressed'
    }
    return 0;                                     // return 'released'
  }
}
