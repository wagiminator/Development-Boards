// ===================================================================================
// Basic GPIO Functions for ATSAMD21E, ATSAMD21G, ATSAMD21J                   * v1.0 *
// ===================================================================================
//
// Pins must be defined as PA0, PA1, .., PA31, PB0, .., PB31 - e.g.:
// #define PIN_LED PA5      // LED on pin PA5
//
// PIN functions available:
// ------------------------
// PIN_input(PIN)           Set PIN as INPUT (floating, no pullup/pulldown)
// PIN_input_PU(PIN)        Set PIN as INPUT with internal PULLUP resistor
// PIN_input_PD(PIN)        Set PIN as INPUT with internal PULLDOWN resistor
// PIN_input_AN(PIN)        Set PIN as INPUT for analog peripherals (ADC, AC, DAC)
// PIN_output(PIN)          Set PIN as OUTPUT (push-pull)
// PIN_disable(PIN)         Disable PIN (input buffer off, high impedance) (*)
//
// PIN_pullup(PIN)          Enable PULLUP resistor on PIN (input only)
// PIN_pulldown(PIN)        Enable PULLDOWN resistor on PIN (input only)
// PIN_pulloff(PIN)         Disable PULLUP/PULLDOWN resistor on PIN (*)
// PIN_drive(PIN, s)        Set PIN output drive strength (0: normal (*), 1: strong)
// PIN_alternate(PIN, AF)   Set peripheral function AF (0..7 = A..H) for PIN
// PIN_alternate_off(PIN)   Disable peripheral function for PIN (back to GPIO) (*)
//
// PIN_low(PIN)             Set PIN output value to LOW (*)
// PIN_high(PIN)            Set PIN output value to HIGH
// PIN_toggle(PIN)          TOGGLE PIN output value
// PIN_read(PIN)            Read PIN input value
// PIN_write(PIN, val)      Write PIN output value (0 = LOW / 1 = HIGH)
//
// PIN interrupt and event functions available:
// --------------------------------------------
// PIN_EVT_set(PIN,TYPE)    Setup PIN event TYPE:
//                          PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH,
//                          PIN_EVT_HIGH, PIN_EVT_LOW
// PIN_INT_set(PIN,TYPE)    Setup PIN interrupt TYPE:
//                          PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH,
//                          PIN_INT_HIGH, PIN_INT_LOW
// PIN_INT_enable(PIN)      Enable EIC interrupt (NVIC)
// PIN_INT_disable(PIN)     Disable EIC interrupt (NVIC)
// PIN_INTFLAG_read(PIN)    Read interrupt flag of PIN
// PIN_INTFLAG_clear(PIN)   Clear interrupt flag of PIN
// PIN_INT_ISR { }          Pin interrupt service routine (EIC_IRQHandler)
//
// PORT functions available:
// -------------------------
// PORT_enable(PIN)         Enable GPIO PORT of PIN
// PORTA_enable()           Enable GPIO PORT A (*)
// PORTB_enable()           Enable GPIO PORT B (*)
// PORTS_enable()           Enable all GPIO PORTS (*)
// PORT_disable(PIN)        Disable GPIO PORT of PIN
// PORTA_disable()          Disable GPIO PORT A
// PORTB_disable()          Disable GPIO PORT B
// PORTS_disable()          Disable all GPIO PORTS
// (both ports share one module clock, so all ports are enabled/disabled together)
//
// Analog-to-Digital Converter (ADC) functions available:
// ------------------------------------------------------
// ADC_init()               Init, calibrate and enable ADC (must be called first)
// ADC_enable()             Enable ADC
// ADC_disable()            Disable ADC (*)
//
// ADC_fast()               Set fast mode   ( 1 ADC clock cycle sampling, least accurate)
// ADC_medium()             Set medium mode ( 8 ADC clock cycles sampling) (*)
// ADC_slow()               Set slow mode   (32 ADC clock cycles sampling, most accurate)
//
// ADC_input(PIN)           Set PIN as ADC input (reference VDD, range 0..VDD)
// ADC_input_VDD()          Set scaled supply voltage (VDD/4) as ADC input (reference 1V)
// ADC_input_TEMP()         Set internal temperature sensor as ADC input (reference 1V)
// ADC_input_DAC()          Set DAC output as ADC input (reference VDD)
//
// ADC_read()               Sample and read 12-bit ADC value (0..4095)
// ADC_read_VDD()           Sample and read supply voltage (VDD) in millivolts (mV)
// ADC_read_TEMP()          Sample and read temperature sensor in °C
//
// Digital-to-Analog Converter (DAC) functions available:
// ------------------------------------------------------
// DAC_init()               Init and enable DAC, output on PA2 (reference VDD)
// DAC_enable()             Enable DAC
// DAC_disable()            Disable DAC (*)
// DAC_write(n)             Write 10-bit value to DAC (0..1023)
// DAC_REF_1V()             Set internal 1V reference for DAC
// DAC_REF_VDD()            Set VDD (VDDANA) as reference for DAC
// DAC_REF_PA3()            Set external reference (VREFP, PA3) for DAC
//
// Analog Comparator (CMP) functions available:
// --------------------------------------------
// CMP_init()               Init and enable analog comparator module
// CMPx_enable()            Enable CMPx (x = 0..1)
// CMPx_disable()           Disable CMPx (*)
// CMPx_HYS_enable()        Enable CMPx hysteresis
// CMPx_HYS_disable()       Disable CMPx hysteresis (*)
// CMPx_read()              Read CMPx output state (1: POS > NEG)
//
// CMPx_POS_PA4()           Set CMPx positive input to pin PA4 (AIN0) (*)
// CMPx_POS_PA5()           Set CMPx positive input to pin PA5 (AIN1)
// CMPx_POS_PA6()           Set CMPx positive input to pin PA6 (AIN2)
// CMPx_POS_PA7()           Set CMPx positive input to pin PA7 (AIN3)
// CMPx_NEG_PA4()           Set CMPx negative input to pin PA4 (AIN0) (*)
// CMPx_NEG_PA5()           Set CMPx negative input to pin PA5 (AIN1)
// CMPx_NEG_PA6()           Set CMPx negative input to pin PA6 (AIN2)
// CMPx_NEG_PA7()           Set CMPx negative input to pin PA7 (AIN3)
// CMPx_NEG_GND()           Set CMPx negative input to GND
// CMPx_NEG_VDD(n)          Set CMPx negative input to VDD * (n + 1) / 64 (n = 0..63)
// CMPx_NEG_BANDGAP()       Set CMPx negative input to internal bandgap reference
// CMPx_NEG_DAC()           Set CMPx negative input to DAC output
//
// CMP0_OUT_PA12()          Set CMP0 output to pin PA12 (SAMD21G/J only)
// CMP0_OUT_PA18()          Set CMP0 output to pin PA18
// CMP1_OUT_PA13()          Set CMP1 output to pin PA13 (SAMD21G/J only)
// CMP1_OUT_PA19()          Set CMP1 output to pin PA19
// CMPx_OUT_OFF()           Disconnect CMPx output from pin (*)
//
// Notes:
// ------
// - (*) reset state
// - Available pins on SAMD21E (32 pins): PA0..PA11, PA14..PA19, PA22..PA25, PA27,
//   PA28 (RESET), PA30 (SWCLK), PA31 (SWDIO); SAMD21G (48 pins): additionally PA12,
//   PA13, PA20, PA21, PB2, PB3, PB8..PB11, PB22, PB23; SAMD21J (64 pins): additionally
//   PB0, PB1, PB4..PB7, PB12..PB17, PB30, PB31.
// - The SAMD21 has no open-drain outputs, they can be emulated by switching between
//   PIN_input() and PIN_output() with output value LOW.
// - Pins used for ADC, AC or DAC must be set with PIN_input_AN beforehand. ADC input
//   pins are: PA2 (AIN0), PA3 (AIN1), PB8 (AIN2), PB9 (AIN3), PA4..PA7 (AIN4..AIN7),
//   PB0..PB7 (AIN8..AIN15), PA8..PA11 (AIN16..AIN19). DAC output is on PA2.
// - External interrupt lines (EXTINT): pin number modulo 16 (e.g. PA3 and PB19 -> 3),
//   except PA24 (12), PA25 (13), PA27 (15), PA28 (8), PA30 (10), PA31 (11).
//   PA8 is NMI only. Each line number can only be used once at a time (e.g. PA4 and
//   PB4 cannot be used simultaneously, but PA4 and PA5 can).
// - The EIC is clocked by GCLK0. Edge detection does not work in STANDBY (GCLK0 is
//   stopped), use PIN_INT_HIGH or PIN_INT_LOW to wake up the MCU from STANDBY.
// - The ADC clock is GCLK0 with a prescaler (ADC clock <= 2.1MHz). ADC_read_TEMP()
//   uses the factory calibration values (temperature log row) of the device.
//
// 2026 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "system.h"

