// ===================================================================================
// CMSIS-DAP Configuration for CH32X035 (Port of ARM DAP_config.h Template)
// ===================================================================================
//
// Hardware I/O pin access, LEDs, timestamp and setup of the debug unit for the
// CMSIS-DAP firmware sources of ARM (cmsis_dap/DAP.c, SW_DP.c, JTAG_DP.c).
// Pins and options are defined in config.h.
//
// Based on: CMSIS-DAP DAP_config.h template, Copyright (c) 2013-2021 ARM Limited,
// SPDX-License-Identifier: Apache-2.0
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#ifndef __DAP_CONFIG_H__
#define __DAP_CONFIG_H__

#include <stdint.h>
#include "config.h"
#include "system.h"
#include "gpio.h"
#include "cmsis_compiler.h"

// ===================================================================================
// Debug Unit Information
// ===================================================================================
#define CPU_CLOCK               F_CPU     // CPU clock in Hz
#define IO_PORT_WRITE_CYCLES    2U        // I/O cycles for a GPIO write
#define DELAY_SLOW_CYCLES       3U        // cycles of one iteration of PIN_DELAY_SLOW
#define DELAY_FAST_CYCLES       0U        // number of NOPs in PIN_DELAY_FAST

#define DAP_SWD                 1         // SWD mode available
#define DAP_JTAG                1         // JTAG mode available
// DAP_JTAG_DEV_CNT, DAP_DEFAULT_PORT, DAP_PACKET_SIZE, DAP_PACKET_COUNT: config.h
#define DAP_DEFAULT_SWJ_CLOCK   DAP_DEFAULT_CLOCK

// SWO trace (UART/NRZ mode via USART RX with DMA, streaming via bulk endpoint)
#if DAP_SWO_ENABLE > 0
#define SWO_UART                1
#define SWO_UART_MAX_BAUDRATE   (F_CPU / 16U)
#define SWO_STREAM              1
#else
#define SWO_UART                0
#define SWO_STREAM              0
#endif
#define SWO_MANCHESTER          0
#define SWO_BUFFER_SIZE         SWO_BUF_SIZE

// Timestamps (SysTick counter at F_CPU)
#define TIMESTAMP_CLOCK         F_CPU

// UART communication port
#if DAP_UART_ENABLE > 0
#define DAP_UART                1
#define DAP_UART_USB_COM_PORT   UART_COM_PORT
#else
#define DAP_UART                0
#define DAP_UART_USB_COM_PORT   0
#endif
#define DAP_UART_RX_BUFFER_SIZE UART_RX_BUF_SIZE
#define DAP_UART_TX_BUFFER_SIZE UART_TX_BUF_SIZE

// Target device information
#define TARGET_FIXED            0

#if (DAP_PACKET_COUNT & (DAP_PACKET_COUNT - 1)) || (DAP_PACKET_COUNT > 128)
  #error DAP_PACKET_COUNT must be a power of 2 (1..128)!
#endif
#if (DAP_PACKET_SIZE < 64) || (DAP_PACKET_SIZE > 1024)
  #error DAP_PACKET_SIZE must be 64..1024!
#endif
#if ((SWO_BUF_SIZE & (SWO_BUF_SIZE - 1)) || (UART_RX_BUF_SIZE & (UART_RX_BUF_SIZE - 1)) \
    || (UART_TX_BUF_SIZE & (UART_TX_BUF_SIZE - 1)))
  #error SWO and UART buffer sizes must be a power of 2!
#endif
#if (DAP_SWO_ENABLE > 0) && (DAP_UART_ENABLE > 0) && (SWO_USART == UART_USART)
  #error SWO and UART must use different USARTs!
#endif

// ===================================================================================
// Debug Unit Strings
// ===================================================================================
// Copy string, return length including terminating NULL character
__STATIC_INLINE uint8_t DAP_copyString(char *str, const char *src) {
  uint8_t len = 0;
  while((str[len] = src[len])) len++;
  return len + 1;
}

// Vendor and product name (USB strings are used by the host for CMSIS-DAP v2)
__STATIC_INLINE uint8_t DAP_GetVendorString(char *str) {
  return DAP_copyString(str, MANUF_STR);
}

__STATIC_INLINE uint8_t DAP_GetProductString(char *str) {
  return DAP_copyString(str, PROD_STR);
}

