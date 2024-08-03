// ===================================================================================
// VDD Demo User Configurations
// ===================================================================================
//
// Description:
// ------------
// Measures 1.2V internal voltage reference (Vref) against Vdd with analog to digital
// converter (ADC), calculates supply voltage in millivolts (mV) and sends value via 
// UART (BAUD: 115200, 8N1, TX: PD5).

#pragma once

// Pin defines
#define PIN_LED   PC0       // define LED pin