// ===================================================================================
// GPIO Register Access (configuration via APB, output via single-cycle IOBUS)
// ===================================================================================
#define GPIO_CFG(PIN)         (PORT->Group[((PIN) >> 5) & 1])       // configuration (APB)
#define GPIO_IOB(PIN)         (PORT_IOBUS->Group[((PIN) >> 5) & 1]) // fast access (IOBUS)
#define GPIO_BIT(PIN)         ((uint32_t)1 << ((PIN) & 31))
#define GPIO_PINCFG(PIN)      (GPIO_CFG(PIN).PINCFG[(PIN) & 31].reg)

// ===================================================================================
// Set PIN as INPUT (high impedance, no pullup/pulldown)
// ===================================================================================
#define PIN_input(PIN)        ( GPIO_IOB(PIN).DIRCLR.reg = GPIO_BIT(PIN),              \
                                GPIO_PINCFG(PIN)    = PORT_PINCFG_INEN )
#define PIN_input_HI          PIN_input
#define PIN_input_FL          PIN_input

// ===================================================================================
// Set PIN as INPUT with internal PULLUP resistor
// ===================================================================================
#define PIN_input_PU(PIN)     ( GPIO_IOB(PIN).DIRCLR.reg = GPIO_BIT(PIN),              \
                                GPIO_IOB(PIN).OUTSET.reg = GPIO_BIT(PIN),              \
                                GPIO_PINCFG(PIN)    = PORT_PINCFG_INEN            \
                                                    | PORT_PINCFG_PULLEN )

