# PY32F002A F15P Development Board
Development board for the PY32F002AF15P ultra-cheap 32-bit ARM Cortex microcontroller with integrated USB-to-serial adapter which can also be used to upload firmware utilizing the factory built-in serial bootloader.

![PY32F002AF15P_DevBoard_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/PY32F002AF15P_DevBoard/documentation/PY32F002AF15P_DevBoard_pic1.jpg)

# Pinout
![PY32F002AF15P_DevBoard_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/PY32F002AF15P_DevBoard/documentation/PY32F002AF15P_DevBoard_pinout.png)

# The PY32F002A Family of 32-bit ARM Microcontrollers
## Overview
The PY32F002A series microcontrollers are cost-effective MCUs with a high performance 32-bit ARM® Cortex®-M0+ core. They feature a wide voltage operating range (1.7V - 5.5V), 20 Kbytes flash, 3 Kbytes SRAM memory, and a maximum operating frequency of 24 MHz. The chip integrates multi-channel I2C, SPI, USART, and other communication peripherals, one channel 12-bit ADC, two 16-bit timers, and two-channel comparators. The PY32F002A series contains various products in different package types.

## Block Diagram
![PY32F002AF15P_DevBoard_block.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/PY32F002AF15P_DevBoard/documentation/PY32F002AF15P_DevBoard_block.png)

## Features
- 32-bit ARM® Cortex®-M0+ CPU
- Maximum 24MHz system main frequency
- 3KB SRAM, 20KB Flash
- Operating voltage: 1.7V - 5.5V
- Internal 8/24MHz RC Oscillator (HSI)
- Internal 32.768kHz RC oscillator (LSI)
- External 4-24MHz crystal oscillator (HSE)
- Multiple low-power modes: Sleep, Stop
- Power on/off reset, programmable brownout detector
- Up to 18 I/Os, all available as external interrupts, 8mA drive current
- 1x 12-bit ADC, 2x comparators
- 1× 16-bit advanced control timer, 1× 16-bit general purpose timer
- 1x low-power timer, supports wake-up from stop mode
- 1x independend watchdog tmer, 1× SysTick timer
- 1x USART interface, 1x I2C interface, 1x SPI interface
- 18 I/O ports, mapping an external interrupt
- Hardware CRC-32 module
- 128-bit chip unique ID
- Serial wire debug interface (SWD)
- Factory built-in UART bootloader

## Unofficial Additional Features
The PY32F002A obviously has the same core as its larger sibling, PY32F030. Therefore, the following additional peripherals and capabilities are also available:
- Maximum 48MHz system frequency via PLL
- 4KB SRAM, 32KB flash
- 3-channel DMA controller
- 3x additional 16-bit general purpose timer
- 1x window watchdog timer
- 1x real-time clock (RTC)
- 1x infrared interface
- 1x additional USART, 1x additional SPI interface

While it cannot be guaranteed that this applies to all PY32F002A devices, at least in the case of mine, these features work very well.

# Compiling, Programming and Debugging
## GCC Toolchain
For compiling install the [ARM GNU toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). On Linux (Debian-based), this can be done with the following command:
```
sudo apt install build-essential gcc-arm-none-eabi
```

## Factory built-in UART Bootlader
The MCU has an embedded bootloader with UART interface, which can be used to upload firmware using the CH340E USB-to-serial adapter integrated into the board. The open-source platform-independent Python tool [puyaisp](https://pypi.org/project/puyaisp/) can be used for this purpose. In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
pip install puyaisp
```

To upload firmware via the USB port, the MCU must first be put into boot mode using ONE of the following methods:
- Disconnect your board from all power supplies, press and hold the BOOT button, then connect the board to your USB port. The BOOT button can be released now.
- Connect your board to your USB port. Press and hold the BOOT button, then press and release the RESET button and then release the BOOT button.

Then run the following command to upload your firmware (example):

```
puyaisp -f firmware.bin
```

## Serial Wire Debug Interface (SWD)
The MCU can also be programmed via the SWD interface and corresponding software (OpenOCD, PyOCD, J-Link), if necessary with modifications. Further information on this can be found [here](https://github.com/IOsetting/py32f0-template), among others.

In order to use [PyOCD](https://pyocd.io/) for uploading and debugging, install it via

```
pip install pyocd
```

Then install the Puya.PY32F0xx_DFP pack:

```
pyocd pack install PY32F002Ax5
```

In order to list all supported MCUs of the PY32F0xx series, run:

```
pyocd pack find py32f0
```

Connect an SWD debug probe (e.g. [ST-Link V2](https://aliexpress.com/w/wholesale-st%2525252dlink-v2.html)) to your board. Upload firmware with the following command (example):

```
pyocd load firmware.bin -t py32f002ax5
```

If you want to erase the chip, run:

```
pyocd erase -t py32f002ax5 --chip
```

## Arduino IDE Support
An Arduino core for the PY32F0 series can be found [here](https://github.com/py32duino/Arduino-PY32).

# References, Links and Notes
- [EasyEDA Design Files](https://oshwlab.com/wagiminator)
- [PUYA Product Page](https://www.puyasemi.com/py32_xilie.html)
- [MCU Templates](https://github.com/wagiminator/MCU-Templates)
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [Arduino Support](https://github.com/py32duino/Arduino-PY32)
- [IOsetting PY32F0 Template](https://github.com/IOsetting/py32f0-template)
- [Jay Carlson's Article](https://jaycarlson.net/2023/02/04/the-cheapest-flash-microcontroller-you-can-buy-is-actually-an-arm-cortex-m0/)
- [EEVblog PY32F002A](https://www.eevblog.com/forum/microcontrollers/$0-11-py32f002a-m0-24mhz-320kb-actually-324kb-more-peripherals/)

![PY32F002AF15P_DevBoard_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/PY32F002AF15P_DevBoard/documentation/PY32F002AF15P_DevBoard_pic2.jpg)
![PY32F002AF15P_DevBoard_pic5.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/PY32F002AF15P_DevBoard/documentation/PY32F002AF15P_DevBoard_pic5.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
