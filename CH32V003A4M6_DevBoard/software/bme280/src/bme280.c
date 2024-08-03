// ===================================================================================
// Basic BME280 Temperature, Pressure and Humidity Sensor Functions           * v1.0 *
// ===================================================================================
// 2024 by Stefan Wagner:   https://github.com/wagiminator

#include "bme280.h"

// Global variables
BME_CALIB_TYPE BME_calib_data;                    // calibration data variables
int32_t BME_t_fine;                               // fine temperature for compensation
int32_t BME_t_fine_adjust = 0;                    // user temperature adjustment

// Init BME280
void BME_init(void) {
  uint8_t  cnt;                                   // counter variable
  uint8_t* ptr= (uint8_t*)&BME_calib_data;        // pointer to calibration variables

  #if BME_INIT_I2C > 0
  I2C_init();                                     // initialize I2C first
  #endif

  #if BME_BOOT_TIME > 0
  DLY_ms(BME_BOOT_TIME);                          // time for the OLED to boot up
  #endif

  #if BME_INIT_RESET > 0
  I2C_start((BME_ADDR << 1) | 0);
  I2C_write(BME_REG_SOFTRESET);
  I2C_write(0xB6);                                // software reset
  I2C_stop();
  uint8_t status;
  do {
    DLY_ms(10);
    I2C_start((BME_ADDR << 1) | 0);
    I2C_write(BME_REG_STATUS);
    I2C_stop();
    I2C_start((BME_ADDR << 1) | 1);
    status = I2C_read(0);
    I2C_stop();
  } while(status & 1);                            // wait for calibration finished
  #endif

  I2C_start((BME_ADDR << 1) | 0);
  I2C_write(BME_REG_CONTROLHUMID);
  I2C_write(0b00000100);                          // humidity: 8x oversampling
  I2C_write(BME_REG_CONTROL);
  I2C_write(0b10010011);                          // pressure/temp: 8x oversampling, normal mode
  I2C_write(BME_REG_CALIB_T1);
  I2C_stop();

  cnt = 26;
  I2C_start((BME_ADDR << 1) | 1);
  while(cnt--) *ptr++ = I2C_read(cnt);            // read calibration data
  I2C_stop();

  I2C_start((BME_ADDR << 1) | 0);
  I2C_write(BME_REG_CALIB_H2);
  I2C_stop();
  cnt = 7;
  I2C_start((BME_ADDR << 1) | 1);
  while(cnt--) *ptr++ = I2C_read(cnt);            // read calibration data
  I2C_stop();

  BME_calib_data.H4 = ((int16_t)BME_calib_data.H451 << 4) | (BME_calib_data.H452 & 0x0F);
  BME_calib_data.H5 = ((int16_t)BME_calib_data.H453 << 4) | (BME_calib_data.H452 >> 4);

  #if BME_INIT_TEMP > 0
  BME_getTemp();
  #endif
}

// Returns temperature in DegC, resolution is 0.01 DegC.
// Output value of “5123” equals 51.23 DegC.
int32_t BME_getTemp(void) {
  I2C_start((BME_ADDR << 1) | 0);
  I2C_write(BME_REG_TEMPDATA);
  I2C_stop();
  I2C_start((BME_ADDR << 1) | 1);
  int32_t adc_T = ((uint32_t)I2C_read(2) << 12) | ((uint32_t)I2C_read(1) << 4) | (I2C_read(0) >> 4);
  I2C_stop();

  int32_t var1, var2;
  var1 = (int32_t)((adc_T >> 3) - ((int32_t)BME_calib_data.T1 << 1));
  var1 = (var1 * ((int32_t)BME_calib_data.T2)) >> 11;
  var2 = (int32_t)((adc_T >> 4) - ((int32_t)BME_calib_data.T1));
  var2 = (((var2 * var2) >> 12) * ((int32_t)BME_calib_data.T3)) >> 14;
  BME_t_fine = var1 + var2 + BME_t_fine_adjust;
  return(BME_t_fine * 5 + 128) >> 8;
}