// ===================================================================================
// Set PIN as INPUT with internal PULLDOWN resistor
// ===================================================================================
#define PIN_input_PD(PIN)     ( GPIO_IOB(PIN).DIRCLR.reg = GPIO_BIT(PIN),              \
                                GPIO_IOB(PIN).OUTCLR.reg = GPIO_BIT(PIN),              \
                                GPIO_PINCFG(PIN)    = PORT_PINCFG_INEN            \
                                                    | PORT_PINCFG_PULLEN )

// ===================================================================================
// Set PIN as INPUT for analog peripherals (peripheral function B: ADC, AC, DAC)
// ===================================================================================
#define PIN_input_AN(PIN)     ( GPIO_IOB(PIN).DIRCLR.reg = GPIO_BIT(PIN),              \
                                PIN_alternate(PIN, 1) )
#define PIN_input_AD          PIN_input_AN
#define PIN_input_ADC         PIN_input_AN

// ===================================================================================
// Set PIN as OUTPUT PUSH-PULL (input buffer stays enabled for reading back)
// ===================================================================================
#define PIN_output(PIN)       ( GPIO_PINCFG(PIN)    = PORT_PINCFG_INEN,           \
                                GPIO_IOB(PIN).DIRSET.reg = GPIO_BIT(PIN) )
#define PIN_output_PP         PIN_output

// ===================================================================================
// Disable PIN (input buffer disabled, high impedance, reset state)
// ===================================================================================
#define PIN_disable(PIN)      ( GPIO_IOB(PIN).DIRCLR.reg = GPIO_BIT(PIN),              \
                                GPIO_PINCFG(PIN)    = 0 )

// ===================================================================================
// Enable PULLUP/PULLDOWN resistor on PIN (only if PIN is input), disable resistor
// ===================================================================================
#define PIN_pullup(PIN)       ( GPIO_IOB(PIN).OUTSET.reg = GPIO_BIT(PIN),              \
                                GPIO_PINCFG(PIN)   |= PORT_PINCFG_PULLEN )
#define PIN_pulldown(PIN)     ( GPIO_IOB(PIN).OUTCLR.reg = GPIO_BIT(PIN),              \
                                GPIO_PINCFG(PIN)   |= PORT_PINCFG_PULLEN )
#define PIN_pulloff(PIN)      ( GPIO_PINCFG(PIN)   &= ~PORT_PINCFG_PULLEN )

// ===================================================================================
// Set PIN output drive strength (0: normal, 1: strong)
// ===================================================================================
#define PIN_drive(PIN, s)     ( (s) ? (GPIO_PINCFG(PIN) |=  PORT_PINCFG_DRVSTR)   \
                                    : (GPIO_PINCFG(PIN) &= ~PORT_PINCFG_DRVSTR) )
#define PIN_speed             PIN_drive

// ===================================================================================
// Set peripheral function AF (0..7 = A..H) for PIN, disable peripheral function
// ===================================================================================
#define PIN_alternate(PIN, AF) \
  ( ((PIN) & 1) ? (GPIO_CFG(PIN).PMUX[((PIN) & 31) >> 1].reg =                         \
                  (GPIO_CFG(PIN).PMUX[((PIN) & 31) >> 1].reg & ~PORT_PMUX_PMUXO_Msk)   \
                  | PORT_PMUX_PMUXO(AF))                                          \
                : (GPIO_CFG(PIN).PMUX[((PIN) & 31) >> 1].reg =                         \
                  (GPIO_CFG(PIN).PMUX[((PIN) & 31) >> 1].reg & ~PORT_PMUX_PMUXE_Msk)   \
                  | PORT_PMUX_PMUXE(AF)),                                         \
    GPIO_PINCFG(PIN) |= PORT_PINCFG_PMUXEN )
#define PIN_alternate_off(PIN) ( GPIO_PINCFG(PIN) &= ~PORT_PINCFG_PMUXEN )

