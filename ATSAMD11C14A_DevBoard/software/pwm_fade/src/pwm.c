// ===================================================================================
// Basic PWM Functions using Timer/Counter for Control (TCC0) for SAMD11      * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "pwm.h"

// Init TCC0 for PWM (normal PWM, single slope, buffered compare values)
void PWM_init(void) {
  PM->APBCMASK.reg |= PM_APBCMASK_TCC0;           // enable TCC0 module clock
  GCLK_setCH(TCC0_GCLK_ID, GCLK_SYS);             // GCLK0 -> TCC0
  TCC0->CTRLA.reg = TCC_CTRLA_SWRST;              // reset TCC0
  while(TCC0->SYNCBUSY.reg & TCC_SYNCBUSY_SWRST);
  TCC0->CTRLA.reg = TCC_CTRLA_PRESCALER(PWM_PRESC);  // set prescaler
  TCC0->WAVE.reg  = TCC_WAVE_WAVEGEN_NPWM;        // normal PWM (single slope)
  TCC0->PER.reg   = TCC_PER_PER(PWM_TOP);         // set TOP value
  TCC0->CC[0].reg = 0;                            // all channels 0% duty cycle
  TCC0->CC[1].reg = 0;
  TCC0->CC[2].reg = 0;
  TCC0->CC[3].reg = 0;
  PWM_sync();
  PWM_enable();                                   // start counter
}

// Enable TCC0
void PWM_enable(void) {
  TCC0->CTRLA.reg |= TCC_CTRLA_ENABLE;
  PWM_sync();
}

// Disable TCC0
void PWM_disable(void) {
  TCC0->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  PWM_sync();
}

// Set compare value of channel (applied at the end of the current period)
void PWM_setCC(uint8_t ch, uint32_t val) {
  while(TCC0->SYNCBUSY.reg & (TCC_SYNCBUSY_CCB0 << (ch & 3)));
  TCC0->CCB[ch & 3].reg = TCC_CCB_CCB(val);
}

// Set TOP value (applied at the end of the current period)
void PWM_setTOP(uint32_t top) {
  while(TCC0->SYNCBUSY.reg & TCC_SYNCBUSY_PERB);
  TCC0->PERB.reg = TCC_PERB_PERB(top);
}

// Set prescaler (TCC0 is disabled while changing)
void PWM_setPRESC(uint8_t presc) {
  uint32_t ctrla = TCC0->CTRLA.reg;
  TCC0->CTRLA.reg = ctrla & ~TCC_CTRLA_ENABLE;
  PWM_sync();
  TCC0->CTRLA.reg = (ctrla & ~(TCC_CTRLA_PRESCALER_Msk | TCC_CTRLA_ENABLE))
                  | TCC_CTRLA_PRESCALER(presc);
  TCC0->CTRLA.reg |= ctrla & TCC_CTRLA_ENABLE;
  PWM_sync();
}