// Serial number (same as USB serial number string)
extern uint8_t USB_getSerialString(char *str);
__STATIC_INLINE uint8_t DAP_GetSerNumString(char *str) {
  return USB_getSerialString(str);
}

__STATIC_INLINE uint8_t DAP_GetTargetDeviceVendorString(char *str) {(void)str; return 0;}
__STATIC_INLINE uint8_t DAP_GetTargetDeviceNameString  (char *str) {(void)str; return 0;}
__STATIC_INLINE uint8_t DAP_GetTargetBoardVendorString (char *str) {(void)str; return 0;}
__STATIC_INLINE uint8_t DAP_GetTargetBoardNameString   (char *str) {(void)str; return 0;}

__STATIC_INLINE uint8_t DAP_GetProductFirmwareVersionString(char *str) {
  return DAP_copyString(str, DAP_PRODUCT_FW_VER);
}

// ===================================================================================
// GPIO Helper Functions (pins are compile-time constants, the compiler reduces each
// function to one or two register accesses)
// ===================================================================================
#ifndef DAP_HOST_TEST
#define DAP_GPIO(p)     ((p) < PB0 ? GPIOA : ((p) < PC0 ? GPIOB : GPIOC))
#define DAP_BIT(p)      ((p) & 31)

// GPIO configuration nibbles
#define DAP_CFG_IN_FL   0x4             // input floating
#define DAP_CFG_IN_PU   0x8             // input with pull-up/-down (ODR selects)
#define DAP_CFG_OUT_PP  0x3             // output push-pull
#define DAP_CFG_OUT_OD  0x7             // output open-drain
#define DAP_CFG_AF_PP   0xB             // alternate function output push-pull

__STATIC_FORCEINLINE void DAP_pinConfig(uint32_t pin, uint32_t cfg) {
  GPIO_TypeDef* gpio = DAP_GPIO(pin);
  uint32_t bit   = DAP_BIT(pin);
  uint32_t shift = (bit & 7) << 2;
  __IO uint32_t* reg = (bit < 8) ? &gpio->CFGLR : ((bit < 16) ? &gpio->CFGHR : &gpio->CFGXR);
  *reg = (*reg & ~((uint32_t)0xF << shift)) | (cfg << shift);
}

__STATIC_FORCEINLINE void DAP_pinHigh(uint32_t pin) {
  uint32_t bit = DAP_BIT(pin);
  if(bit < 16) DAP_GPIO(pin)->BSHR = (uint32_t)1 << bit;
  else         DAP_GPIO(pin)->BSXR = (uint32_t)1 << (bit - 16);
}

__STATIC_FORCEINLINE void DAP_pinLow(uint32_t pin) {
  uint32_t bit = DAP_BIT(pin);
  if(bit < 16) DAP_GPIO(pin)->BCR  = (uint32_t)1 << bit;
  else         DAP_GPIO(pin)->BSXR = (uint32_t)1 << bit;
}

// Write pin with one register access (bit: 0 or 1)
__STATIC_FORCEINLINE void DAP_pinWrite(uint32_t pin, uint32_t val) {
  uint32_t bit = DAP_BIT(pin);
  if(bit < 16) DAP_GPIO(pin)->BSHR = ((uint32_t)1 << bit) << ((~val & 1) << 4);
  else         DAP_GPIO(pin)->BSXR = ((uint32_t)1 << (bit - 16)) << ((~val & 1) << 4);
}

__STATIC_FORCEINLINE uint32_t DAP_pinRead(uint32_t pin) {
  return (DAP_GPIO(pin)->INDR >> DAP_BIT(pin)) & 1;
}

#else
#include "dap_gpio_mock.h"        // GPIO model for host tests
#endif

// Input with pull-up
__STATIC_FORCEINLINE void DAP_pinInputPU(uint32_t pin) {
  DAP_pinConfig(pin, DAP_CFG_IN_PU);
  DAP_pinHigh(pin);
}

// ===================================================================================
// Configure DAP I/O Pins
// ===================================================================================