// ===================================================================================
// Set PIN output value (LOW, HIGH, TOGGLE, WRITE) and read PIN input value
// ===================================================================================
#define PIN_low(PIN)          ( GPIO_IOB(PIN).OUTCLR.reg = GPIO_BIT(PIN) )
#define PIN_high(PIN)         ( GPIO_IOB(PIN).OUTSET.reg = GPIO_BIT(PIN) )
#define PIN_toggle(PIN)       ( GPIO_IOB(PIN).OUTTGL.reg = GPIO_BIT(PIN) )
#define PIN_read(PIN)         ( (GPIO_CFG(PIN).IN.reg >> ((PIN) & 31)) & 1 )
#define PIN_write(PIN, val)   ( (val) ? (PIN_high(PIN)) : (PIN_low(PIN)) )

// ===================================================================================
// Setup PIN interrupt / event (EIC)
// ===================================================================================
enum{PIN_INT_OFF, PIN_INT_RISING, PIN_INT_FALLING, PIN_INT_BOTH, PIN_INT_HIGH, PIN_INT_LOW};
enum{PIN_EVT_OFF, PIN_EVT_RISING, PIN_EVT_FALLING, PIN_EVT_BOTH, PIN_EVT_HIGH, PIN_EVT_LOW};

// External interrupt line of PIN (0xff: no external interrupt available)
#define PIN_EXTINT(PIN) \
  ( ((PIN) == PA8 ) ? 0xff : ((PIN) == PA24) ? 12 : ((PIN) == PA25) ? 13 :         \
    ((PIN) == PA27) ? 15   : ((PIN) == PA28) ? 8  : ((PIN) == PA30) ? 10 :         \
    ((PIN) == PA31) ? 11   : ((PIN) & 15) )

#define EIC_sync()            while(EIC->STATUS.reg & EIC_STATUS_SYNCBUSY)

// Setup external interrupt controller line (sense: 0 = off, 1..5 see above;
// evt: 0 = interrupt, 1 = event). CONFIG and EVCTRL are enable-protected.
static inline void PIN_EIC_set(uint8_t pin, uint8_t line, uint8_t sense, uint8_t evt) {
  uint8_t  pos;
  uint32_t mask;
  if(line > 15) return;                             // no external interrupt line
  pos  = (line & 7) << 2;
  mask = (uint32_t)1 << line;
  if(!(PM->APBAMASK.reg & PM_APBAMASK_EIC) || !(EIC->CTRL.reg & EIC_CTRL_ENABLE)) {
    PM->APBAMASK.reg |= PM_APBAMASK_EIC;            // enable EIC module clock
    GCLK_setCH(EIC_GCLK_ID, GCLK_SYS);              // GCLK0 -> EIC
  }
  EIC->CTRL.reg = 0;                                // disable EIC
  EIC_sync();
  EIC->CONFIG[line >> 3].reg = (EIC->CONFIG[line >> 3].reg & ~((uint32_t)0x0f << pos))
                     | ((uint32_t)(sense & 7) << pos);
  if(evt) {                                         // event output
    if(sense) EIC->EVCTRL.reg |=  mask;
    else      EIC->EVCTRL.reg &= ~mask;
  }
  else {                                            // interrupt (with wake-up)
    if(sense) { EIC->INTENSET.reg = mask; EIC->WAKEUP.reg |=  mask; }
    else      { EIC->INTENCLR.reg = mask; EIC->WAKEUP.reg &= ~mask; }
    EIC->INTFLAG.reg = mask;                        // clear interrupt flag
  }
  EIC->CTRL.reg = EIC_CTRL_ENABLE;                  // enable EIC
  EIC_sync();
  if(sense) {                                       // connect pin to EIC (function A)
    GPIO_PINCFG(pin) |= PORT_PINCFG_INEN;
    PIN_alternate(pin, 0);
  }
}

#define PIN_INT_set(PIN, TYPE)  PIN_EIC_set(PIN, PIN_EXTINT(PIN), TYPE, 0)
#define PIN_EVT_set(PIN, TYPE)  PIN_EIC_set(PIN, PIN_EXTINT(PIN), TYPE, 1)

#define PIN_INT_enable(PIN)     NVIC_EnableIRQ(EIC_IRQn)
#define PIN_INT_disable(PIN)    NVIC_DisableIRQ(EIC_IRQn)

#define PIN_INTFLAG_read(PIN)   (EIC->INTFLAG.reg & ((uint32_t)1 << (PIN_EXTINT(PIN) & 15)))
#define PIN_INTFLAG_clear(PIN)  (EIC->INTFLAG.reg = ((uint32_t)1 << (PIN_EXTINT(PIN) & 15)))

#define PIN_INT_ISR             void EIC_IRQHandler(void)

