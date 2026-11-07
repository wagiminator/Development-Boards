// ===================================================================================
// Basic PWM Functions using Timer/Counter for Control (TCC0..2) for SAMD21   * v1.0 *
// ===================================================================================
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#include "pwm.h"

static Tcc* const PWM_TCCs[3] = {TCC0, TCC1, TCC2};
#define PWM_sync(t)       while((t)->SYNCBUSY.reg)

// Init TCC0..2 for PWM (normal PWM, single slope, buffered compare values)
void PWM_init(void) {
  uint8_t i, ch;
  PM->APBCMASK.reg |= PM_APBCMASK_TCC0 | PM_APBCMASK_TCC1 | PM_APBCMASK_TCC2;
  GCLK_setCH(TCC0_GCLK_ID, GCLK_SYS);             // GCLK0 -> TCC0/TCC1
  GCLK_setCH(TCC2_GCLK_ID, GCLK_SYS);             // GCLK0 -> TCC2/TC3
  for(i = 0; i < 3; i++) {
    Tcc* tcc = PWM_TCCs[i];
    tcc->CTRLA.reg = TCC_CTRLA_SWRST;             // reset TCC
    while(tcc->SYNCBUSY.reg & TCC_SYNCBUSY_SWRST);
    tcc->CTRLA.reg = TCC_CTRLA_PRESCALER(PWM_PRESC);  // set prescaler
    tcc->WAVE.reg  = TCC_WAVE_WAVEGEN_NPWM;       // normal PWM (single slope)
    tcc->PER.reg   = TCC_PER_PER(PWM_TOP);        // set TOP value
    for(ch = 0; ch < (i ? 2 : 4); ch++) tcc->CC[ch].reg = 0;  // 0% duty cycle
    PWM_sync(tcc);
  }
  PWM_enable();                                   // start counters
}

// Enable TCC0..2
void PWM_enable(void) {
  uint8_t i;
  for(i = 0; i < 3; i++) {
    PWM_TCCs[i]->CTRLA.reg |= TCC_CTRLA_ENABLE;
    PWM_sync(PWM_TCCs[i]);
  }
}

// Disable TCC0..2
void PWM_disable(void) {
  uint8_t i;
  for(i = 0; i < 3; i++) {
    PWM_TCCs[i]->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
    PWM_sync(PWM_TCCs[i]);
  }
}

// Set compare value of channel (applied at the end of the current period)
void PWM_setCC(uint8_t tcc, uint8_t ch, uint32_t val) {
  Tcc* t;
  if(tcc > 2) return;
  t   = PWM_TCCs[tcc];
  ch &= (tcc ? 1 : 3);
  while(t->SYNCBUSY.reg & (TCC_SYNCBUSY_CCB0 << ch));
  t->CCB[ch].reg = TCC_CCB_CCB(val);
}

// Set TOP value of all TCCs (applied at the end of the current period)
void PWM_setTOP(uint32_t top) {
  uint8_t i;
  for(i = 0; i < 3; i++) {
    while(PWM_TCCs[i]->SYNCBUSY.reg & TCC_SYNCBUSY_PERB);
    PWM_TCCs[i]->PERB.reg = TCC_PERB_PERB(top);
  }
}

// Set prescaler of all TCCs (TCCs are disabled while changing)
void PWM_setPRESC(uint8_t presc) {
  uint8_t i;
  for(i = 0; i < 3; i++) {
    Tcc* t = PWM_TCCs[i];
    uint32_t ctrla = t->CTRLA.reg;
    t->CTRLA.reg = ctrla & ~TCC_CTRLA_ENABLE;
    PWM_sync(t);
    t->CTRLA.reg = (ctrla & ~(TCC_CTRLA_PRESCALER_Msk | TCC_CTRLA_ENABLE))
                 | TCC_CTRLA_PRESCALER(presc);
    t->CTRLA.reg |= ctrla & TCC_CTRLA_ENABLE;
    PWM_sync(t);
  }
}
