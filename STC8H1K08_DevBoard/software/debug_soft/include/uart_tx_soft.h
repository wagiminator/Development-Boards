// ===================================================================================
// Software-UART (TX only) for STC8H Microcontrollers                         * v1.0 *
// ===================================================================================
//
// Functions available:
// --------------------
// SUART_init()             Init SUART with 8N1 and default BAUD rate
// SUART_write(c)           Send character via UART
//
// 2023 by Stefan Wagner:   https://github.com/wagiminator

#pragma once
#include <stdint.h>
#include "gpio.h"

// ===================================================================================
// SUART Parameters
// ===================================================================================
#define PIN_TXD       P31         // define TX pin here
#define SUART_BAUD    115200      // define BAUD rate here

// ===================================================================================
// SUART Functions and Macros
// ===================================================================================
#define SUART_init()  PIN_bidirect(PIN_TXD);
void SUART_write(uint8_t data);