// ===================================================================================
// Enable/disable GPIO PORT (PORT module clock, enabled after reset)
// ===================================================================================
#define PORTA_enable()          (PM->APBBMASK.reg |=  PM_APBBMASK_PORT)
#define PORTB_enable()          PORTA_enable()
#define PORTS_enable()          PORTA_enable()
#define PORT_enable(PIN)        PORTA_enable()
#define PORTA_disable()         (PM->APBBMASK.reg &= ~PM_APBBMASK_PORT)
#define PORTB_disable()         PORTA_disable()
#define PORTS_disable()         PORTA_disable()
#define PORT_disable(PIN)       PORTA_disable()

// ===================================================================================
// ADC Functions
// ===================================================================================

// ADC clock prescaler (ADC clock = GCLK0 / 2^(n+2) <= 2.1MHz)
#if   F_CPU <=   8400000
  #define ADC_PRESC         0       // DIV4
#elif F_CPU <=  16800000
  #define ADC_PRESC         1       // DIV8
#elif F_CPU <=  33600000
  #define ADC_PRESC         2       // DIV16
#else
  #define ADC_PRESC         3       // DIV32
#endif

// ADC input channels (MUXPOS) and references
#define ADC_CH_TEMP         0x18    // temperature sensor
#define ADC_CH_BANDGAP      0x19    // bandgap voltage
#define ADC_CH_VDD          0x1B    // scaled I/O supply voltage (VDDANA / 4)
#define ADC_CH_DAC          0x1C    // DAC output
#define ADC_REF_1V          ADC_REFCTRL_REFSEL_INT1V  // internal 1.0V reference
#define ADC_REF_VDD         (ADC_REFCTRL_REFSEL_INTVCC1 | ADC_REFCTRL_REFCOMP) // VDDANA/2

#define ADC_sync()          while(ADC->STATUS.reg & ADC_STATUS_SYNCBUSY)

// Set ADC sampling time (in half ADC clock cycles: (SAMPLEN + 1) / 2)
#define ADC_fast()          (ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(1))
#define ADC_medium()        (ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(15))
#define ADC_slow()          (ADC->SAMPCTRL.reg = ADC_SAMPCTRL_SAMPLEN(63))

// Enable/disable ADC
static inline void ADC_enable(void)  {ADC->CTRLA.reg = ADC_CTRLA_ENABLE; ADC_sync();}
static inline void ADC_disable(void) {ADC->CTRLA.reg = 0; ADC_sync();}

// Sample and read ADC value of current input
static inline uint16_t ADC_read(void) {
  ADC->INTFLAG.reg = ADC_INTFLAG_RESRDY;            // clear result ready flag
  ADC->SWTRIG.reg  = ADC_SWTRIG_START;              // start conversion
  while(!(ADC->INTFLAG.reg & ADC_INTFLAG_RESRDY));  // wait until finished
  return ADC->RESULT.reg;                           // return result
}

// Select ADC input channel (muxpos), gain and reference
static inline void ADC_setInput(uint8_t muxpos, uint32_t gain, uint8_t ref) {
  if(muxpos > 0x1C) return;                         // invalid channel
  ADC->INPUTCTRL.reg = ADC_INPUTCTRL_MUXPOS(muxpos) // set positive input
                     | ADC_INPUTCTRL_MUXNEG_GND     // negative input: internal ground
                     | gain;                        // set gain
  ADC_sync();
  if(ADC->REFCTRL.reg != ref) {                     // reference changed?
    ADC->REFCTRL.reg = ref;                         // set reference
    ADC_sync();
    (void)ADC_read();                               // discard first conversion
  }
}

// Set GPIO pin as ADC input (reference VDDANA/2, gain 1/2 -> range 0..VDD)
#define ADC_input(PIN) ADC_setInput(                                               \
  (((PIN) >= PA2 ) && ((PIN) <= PA3 )) ? ((PIN) - PA2)      :                      \
  (((PIN) >= PB8 ) && ((PIN) <= PB9 )) ? ((PIN) - PB8 + 2)  :                      \
  (((PIN) >= PA4 ) && ((PIN) <= PA7 )) ? ((PIN) - PA4 + 4)  :                      \
  (((PIN) >= PB0 ) && ((PIN) <= PB7 )) ? ((PIN) - PB0 + 8)  :                      \
  (((PIN) >= PA8 ) && ((PIN) <= PA11)) ? ((PIN) - PA8 + 16) : 0xff,                \
  ADC_INPUTCTRL_GAIN_DIV2, ADC_REF_VDD)

// Set internal channels as ADC input
#define ADC_input_VDD()     ADC_setInput(ADC_CH_VDD,  ADC_INPUTCTRL_GAIN_1X,   ADC_REF_1V)
#define ADC_input_TEMP()    ADC_setInput(ADC_CH_TEMP, ADC_INPUTCTRL_GAIN_1X,   ADC_REF_1V)
#define ADC_input_DAC()     ADC_setInput(ADC_CH_DAC,  ADC_INPUTCTRL_GAIN_DIV2, ADC_REF_VDD)

