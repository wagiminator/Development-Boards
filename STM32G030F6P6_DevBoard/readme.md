# STM32G030 F6P6 Development Board
Development board for the STM32G030F6P6 cost-effective 32-bit ARM Cortex M0+ microcontroller with integrated USB-to-serial adapter which can also be used to upload firmware utilizing the factory built-in serial bootloader.

![STM32G030F6P6_DevBoard_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32G030F6P6_DevBoard/documentation/STM32G030F6P6_DevBoard_pic1.jpg)

# Pinout
![STM32G030F6P6_DevBoard_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32G030F6P6_DevBoard/documentation/STM32G030F6P6_DevBoard_pinout.png)

# The STM32G030 Family of 32-bit ARM Microcontrollers
## Overview
The STM32G030 mainstream microcontrollers are based on high-performance Arm® Cortex®-M0+ 32-bit RISC core operating at up to 64 MHz frequency. The devices incorporate a memory protection unit (MPU), high-speed embedded memories (8 Kbytes of SRAM and up to 64 Kbytes of Flash program memory with read protection, write protection), DMA, an extensive range of system functions, enhanced I/Os, and peripherals. The devices offer standard communication interfaces (two I2Cs, two SPIs / one I2S, and two USARTs), one 12-bit ADC (2.5 MSps) with up to 19 channels, a low-power RTC, an advanced control PWM timer, four general-purpose 16-bit timers, two watchdog timers, and a SysTick timer. The devices operate with supply voltages from 2.0 V to 3.6 V. Optimized dynamic consumption combined with a comprehensive set of power-saving modes allows the design of low-power applications. The devices come in packages with 8 to 48 pins.

## Features of STM32G030F6P6
- 32-bit ARM® Cortex®-M0+ CPU
- Maximum 64MHz system main frequency
- 8KB SRAM with hardware parity check
- 32KB Flash with memory protection
- Operating voltage: 2.0V - 3.6V
- Internal 16MHz (±1%) RC oscillator (HSI) with PLL option
- Internal 32kHz (±5%) RC oscillator (LSI)
- External 4-48MHz crystal oscillator (HSE)
- Multiple low-power modes: Sleep, Stop, Standby
- Power on/down reset
- 18 I/Os, all available as external interrupts, all 5V-tolerant
- 1x 5-channel DMA controller with flexible mapping
- 1x 12-bit ADC, up to 16 channels, 16-bit oversampling, 0V - 3.6V
- 1× 16-bit advanced motor control timer
- 4× 16-bit general-purpose timer
- 2x watchdog timer
- 1x SysTick timer
- 1x real-time clock (RTC) with calendar, alarm and periodic wakeup
- 2x USART interface, one with LIN, IrDA, auto baud and wakeup feature
- 2x I²C interface supporting Fast-mode Plus (1Mbit/s), wakeup feature
- 2x SPI interface, 32Mbit/s, one multiplexed with I²S interface
- Hardware CRC-32 calculation unit
- Serial wire debug interface (SWD)
- Factory built-in embedded UART bootloader

## Unofficial Additional Features
The STM32G030 obviously has the same core as its larger sibling, [STM32G031](https://dannyelectronics.wordpress.com/2023/08/23/is-stm32g030-really-a-stm32g031/) and partly [STM32G041](https://dannyelectronics.wordpress.com/2023/08/23/are-stm32g030-031-really-the-same-as-stm32g041/). Therefore, the following additional peripherals and capabilities are also available:
- 1x 32-bit general-purpose timer
- 2x 16-bit low-power timer
- 1x low-power UART interface
- True random number generator (RNG)
- Programmable brownout reset (BOD)
- Programmable voltage detector (PVD)

While it cannot be guaranteed that this applies to all STM32G030 devices, at least in the case of mine, these features work very well.

# Compiling, Programming and Debugging
## GCC Toolchain
For compiling install the [ARM GNU toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). On Linux (Debian-based), this can be done with the following command:
```
sudo apt install build-essential gcc-arm-none-eabi
```

## Factory built-in UART Bootlader
The MCU has an embedded bootloader with UART interface, which can be used to upload firmware using the CH340E USB-to-serial adapter integrated into the board. The open-source platform-independent Python tool [stm32isp](https://pypi.org/project/stm32isp/) can be used for this purpose. In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
pip install stm32isp
```

To upload firmware via the USB port, the MCU must first be put into boot mode using ONE of the following methods:
- Disconnect your board from all power supplies, press and hold the BOOT button, then connect the board to your USB port. The BOOT button can be released now.
- Connect your board to your USB port. Press and hold the BOOT button, then press and release the RESET button and then release the BOOT button.

Then run the following command to upload your firmware (example):

```
stm32isp -f firmware.bin
```

On STM32G030 microcontrollers, the BOOT0 pin is initially disabled. When the chip is brand new or the main flash memory is erased, this isn't an issue as the embedded bootloader automatically kicks in. By using the stm32isp.py tool, the BOOT0 pin will then be activated for subsequent use. However, if the chip has been previously programmed using a different software tool, the bootloader might not be accessible through the BOOT0 pin (or BOOT button) anymore. In such cases, the nBOOT_SEL bit in the User Option Bytes must be cleared (set to 0) using an SWD programmer like ST-Link and the appropriate software.

## Serial Wire Debug Interface (SWD)
The MCU can also be programmed via the SWD interface with corresponding hardware (ST-Link, DAPLink) and software (STM32CubeProg, OpenOCD, PyOCD, J-Link).

In order to use [PyOCD](https://pyocd.io/) for uploading and debugging, install it via

```
pip install pyocd
```

Then install the Keil.STM32G0xx_DFP pack:

```
pyocd pack install STM32G030F6Px
```

In order to list all supported MCUs of the PY32F0xx series, run:

```
pyocd pack find stm32g0
```

Connect an SWD debug probe (e.g. [ST-Link V2](https://aliexpress.com/w/wholesale-st%2525252dlink-v2.html)) to your board. Upload firmware with the following command (example):

```
pyocd load firmware.bin -t stm32g030f6px
```

If you want to erase the chip, run:

```
pyocd erase -t stm32g030f6px --chip
```

## Arduino and PlatformIO Support
You can use [STM32duino](https://github.com/stm32duino) as an Arduino IDE core for several STM32 microcontrollers. There is also support for [PlatformIO](https://stm32-base.org/guides/platformio.html).

# References, Links and Notes
- [EasyEDA Design Files](https://oshwlab.com/wagiminator)
- [STM Product Page](https://www.st.com/en/microcontrollers-microprocessors/stm32g030f6.html)
- [MCU Templates](https://github.com/wagiminator/MCU-Templates)
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [Arduino Support](https://github.com/stm32duino)
- [PlatformIO Support](https://stm32-base.org/guides/platformio.html)
- [ST-Link Software](https://www.st.com/en/development-tools/st-link-v2.html#tools-software)

![STM32G030F6P6_DevBoard_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32G030F6P6_DevBoard/documentation/STM32G030F6P6_DevBoard_pic2.jpg)
![STM32G030F6P6_DevBoard_pic3.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32G030F6P6_DevBoard/documentation/STM32G030F6P6_DevBoard_pic3.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