// JTAG mode: TCK, TMS, TDI, nTRST, nRESET outputs (high), TDO input
__STATIC_INLINE void PORT_JTAG_SETUP(void) {
  DAP_pinHigh(PIN_SWCLK_TCK);   DAP_pinConfig(PIN_SWCLK_TCK, DAP_CFG_OUT_PP);
  DAP_pinHigh(PIN_SWDIO_TMS);   DAP_pinConfig(PIN_SWDIO_TMS, DAP_CFG_OUT_PP);
  DAP_pinHigh(PIN_TDI);         DAP_pinConfig(PIN_TDI,       DAP_CFG_OUT_PP);
  DAP_pinInputPU(PIN_TDO);
  #ifdef PIN_nTRST
  DAP_pinHigh(PIN_nTRST);       DAP_pinConfig(PIN_nTRST,     DAP_CFG_OUT_OD);
  #endif
  #ifdef PIN_nRESET
  DAP_pinHigh(PIN_nRESET);      DAP_pinConfig(PIN_nRESET,    DAP_CFG_OUT_OD);
  #endif
}

// SWD mode: SWCLK, SWDIO, nRESET outputs (high), TDI, nTRST high impedance
__STATIC_INLINE void PORT_SWD_SETUP(void) {
  DAP_pinHigh(PIN_SWCLK_TCK);   DAP_pinConfig(PIN_SWCLK_TCK, DAP_CFG_OUT_PP);
  DAP_pinHigh(PIN_SWDIO_TMS);   DAP_pinConfig(PIN_SWDIO_TMS, DAP_CFG_OUT_PP);
  DAP_pinConfig(PIN_TDI, DAP_CFG_IN_FL);
  DAP_pinInputPU(PIN_TDO);
  #ifdef PIN_nTRST
  DAP_pinConfig(PIN_nTRST, DAP_CFG_IN_FL);
  #endif
  #ifdef PIN_nRESET
  DAP_pinHigh(PIN_nRESET);      DAP_pinConfig(PIN_nRESET,    DAP_CFG_OUT_OD);
  #endif
}

// Disable DAP I/O pins (high impedance)
__STATIC_INLINE void PORT_OFF(void) {
  DAP_pinConfig(PIN_SWCLK_TCK, DAP_CFG_IN_FL);
  DAP_pinConfig(PIN_SWDIO_TMS, DAP_CFG_IN_FL);
  DAP_pinConfig(PIN_TDI,       DAP_CFG_IN_FL);
  DAP_pinConfig(PIN_TDO,       DAP_CFG_IN_FL);
  #ifdef PIN_nTRST
  DAP_pinConfig(PIN_nTRST,     DAP_CFG_IN_FL);
  #endif
  #ifdef PIN_nRESET
  DAP_pinConfig(PIN_nRESET,    DAP_CFG_IN_FL);
  #endif
}

// ===================================================================================
// SWCLK/TCK and SWDIO/TMS Pins
// ===================================================================================
__STATIC_FORCEINLINE uint32_t PIN_SWCLK_TCK_IN (void) { return DAP_pinRead(PIN_SWCLK_TCK); }
__STATIC_FORCEINLINE void     PIN_SWCLK_TCK_SET(void) { DAP_pinHigh(PIN_SWCLK_TCK); }
__STATIC_FORCEINLINE void     PIN_SWCLK_TCK_CLR(void) { DAP_pinLow(PIN_SWCLK_TCK); }

__STATIC_FORCEINLINE uint32_t PIN_SWDIO_TMS_IN (void) { return DAP_pinRead(PIN_SWDIO_TMS); }
__STATIC_FORCEINLINE void     PIN_SWDIO_TMS_SET(void) { DAP_pinHigh(PIN_SWDIO_TMS); }
__STATIC_FORCEINLINE void     PIN_SWDIO_TMS_CLR(void) { DAP_pinLow(PIN_SWDIO_TMS); }

__STATIC_FORCEINLINE uint32_t PIN_SWDIO_IN     (void) { return DAP_pinRead(PIN_SWDIO_TMS); }
__STATIC_FORCEINLINE void     PIN_SWDIO_OUT    (uint32_t bit) { DAP_pinWrite(PIN_SWDIO_TMS, bit & 1); }

// SWDIO output mode (output level stays high from input pull-up)
__STATIC_FORCEINLINE void PIN_SWDIO_OUT_ENABLE(void) {
  DAP_pinConfig(PIN_SWDIO_TMS, DAP_CFG_OUT_PP);
}