// Setup, calibrate and enable ADC
static inline void ADC_init(void) {
  uint32_t lin, bias;
  PM->APBCMASK.reg  |= PM_APBCMASK_ADC;             // enable ADC module clock
  GCLK_setCH(ADC_GCLK_ID, GCLK_SYS);                // GCLK0 -> ADC
  ADC->CTRLA.reg     = ADC_CTRLA_SWRST;             // reset ADC
  while(ADC->CTRLA.reg & ADC_CTRLA_SWRST);

  // Load factory calibration values
  lin  = ((*(uint32_t*)ADC_FUSES_LINEARITY_0_ADDR) & ADC_FUSES_LINEARITY_0_Msk)
         >> ADC_FUSES_LINEARITY_0_Pos;
  lin |= (((*(uint32_t*)ADC_FUSES_LINEARITY_1_ADDR) & ADC_FUSES_LINEARITY_1_Msk)
         >> ADC_FUSES_LINEARITY_1_Pos) << 5;
  bias = ((*(uint32_t*)ADC_FUSES_BIASCAL_ADDR) & ADC_FUSES_BIASCAL_Msk)
         >> ADC_FUSES_BIASCAL_Pos;
  ADC->CALIB.reg     = ADC_CALIB_LINEARITY_CAL(lin) | ADC_CALIB_BIAS_CAL(bias);

  // Setup ADC
  SYSCTRL->VREF.reg |= SYSCTRL_VREF_TSEN;           // enable temperature sensor
  ADC->REFCTRL.reg   = ADC_REF_VDD;                 // default reference: VDDANA/2
  ADC->CTRLB.reg     = ADC_CTRLB_PRESCALER(ADC_PRESC) // set ADC clock prescaler
                     | ADC_CTRLB_RESSEL_12BIT;      // 12-bit resolution
  ADC_sync();
  ADC_medium();                                     // set medium sampling time
  ADC->INPUTCTRL.reg = ADC_INPUTCTRL_MUXPOS(0)      // default input: AIN0 (PA2)
                     | ADC_INPUTCTRL_MUXNEG_GND
                     | ADC_INPUTCTRL_GAIN_DIV2;
  ADC_sync();
  ADC_enable();                                     // enable ADC
  (void)ADC_read();                                 // discard first conversion
}

// Sample and read supply voltage (VDD) in millivolts (mV)
static inline uint16_t ADC_read_VDD(void) {
  ADC_input_VDD();                                  // VDD/4 with 1V reference
  return ((uint32_t)ADC_read() * 4000 + 2048) >> 12;// VDD = 4 * 1000mV * val / 4096
}

// Sample and read temperature sensor in °C (linear interpolation between the factory
// calibration points at room and hot temperature, see datasheet "Temperature Sensor")
static inline int8_t ADC_read_TEMP(void) {
  uint32_t log0 = *(uint32_t*)FUSES_ROOM_TEMP_VAL_INT_ADDR;
  uint32_t log1 = *(uint32_t*)FUSES_ROOM_ADC_VAL_ADDR;
  int32_t tempR = ((log0 & FUSES_ROOM_TEMP_VAL_INT_Msk) >> FUSES_ROOM_TEMP_VAL_INT_Pos) * 10
                + ((log0 & FUSES_ROOM_TEMP_VAL_DEC_Msk) >> FUSES_ROOM_TEMP_VAL_DEC_Pos);
  int32_t tempH = ((log0 & FUSES_HOT_TEMP_VAL_INT_Msk)  >> FUSES_HOT_TEMP_VAL_INT_Pos)  * 10
                + ((log0 & FUSES_HOT_TEMP_VAL_DEC_Msk)  >> FUSES_HOT_TEMP_VAL_DEC_Pos);
  int32_t adcR  =  (log1 & FUSES_ROOM_ADC_VAL_Msk)      >> FUSES_ROOM_ADC_VAL_Pos;
  int32_t adcH  =  (log1 & FUSES_HOT_ADC_VAL_Msk)       >> FUSES_HOT_ADC_VAL_Pos;
  int32_t val, temp;
  uint32_t samp = ADC->SAMPCTRL.reg;                // save sampling time
  ADC_slow();                                       // temp sensor needs slow sampling
  ADC_input_TEMP();                                 // set temp sensor as ADC input
  val = ADC_read();                                 // sample temperature sensor
  ADC->SAMPCTRL.reg = samp;                         // restore sampling time
  if(adcH == adcR) return 0;                        // no calibration values
  temp = tempR + (val - adcR) * (tempH - tempR) / (adcH - adcR);  // in 0.1°C
  return (temp + (temp >= 0 ? 5 : -5)) / 10;        // round and return in °C
}