// Returns pressure in Pa as unsigned 32 bit integer.
// Output value of “96386” equals 96386 Pa = 963.86 hPa.
uint32_t BME_getPressure(void) {
  I2C_start((BME_ADDR << 1) | 0);
  I2C_write(BME_REG_PRESSUREDATA);
  I2C_stop();
  I2C_start((BME_ADDR << 1) | 1);
  int32_t adc_P = ((uint32_t)I2C_read(3) << 24) | ((uint32_t)I2C_read(2) << 16) 
                | ((uint32_t)I2C_read(1) <<  8) | ((uint32_t)I2C_read(0));
  I2C_stop();

  int32_t var1, var2;
  uint32_t p;
  var1 = (BME_t_fine >> 1) - 64000;
  var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t)BME_calib_data.P6;
  var2 = var2 + ((var1 * ((int32_t)BME_calib_data.P5)) << 1);
  var2 = (var2 >> 2) + (((int32_t)BME_calib_data.P4) << 16);
  var1 = (((BME_calib_data.P3 * (((var1 >> 2) * (var1 >> 2)) >> 13 )) >> 3) 
         + ((((int32_t)BME_calib_data.P2) * var1) >> 1)) >> 18;
  var1 = ((((32768 + var1)) * ((int32_t)((uint16_t)BME_calib_data.P1))) >> 15);
  if(var1 == 0) return 0;
  p = (((uint32_t)(((int32_t)1048576) - adc_P) - (var2 >> 12))) * 3125;
  if(p < 0x80000000) p = (p << 1) / ((uint32_t)var1);
  else p = (p / (uint32_t)var1) * 2;
  var1 = (((int32_t)BME_calib_data.P9) * ((int32_t)(((p >> 3) * (p >> 3)) >> 13))) >> 12;
  var2 = (((int32_t)(p >> 2)) * ((int32_t)BME_calib_data.P8)) >> 13;
  p = (uint32_t)((int32_t)p + ((var1 + var2 + BME_calib_data.P7) >> 4));
  return p;
}

// Humidity in %RH, resolution is 0.01%RH.
// Output value of “4653” represents 46.53 %RH.
uint32_t BME_getHumidity(void) {
  I2C_start((BME_ADDR << 1) | 0);
  I2C_write(BME_REG_HUMIDDATA);
  I2C_stop();
  I2C_start((BME_ADDR << 1) | 1);
  int32_t adc_H = ((uint16_t)I2C_read(1) << 8) | I2C_read(0);
  I2C_stop();

  int32_t var1; 
  var1 = BME_t_fine - 76800;
  var1 = (((((adc_H << 14) - (((int32_t)BME_calib_data.H4) << 20) - (((int32_t)BME_calib_data.H5) * var1))
         + ((int32_t)16384)) >> 15) * (((((((var1 * ((int32_t)BME_calib_data.H6)) >> 10)
         * (((var1 * ((int32_t)BME_calib_data.H3)) >> 11) + ((int32_t)32768))) >> 10)
         + ((int32_t)2097152)) * ((int32_t)BME_calib_data.H2) + 8192) >> 14));
  var1 = (var1 - (((((var1 >> 15) * (var1 >> 15)) >> 7) * ((int32_t)BME_calib_data.H1)) >> 4));
  var1 = (var1 < 0 ? 0 : var1);
  var1 = (var1 > 419430400 ? 419430400 : var1);
  return((var1 >> 12) * 25) >> 8;
}

// Put device into sleep mode (1: sleep, 0: wake up)
void BME_sleep(uint8_t slp) {
  I2C_start((BME_ADDR << 1) | 0);
  I2C_write(BME_REG_CONTROL);
  I2C_write(slp ? 0 : 0b10010011);
  I2C_stop();
}

// Set user temperature adjustment ("-127" means -1.27 DegC)
void BME_adjustTemp(int32_t adjust) {
  BME_t_fine_adjust = (adjust << 8) / 5;
};
