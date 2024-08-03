// ===================================================================================
// Basic BME280 Temperature, Pressure and Humidity Sensor Functions           * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// BME_init()               Init BME280
// BME_sleep(s)             Set BME280 sleep state (0: wake up, 1: sleep)
// BME_getTemp()            Read temperature (value "2391" means 23.91 °C)
// BME_getPressure()        Read pressure (value "101945" means 1019.45 hPa)
// BME_getHumidity()        Read humidity (value "4697" means 46.97 %RH)
// BME_adjustTemp(a)        Set user temperature adjustment ("-127" means -1.27 °C)
//
// References:
// -----------
// Adafruit:                https://github.com/adafruit/Adafruit_BME280_Library
// David Johnson-Davies:    https://github.com/technoblogy/tiny-bme280
//
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "i2c.h"                      // choose your I2C library
#include "system.h"

// BME280 Parameters
#define BME_INIT_I2C          1       // 1: init I2C with BME_init()
#define BME_INIT_RESET        0       // 1: reset BME280 with BME_init()
#define BME_INIT_TEMP         0       // 1: make first temp read with BME_init() for calib
#define BME_BOOT_TIME         0       // BME280 boot up time in milliseconds
#define BME_ADDR              0x76    // BME280 I2C device address (0x76 or 0x77)

// BME280 Calibration Data Registers
#define BME_REG_CALIB_T1      0x88    // temperature
#define BME_REG_CALIB_T2      0x8A
#define BME_REG_CALIB_T3      0x8C
#define BME_REG_CALIB_P1      0x8E    // pressure
#define BME_REG_CALIB_P2      0x90
#define BME_REG_CALIB_P3      0x92
#define BME_REG_CALIB_P4      0x94
#define BME_REG_CALIB_P5      0x96
#define BME_REG_CALIB_P6      0x98
#define BME_REG_CALIB_P7      0x9A
#define BME_REG_CALIB_P8      0x9C
#define BME_REG_CALIB_P9      0x9E
#define BME_REG_CALIB_H1      0xA1    // humidity
#define BME_REG_CALIB_H2      0xE1
#define BME_REG_CALIB_H3      0xE3
#define BME_REG_CALIB_H4      0xE4
#define BME_REG_CALIB_H5      0xE5
#define BME_REG_CALIB_H6      0xE7

// BME280 Control Registers
#define BME_REG_CHIPID        0xD0
#define BME_REG_VERSION       0xD1
#define BME_REG_SOFTRESET     0xE0
#define BME_REG_CAL26         0xE1    // R calibration stored in 0xE1-0xF0
#define BME_REG_CONTROLHUMID  0xF2
#define BME_REG_STATUS        0XF3
#define BME_REG_CONTROL       0xF4
#define BME_REG_CONFIG        0xF5

// BME280 Data Registers
#define BME_REG_PRESSUREDATA  0xF7
#define BME_REG_TEMPDATA      0xFA
#define BME_REG_HUMIDDATA     0xFD

// BME280 Calibration and Data Variables
typedef struct {
  uint16_t T1;
  int16_t  T2;
  int16_t  T3;
  uint16_t P1;
  int16_t  P2;
  int16_t  P3;
  int16_t  P4;
  int16_t  P5;
  int16_t  P6;
  int16_t  P7;
  int16_t  P8;
  int16_t  P9;
  uint8_t  X1;
  uint8_t  H1;
  int16_t  H2;
  uint8_t  H3;
  int8_t   H451;
  uint8_t  H452;
  int8_t   H453;
  int8_t   H6;
  int16_t  H4;
  int16_t  H5;
} BME_CALIB_TYPE;

// BME280 Functions
void BME_init(void);
void BME_sleep(uint8_t slp);
int32_t BME_getTemp(void);
uint32_t BME_getPressure(void);
uint32_t BME_getHumidity(void);
void BME_adjustTemp(int32_t adjust);

#ifdef __cplusplus
};
#endif