// ===================================================================================
// DAC Functions
// ===================================================================================
#define DAC_sync()          while(DAC->STATUS.reg & DAC_STATUS_SYNCBUSY)
static inline void DAC_enable(void)  {DAC->CTRLA.reg |=  DAC_CTRLA_ENABLE; DAC_sync();}
static inline void DAC_disable(void) {DAC->CTRLA.reg &= ~DAC_CTRLA_ENABLE; DAC_sync();}
static inline void DAC_write(uint16_t n) {DAC->DATA.reg = n & 0x3ff; DAC_sync();}

// Set DAC reference (DAC is disabled and enabled again)
static inline void DAC_setREF(uint8_t ref) {
  uint8_t en = DAC->CTRLA.reg & DAC_CTRLA_ENABLE;
  DAC->CTRLA.reg = 0;
  DAC_sync();
  DAC->CTRLB.reg = (DAC->CTRLB.reg & ~DAC_CTRLB_REFSEL_Msk) | ref;
  if(en) DAC_enable();
}
#define DAC_REF_1V()        DAC_setREF(DAC_CTRLB_REFSEL_INT1V)
#define DAC_REF_VDD()       DAC_setREF(DAC_CTRLB_REFSEL_AVCC)
#define DAC_REF_PA3()       DAC_setREF(DAC_CTRLB_REFSEL_VREFP)

// Init and enable DAC, output on PA2
static inline void DAC_init(void) {
  PM->APBCMASK.reg |= PM_APBCMASK_DAC;              // enable DAC module clock
  GCLK_setCH(DAC_GCLK_ID, GCLK_SYS);                // GCLK0 -> DAC
  PIN_input_AN(PA2);                                // set PA2 as analog pin
  DAC->CTRLA.reg = DAC_CTRLA_SWRST;                 // reset DAC
  while(DAC->CTRLA.reg & DAC_CTRLA_SWRST);
  DAC->CTRLB.reg = DAC_CTRLB_EOEN                   // enable external output
                 | DAC_CTRLB_IOEN                   // enable internal output (ADC/AC)
                 | DAC_CTRLB_REFSEL_AVCC;           // reference: VDDANA
  DAC->DATA.reg  = 0;
  DAC_enable();                                     // enable DAC
}

// ===================================================================================
// Analog Comparator (CMP) Functions
// ===================================================================================
#define AC_sync()           while(AC->STATUSB.reg & AC_STATUSB_SYNCBUSY)

// Init and enable analog comparator module
static inline void CMP_init(void) {
  PM->APBCMASK.reg |= PM_APBCMASK_AC;               // enable AC module clock
  GCLK_setCH(AC_GCLK_ID_DIG, GCLK_SYS);             // GCLK0 -> AC (digital)
  GCLK_setCH(AC_GCLK_ID_ANA, GCLK_SYS);             // GCLK0 -> AC (analog)
  AC->CTRLA.reg = AC_CTRLA_SWRST;                   // reset AC
  while(AC->CTRLA.reg & AC_CTRLA_SWRST);
  AC->COMPCTRL[0].reg = AC_COMPCTRL_SPEED_HIGH | AC_COMPCTRL_FLEN_MAJ3;
  AC->COMPCTRL[1].reg = AC_COMPCTRL_SPEED_HIGH | AC_COMPCTRL_FLEN_MAJ3;
  AC_sync();
  AC->CTRLA.reg = AC_CTRLA_ENABLE;                  // enable AC module
  AC_sync();
}

// Modify comparator control register (comparator is disabled while changing)
static inline void CMP_config(uint8_t n, uint32_t mask, uint32_t val) {
  uint32_t ctrl = AC->COMPCTRL[n].reg;
  AC->COMPCTRL[n].reg = ctrl & ~AC_COMPCTRL_ENABLE;
  AC_sync();
  ctrl = (ctrl & ~mask) | val;
  AC->COMPCTRL[n].reg = ctrl & ~AC_COMPCTRL_ENABLE;
  AC_sync();
  if(ctrl & AC_COMPCTRL_ENABLE) {
    AC->COMPCTRL[n].reg = ctrl;
    AC_sync();
  }
}

