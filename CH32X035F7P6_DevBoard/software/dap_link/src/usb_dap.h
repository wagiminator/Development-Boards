// ===================================================================================
// CMSIS-DAP v2 USB Transport for CH32X035                                    * v1.0 *
// ===================================================================================
//
// DAP_init()           init DAP I/O pins and USB device
// DAP_usbReady()       check if USB device is configured by the host
// DAP_task()           execute received DAP commands (call in main loop)
// DAP_activity         flag, set when a DAP command was executed
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

extern volatile uint8_t DAP_activity;

void    DAP_init(void);
uint8_t DAP_usbReady(void);
void    DAP_task(void);

// USB handler functions
void    DAP_EP_init(void);
void    DAP_EP1_IN(void);
void    DAP_EP1_OUT(void);
void    DAP_EP2_IN(void);
uint8_t DAP_vendorSetup(void);

#ifdef __cplusplus
}
#endif