// SWDIO input mode with pull-up (line is read as high if not driven)
__STATIC_FORCEINLINE void PIN_SWDIO_OUT_DISABLE(void) {
  DAP_pinConfig(PIN_SWDIO_TMS, DAP_CFG_IN_PU);
  DAP_pinHigh(PIN_SWDIO_TMS);
}

// ===================================================================================
// TDI, TDO, nTRST and nRESET Pins
// ===================================================================================
__STATIC_FORCEINLINE uint32_t PIN_TDI_IN (void) { return DAP_pinRead(PIN_TDI); }
__STATIC_FORCEINLINE void     PIN_TDI_OUT(uint32_t bit) { DAP_pinWrite(PIN_TDI, bit & 1); }
__STATIC_FORCEINLINE uint32_t PIN_TDO_IN (void) { return DAP_pinRead(PIN_TDO); }

__STATIC_FORCEINLINE uint32_t PIN_nTRST_IN(void) {
  #ifdef PIN_nTRST
  return DAP_pinRead(PIN_nTRST);
  #else
  return 1;
  #endif
}

// nTRST/nRESET are open-drain outputs (also when the debug port is disabled)
__STATIC_FORCEINLINE void PIN_nTRST_OUT(uint32_t bit) {
  #ifdef PIN_nTRST
  DAP_pinWrite(PIN_nTRST, bit & 1);
  DAP_pinConfig(PIN_nTRST, DAP_CFG_OUT_OD);
  #else
  (void)bit;
  #endif
}

__STATIC_FORCEINLINE uint32_t PIN_nRESET_IN(void) {
  #ifdef PIN_nRESET
  return DAP_pinRead(PIN_nRESET);
  #else
  return 1;
  #endif
}

__STATIC_FORCEINLINE void PIN_nRESET_OUT(uint32_t bit) {
  #ifdef PIN_nRESET
  DAP_pinWrite(PIN_nRESET, bit & 1);
  DAP_pinConfig(PIN_nRESET, DAP_CFG_OUT_OD);
  #else
  (void)bit;
  #endif
}

// ===================================================================================
// Status LEDs
// ===================================================================================
extern volatile uint8_t DAP_LED_connected;
extern volatile uint8_t DAP_LED_running;

__STATIC_INLINE void LED_CONNECTED_OUT(uint32_t bit) {
  DAP_LED_connected = bit & 1;
  #ifdef PIN_LED_CONNECTED
  DAP_pinWrite(PIN_LED_CONNECTED, (bit & 1) ? LED_ACTIVE_LEVEL : !LED_ACTIVE_LEVEL);
  #endif
}

__STATIC_INLINE void LED_RUNNING_OUT(uint32_t bit) {
  DAP_LED_running = bit & 1;
  #ifdef PIN_LED_RUNNING
  DAP_pinWrite(PIN_LED_RUNNING, (bit & 1) ? LED_ACTIVE_LEVEL : !LED_ACTIVE_LEVEL);
  #endif
}

// ===================================================================================
// Timestamp (SysTick counter)
// ===================================================================================
__STATIC_INLINE uint32_t TIMESTAMP_GET(void) {
  return STK->CNTL;
}

// ===================================================================================
// Setup of the Debug Unit I/O Pins and LEDs
// ===================================================================================
__STATIC_INLINE void DAP_SETUP(void) {
  RCC->APB2PCENR |= RCC_IOPAEN | RCC_IOPBEN | RCC_IOPCEN | RCC_AFIOEN;
  PORT_OFF();
  #ifdef PIN_nRESET
  DAP_pinInputPU(PIN_nRESET);
  #endif
  #ifdef PIN_nTRST
  DAP_pinInputPU(PIN_nTRST);
  #endif
  #ifdef PIN_LED_CONNECTED
  DAP_pinWrite(PIN_LED_CONNECTED, !LED_ACTIVE_LEVEL);
  DAP_pinConfig(PIN_LED_CONNECTED, DAP_CFG_OUT_PP);
  #endif
  #ifdef PIN_LED_RUNNING
  DAP_pinWrite(PIN_LED_RUNNING, !LED_ACTIVE_LEVEL);
  DAP_pinConfig(PIN_LED_RUNNING, DAP_CFG_OUT_PP);
  #endif
}

// Device specific reset sequence (not implemented, nRESET pin is used)
__STATIC_INLINE uint8_t RESET_TARGET(void) {
  return 0;
}

#endif // __DAP_CONFIG_H__
