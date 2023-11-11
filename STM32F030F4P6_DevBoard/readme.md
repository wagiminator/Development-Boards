# STM32F030 F4P6 Development Board
Development board for the STM32F030F4P6 cost-effective 32-bit ARM Cortex M0 microcontroller with integrated USB-to-serial adapter which can also be used to upload firmware utilizing the factory built-in serial bootloader. Due to the comparable pinouts, this board can also be used with other microcontrollers from the STM32F0 and STM32L0 series that come in the same TSSOP20 package.

![STM32F030F4P6_DevBoard_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32F030F4P6_DevBoard/documentation/STM32F030F4P6_DevBoard_pic1.jpg)

# Pinout
![STM32F030F4P6_DevBoard_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32F030F4P6_DevBoard/documentation/STM32F030F4P6_DevBoard_pinout.png)

# The STM32F030 Family of 32-bit ARM Microcontrollers
## Overview
The STM32F030 microcontrollers feature an Arm® Cortex®-M0 core and run at speeds up to 48 MHz. While achieving the STM32’s lowest ever price point, the STM32F030 line comes with a full set of performing peripherals, such as fast 12-bit ADC, advanced and flexible timers, calendar RTC and communication peripherals such as the I²C, USART and SPI. This combination easily outperforms existing 8-bit architectures and allows all application designers to benefit from the simplicity and efficiency of a state-of-the-art 32-bit core. The STM32F030 line covers many memory and pin count combinations with only few competitive devices and thus further enhances the overall cost efficiency of your projects. Application designers starting with the STM32F030 benefit from being able to upgrade to any of the devices in the powerful STM32 portfolio at any time with an extraordinary degree of reusability of tools and application software.

## Features of STM32F030F4P6
- 32-bit ARM® Cortex®-M0 CPU
- Maximum 48MHz system main frequency
- 4KB SRAM with hardware parity check
- 16KB Flash
- Operating voltage: 2.4V - 3.6V
- Internal 8MHz RC oscillator (HSI) with PLL option
- Internal 40kHz RC oscillator (LSI)
- External 4-32MHz crystal oscillator (HSE)
- Multiple low-power modes: Sleep, Stop, Standby
- Power on/down reset
- 15 I/Os, all available as external interrupts, many 5V-tolerant
- 1x 5-channel DMA controller with flexible mapping
- 1x 12-bit ADC, up to 9+2 channels, 0V - 3.6V
- 1× 16-bit advanced-control timer wit 6 PWM-channels
- 4× 16-bit general-purpose timer
- 2x watchdog timer
- 1x SysTick timer
- 1x real-time clock (RTC) with calendar, alarm and periodic wakeup
- 1x USART interface with auto baud rate detection
- 1x I²C interface supporting Fast-mode Plus (1Mbit/s)
- 1x SPI interface with up to 18Mbit/s
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

## Serial Wire Debug Interface (SWD)
The MCU can also be programmed via the SWD interface with corresponding hardware (ST-Link, DAPLink) and software (STM32CubeProg, OpenOCD, PyOCD, J-Link).

## Arduino and PlatformIO Support
You can use [STM32duino](https://github.com/stm32duino) as an Arduino IDE core for several STM32 microcontrollers. There is also support for [PlatformIO](https://stm32-base.org/guides/platformio.html).

# References, Links and Notes
- [EasyEDA Design Files](https://oshwlab.com/wagiminator)
- [STM Product Page](https://www.st.com/en/microcontrollers-microprocessors/stm32f030f4.html)
- [MCU Templates](https://github.com/wagiminator/MCU-Templates)
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [Arduino Support](https://github.com/stm32duino)
- [PlatformIO Support](https://stm32-base.org/guides/platformio.html)
- [ST-Link Software](https://www.st.com/en/development-tools/st-link-v2.html#tools-software)

![STM32F030F4P6_DevBoard_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32F030F4P6_DevBoard/documentation/STM32F030F4P6_DevBoard_pic2.jpg)
![STM32F030F4P6_DevBoard_pic3.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STM32F030F4P6_DevBoard/documentation/STM32F030F4P6_DevBoard_pic3.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
