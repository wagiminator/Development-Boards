// ===================================================================================
// USB PD SINK Handler for CH32X035                                           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// PD_connect()             Initialize USB-PD and connect, returns 0 if failed
// PD_setVoltage(mV)        Request specified voltage in millivolts, returns 0 if failed
//
// PD_getPDONum()           Get total number of PDOs
// PD_getFixedNum()         Get number of fixed power PDOs
// PD_getPPSNum()           Get number of programmable power PDOs       
//
// PD_getPDOVoltage(p)      Get voltage of specified fixed power PDO (1..PD_getFixedNum())
// PD_getPDOMinVoltage(p)   Get min voltage of specified PDO (p = 1..PD_getPDONum())
// PD_getPDOMaxVoltage(p)   Get max voltage of specified PDO (p = 1..PD_getPDONum())
// PD_getPDOMaxCurrent(p)   Get max current of specified PDO (p = 1..PD_getPDONum())
//
// PD_getPDO()              Get active PDO
// PD_getVoltage()          Get active voltage
// PD_getCurrent()          Get active max current
//
// Reference:               https://github.com/openwch/ch32x035
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "config.h"
#include "system.h"
#include "usbpd.h"

// ===================================================================================
// Parameters and Checks
// ===================================================================================
#if SYS_USE_VECTORS == 0
  #error Interrupt vector table must be enabled (SYS_USE_VECTORS in system.h)!
#endif

#ifndef USB_VDD
  #define USB_VDD         0             // MCU supply voltage (0: 3.3, 1: 5V)
#endif

#if   F_CPU == 48000000
  #define USBPD_TMR_TX    (80-1)        // timer value for USB PD BMC TX @ F_CPU=48MHz
  #define USBPD_TMR_RX    (120-1)       // timer value for USB PD BMC RX @ F_CPU=48MHz
#elif F_CPU == 24000000
  #define USBPD_TMR_TX    (40-1)        // timer value for USB PD BMC TX @ F_CPU=24MHz
  #define USBPD_TMR_RX    (60-1)        // timer value for USB PD BMC RX @ F_CPU=24MHz
#elif F_CPU == 12000000
  #define USBPD_TMR_TX    (20-1)        // timer value for USB PD BMC TX @ F_CPU=12MHz
  #define USBPD_TMR_RX    (30-1)        // timer value for USB PD BMC RX @ F_CPU=12MHz
#elif F_CPU ==  6000000
  #define USBPD_TMR_TX    (10-1)        // timer value for USB PD BMC TX @ F_CPU=6MHz
  #define USBPD_TMR_RX    (15-1)        // timer value for USB PD BMC RX @ F_CPU=6MHz
#else
  #error Unsupported system frequency for USBPD!
#endif

// ===================================================================================
// Type defines
// ===================================================================================
typedef struct {
  uint16_t Current;
  uint16_t Voltage;
} FixedSourceCap_t;

typedef struct  {
  uint16_t MinVoltage;
  uint16_t MaxVoltage;
  uint16_t Current;
} PPSSourceCap_t;

typedef enum {
  CC_IDLE = 0u,
  CC_CHECK_CONNECT,
  CC_CONNECT,
  CC_SOURCE_CAP,
  CC_SEND_REQUEST,
  CC_WAIT_ACCEPT,
  CC_ACCEPT,
  CC_WAIT_PS_RDY,
  CC_PS_RDY,
  CC_GET_SOURCE_CAP,
} cc_state_t;

typedef struct {
  volatile cc_state_t CC_State;
  volatile cc_state_t CC_LastState;
  volatile uint8_t    CC_NoneTimes;
  volatile uint8_t    CC1_ConnectTimes;
  volatile uint8_t    CC2_ConnectTimes;
  FixedSourceCap_t*   FixedSourceCap;
  PPSSourceCap_t*     PPSSourceCap;
  volatile uint8_t    SourcePDONum;
  volatile uint8_t    SourcePPSNum;
  volatile uint8_t    PD_Version;
  volatile uint16_t   WaitTime;
  volatile uint8_t    SetPDONum;
  volatile uint8_t    LastSetPDONum;
  volatile uint16_t   SetVoltage;
  volatile uint16_t   LastSetVoltage;
  volatile uint8_t    USBPD_READY;
  volatile uint8_t    SourceMessageID;
  volatile uint8_t    SinkMessageID;
  volatile uint8_t    SinkGoodCRCOver;
  volatile uint8_t    SourceGoodCRCOver;
} pd_control_t;

// ===================================================================================
// Functions
// ===================================================================================
uint8_t  PD_connect(void);                      // Initialize PD and connect
uint8_t  PD_setVoltage(uint16_t voltage);       // Set specified voltage (in millivolts)

uint8_t  PD_getPDONum(void);                    // Get total number of PDOs
uint8_t  PD_getFixedNum(void);                  // Get number of fixed power PDOs
uint8_t  PD_getPPSNum(void);                    // Get number of programmable power PDOs       

uint16_t PD_getPDOVoltage(uint8_t pdonum);      // Get voltage of specified fixed power PDO
uint16_t PD_getPDOMinVoltage(uint8_t pdonum);   // Get minimum voltage of specified PDO
uint16_t PD_getPDOMaxVoltage(uint8_t pdonum);   // Get maximum voltage of specified PDO
uint16_t PD_getPDOMaxCurrent(uint8_t pdonum);   // Get max current of specified PDO

uint8_t  PD_getPDO(void);                       // Get active PDO
uint16_t PD_getVoltage(void);                   // Get active voltage
uint16_t PD_getCurrent(void);                   // Get active max current

uint8_t PD_setPDO(uint8_t pdonum, uint16_t voltage);  // Set specified PDO and voltage

#ifdef __cplusplus
}
#endif
