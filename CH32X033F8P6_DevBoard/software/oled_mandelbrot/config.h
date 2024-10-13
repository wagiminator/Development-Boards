// ===================================================================================
// User Configurations for OLED Mandelbrot Demo
// ===================================================================================
//
// Description:
// ------------
// The Mandelbrot set is a complex set of points that forms a fractal, named after
// mathematician Benoît Mandelbrot. It consists of all complex numbers c for which
// the function z_(n+1) = z_(n)^2 + c does not diverge when iterated from z_(0) = 0.
// The set is typically visualized in the complex plane, displaying intricate,
// self-similar patterns. It exhibits a distinctive shape with a large central "bulb"
// and smaller surrounding "satellite" bulbs, revealing more detailed structures at
// increasing magnification. The Mandelbrot set is an iconic example of how simple
// mathematical rules can generate incredibly complex and beautiful patterns.
//
// Connect an SSD1306 128x64 Pixels I2C OLED to the MCU pins defined below.

#pragma once

// Pin definitions
#define PIN_SCL             PA10      // pin connected to serial clock of the I2C bus
#define PIN_SDA             PA11      // pin connected to serial data of the I2C bus