#define CMP0_enable()       CMP_config(0, AC_COMPCTRL_ENABLE, AC_COMPCTRL_ENABLE)
#define CMP0_disable()      CMP_config(0, AC_COMPCTRL_ENABLE, 0)
#define CMP0_HYS_enable()   CMP_config(0, AC_COMPCTRL_HYST, AC_COMPCTRL_HYST)
#define CMP0_HYS_disable()  CMP_config(0, AC_COMPCTRL_HYST, 0)
#define CMP0_read()         ((AC->STATUSA.reg & AC_STATUSA_STATE0) ? 1 : 0)
#define CMP0_POS_PA4()      CMP_config(0, AC_COMPCTRL_MUXPOS_Msk, AC_COMPCTRL_MUXPOS_PIN0)
#define CMP0_POS_PA5()      CMP_config(0, AC_COMPCTRL_MUXPOS_Msk, AC_COMPCTRL_MUXPOS_PIN1)
#define CMP0_POS_PA6()      CMP_config(0, AC_COMPCTRL_MUXPOS_Msk, AC_COMPCTRL_MUXPOS_PIN2)
#define CMP0_POS_PA7()      CMP_config(0, AC_COMPCTRL_MUXPOS_Msk, AC_COMPCTRL_MUXPOS_PIN3)
#define CMP0_NEG_PA4()      CMP_config(0, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_PIN0)
#define CMP0_NEG_PA5()      CMP_config(0, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_PIN1)
#define CMP0_NEG_PA6()      CMP_config(0, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_PIN2)
#define CMP0_NEG_PA7()      CMP_config(0, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_PIN3)
#define CMP0_NEG_GND()      CMP_config(0, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_GND)
#define CMP0_NEG_VDD(n)     (AC->SCALER[0].reg = (n) & 63, \
                            CMP_config(0, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_VSCALE))
#define CMP0_NEG_BANDGAP()  CMP_config(0, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_BANDGAP)
#define CMP0_NEG_DAC()      CMP_config(0, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_DAC)
#define CMP0_OUT_PA12()     (CMP_config(0, AC_COMPCTRL_OUT_Msk, AC_COMPCTRL_OUT_ASYNC), \
                             PIN_output(PA12), PIN_alternate(PA12, 7))
#define CMP0_OUT_PA18()     (CMP_config(0, AC_COMPCTRL_OUT_Msk, AC_COMPCTRL_OUT_ASYNC), \
                             PIN_output(PA18), PIN_alternate(PA18, 7))
#define CMP0_OUT_OFF()      CMP_config(0, AC_COMPCTRL_OUT_Msk, AC_COMPCTRL_OUT_OFF)

#define CMP1_enable()       CMP_config(1, AC_COMPCTRL_ENABLE, AC_COMPCTRL_ENABLE)
#define CMP1_disable()      CMP_config(1, AC_COMPCTRL_ENABLE, 0)
#define CMP1_HYS_enable()   CMP_config(1, AC_COMPCTRL_HYST, AC_COMPCTRL_HYST)
#define CMP1_HYS_disable()  CMP_config(1, AC_COMPCTRL_HYST, 0)
#define CMP1_read()         ((AC->STATUSA.reg & AC_STATUSA_STATE1) ? 1 : 0)
#define CMP1_POS_PA4()      CMP_config(1, AC_COMPCTRL_MUXPOS_Msk, AC_COMPCTRL_MUXPOS_PIN0)
#define CMP1_POS_PA5()      CMP_config(1, AC_COMPCTRL_MUXPOS_Msk, AC_COMPCTRL_MUXPOS_PIN1)
#define CMP1_POS_PA6()      CMP_config(1, AC_COMPCTRL_MUXPOS_Msk, AC_COMPCTRL_MUXPOS_PIN2)
#define CMP1_POS_PA7()      CMP_config(1, AC_COMPCTRL_MUXPOS_Msk, AC_COMPCTRL_MUXPOS_PIN3)
#define CMP1_NEG_PA4()      CMP_config(1, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_PIN0)
#define CMP1_NEG_PA5()      CMP_config(1, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_PIN1)
#define CMP1_NEG_PA6()      CMP_config(1, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_PIN2)
#define CMP1_NEG_PA7()      CMP_config(1, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_PIN3)
#define CMP1_NEG_GND()      CMP_config(1, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_GND)
#define CMP1_NEG_VDD(n)     (AC->SCALER[1].reg = (n) & 63, \
                            CMP_config(1, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_VSCALE))
#define CMP1_NEG_BANDGAP()  CMP_config(1, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_BANDGAP)
#define CMP1_NEG_DAC()      CMP_config(1, AC_COMPCTRL_MUXNEG_Msk, AC_COMPCTRL_MUXNEG_DAC)
#define CMP1_OUT_PA13()     (CMP_config(1, AC_COMPCTRL_OUT_Msk, AC_COMPCTRL_OUT_ASYNC), \
                             PIN_output(PA13), PIN_alternate(PA13, 7))
#define CMP1_OUT_PA19()     (CMP_config(1, AC_COMPCTRL_OUT_Msk, AC_COMPCTRL_OUT_ASYNC), \
                             PIN_output(PA19), PIN_alternate(PA19, 7))
#define CMP1_OUT_OFF()      CMP_config(1, AC_COMPCTRL_OUT_Msk, AC_COMPCTRL_OUT_OFF)

#ifdef __cplusplus
};
#endif
