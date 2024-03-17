# STM32C011 F6P6 Development Board
Development board for the STM32C0x1FxP cost-effective 32-bit ARM Cortex M0+ microcontroller with integrated USB-to-serial adapter which can also be used to upload firmware utilizing the factory built-in serial bootloader.

![STM32C011F4P6_DevBoard_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32C011F4P6_DevBoard/documentation/STM32C011F4P6_DevBoard_pic1.jpg)

# Pinout
![STM32C011F4P6_DevBoard_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32C011F4P6_DevBoard/documentation/STM32C011F4P6_DevBoard_pinout.png)

# The STM32C0x1 Family of 32-bit ARM Microcontrollers
## Overview
The STM32C0x1 product line features an Arm® Cortex®-M0+ core and runs up to 48 MHz. Setting the lowest STM32 price point ever, the STM32C0x1 line makes no compromise on design quality and delivers a performance far beyond the existing 8-bit architectures. Compact, affordable, and reliable, the STM32C0x1 line features a fast 12-bit ADC with hardware resolution up to 16-bit, flexible mapping on the DMA channels, timers with advanced control capability, and several communication peripherals, always including 2 UARTs, even on the smallest microcontroller of the STM32C0x1 line. With STM32C0x1, application designers can start with an entry-level 32-bit MCU that leaves room for future product upgrades, thanks to the broad STM32 portfolio and ecosystem. The STM32C0x1 shares the same technological and design platform as the STM32G0, and benefits from a consistent pinout mapping. The portfolio supports from 16 to 32 Kbytes of on-chip flash memory.

## Compatible Microcontrollers in TSSOP-20 Package
 MCU|Flash|SRAM
:---|----:|---:
STM32C011F4P|16KB|6KB
STM32C011F6P|32KB|6KB
STM32C031F4P|16KB|12KB
STM32C031F6P|32KB|12KB

## Features of STM32C0x1FxP
- 32-bit ARM® Cortex®-M0+ CPU
- Maximum 48MHz system main frequency
- Up to 32KB Flash with memory protection
- Up to 12KB SRAM with hardware parity check
- Operating voltage: 2.0V - 3.6V
- Internal 48MHz (±1%) RC oscillator (HSI)
- Internal 32kHz (±5%) RC oscillator (LSI)
- External 4-48MHz crystal oscillator (HSE)
- External 32kHz crystal oscillator (LSE) with calibration
- Multiple low-power modes: Sleep, Stop, Standby, Shutdown
- Power on/down reset
- 18 I/Os, all available as external interrupts, all 5V-tolerant
- 1x 3-channel DMA controller with flexible mapping
- 1x 12-bit ADC, up to 19 channels, 16-bit oversampling, 0V - 3.6V
- 1× 16-bit advanced control timer
- 4× 16-bit general-purpose timer
- 2x watchdog timer (IWDG, WWDG)
- 1x SysTick timer
- 1x real-time clock (RTC) with calendar, alarm and periodic wake-up
- 2x USART interface, one with LIN, IrDA, auto baud and wake-up feature
- 1x I²C interface supporting Fast-mode Plus (1Mbit/s), wake-up feature
- 1x SPI interface, 24Mbit/s, multiplexed with I²S interface
- Hardware CRC-32 calculation unit
- Serial wire debug interface (SWD)
- Factory built-in embedded UART bootloader

# Compiling, Programming and Debugging
## GCC Toolchain
For compiling install the [ARM GNU toolchain](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads). On Linux (Debian-based), this can be done with the following command:
```
sudo apt install build-essential gcc-arm-none-eabi
```

## Factory built-in UART Bootlader
The MCU has an embedded bootloader with UART interface, which can be used to upload firmware using the CH340E USB-to-serial adapter integrated into the board. The Python tool [stm32isp.py](https://github.com/wagiminator/MCU-Flash-Tools) included with the example software can be used for this purpose. In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [PySerial](https://github.com/pyserial/pyserial) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
python3 -m pip install pyserial
```

To upload firmware via the USB port, the MCU must first be put into boot mode using ONE of the following methods:
- Disconnect your board from all power supplies, press and hold the BOOT button, then connect the board to your USB port. The BOOT button can be released now.
- Connect your board to your USB port. Press and hold the BOOT button, then press and release the RESET button and then release the BOOT button.

On STM32C0x1 microcontrollers, the BOOT0 pin is initially disabled. When the chip is brand new or the main flash memory is erased, this isn't an issue as the embedded bootloader automatically kicks in. By using the stm32isp.py tool, the BOOT0 pin will then be activated for subsequent use. However, if the chip has been previously programmed using a different software tool, the bootloader might not be accessible through the BOOT0 pin (or BOOT button) anymore. In such cases, the nBOOT_SEL bit in the User Option Bytes must be cleared (set to 0) using an SWD programmer like ST-Link and the appropriate software.

## Serial Wire Debug Interface (SWD)
The MCU can also be programmed via the SWD interface with corresponding hardware (ST-Link, DAPLink) and software (STM32CubeProg, OpenOCD, PyOCD, J-Link).

## Arduino and PlatformIO Support
You can use [STM32duino](https://github.com/stm32duino) as an Arduino IDE core for several STM32 microcontrollers. There is also support for [PlatformIO](https://stm32-base.org/guides/platformio.html).

# References, Links and Notes
- [EasyEDA Design Files](https://oshwlab.com/wagiminator)
- [STM Product Page](https://www.st.com/en/microcontrollers-microprocessors/stm32c0x1.html)
- [MCU Templates](https://github.com/wagiminator/MCU-Templates)
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [Arduino Support](https://github.com/stm32duino)
- [PlatformIO Support](https://stm32-base.org/guides/platformio.html)
- [ST-Link Software](https://www.st.com/en/development-tools/st-link-v2.html#tools-software)

![STM32C011F4P6_DevBoard_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32C011F4P6_DevBoard/documentation/STM32C011F4P6_DevBoard_pic2.jpg)
![STM32C011F4P6_DevBoard_pic3.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32C011F4P6_DevBoard/documentation/STM32C011F4P6_DevBoard_pic3.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
