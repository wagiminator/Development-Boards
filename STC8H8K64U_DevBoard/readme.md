# STC8H8K64U-45I-TSSOP20 Development Board
Development board for the STC8H8K64U low-cost 8-bit 8051 core microcontroller with native full-speed USB 2.0 and factory built-in USB bootloader.

![STC8H8K64U_DevBoard_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STC8H8K64U_DevBoard/documentation/STC8H8K64U_DevBoard_pic1.jpg)

# Pinout
![STC8H8K64U_DevBoard_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STC8H8K64U_DevBoard/documentation/STC8H8K64U_DevBoard_pinout.png)

# The STC8H8K64U Family of 8-bit 8051 USB2.0 Microcontrollers
## Overview
The STC8H8K64U series is a cost-effective family of 8-bit microcontrollers with an ultra-high speed 8051 core that is compatible with the MCS51 instruction set. These microcontrollers offer an average instruction speed that is approximately 12 times faster than the traditional 8051, with most instructions requiring only one clock cycle for execution. They support clock frequencies of up to 40MHz, operate within a voltage range of 1.9V to 5.5V, and come equipped with standard peripherals such as a 12-bit ADC, analog comparator, multiple timers, PWM, DMA, UART, SPI, and I2C. Additionally, these microcontrollers feature a built-in bootloader, allowing firmware to be uploaded directly via the native full-speed USB 2.0 interface without the need for an extra programming device.

## Features
- Ultra-high-speed 8051 core with single clock per machine cycle (1T)
- Fully compatible instruction set with the traditional 8051
- Maximum 40MHz system main frequency
- 64KB Flash for user code (CODE)
- 128 bytes internal direct access SRAM (DATA)
- 128 bytes internal indirect access SRAM (IDATA)
- 8192 bytes internal extended SRAM (XDATA)
- 1280 bytes USB data RAM (FIFO)
- Up to 32KB EEPROM (size configurable), can be erased more than 100,000 times
- Operating voltage: 1.9V - 5.5V
- Internal high-precision RC clock (IRC), adjustable to a max of 45MHz, error ±0.3%
- Internal high-precision 48MHz clock (IRC48M) for USB module
- Internal 32KHz low-speed oscillator (LSI) with a large error
- External 4MHz - 33MHz oscillator or external clock (HSE)
- Hardware 16-bit multiplier and divider (MDU16)
- Enhanced dual 16-bit data pointer with auto-increment/auto-decrement and auto-switch
- Direct memory access (DMA) controller, supports MEM-to-MEM, UART, SPI, ADC, LCM
- Power-on reset, programmable low-voltage detector
- 22 interrupt sources and 4 interrupt priority levels
- Multiple low-power modes: Idle, Stop
- Up to 61 GPIOs
- 4 modes for all GPIOs: quasi-bidirectional, push-pull output, open-drain, high-impedance input
- 8 channels enhanced 16-bit PWM, complementary output with dead time, external pulse width measurement
- 5x 16-bit timers with multiple operating modes
- 1x watchdog timer
- 1x real-time clock (RTC) with calendar and alarm functions
- 1x USB2.0/USB1.1 full-speed device interface, 6 two-way endpoints with 64 byte FIFO each
- 4x UART interface, max baud rate F_CPU/4
- 1x SPI interface, master and slave mode
- 1x I2C interface, master and slave mode
- 1x 12-bit ADC, 14 channels + internal 1.19V reference voltage, 800ksps
- 1x set of comparators
- LCM (TFT color screen) driver, supports 8080 and 6800 interface, 8-bit and 16-bit data width
- Factory-built-in USB bootloader

# Compiling, Programming and Debugging
## SDCC Toolchain
For compiling install the SDCC compiler. On Linux (Debian-based), this can be done with the following command:
```
sudo apt install build-essential sdcc
```

## Factory built-in USB Bootlader
The MCU has an embedded bootloader with USB interface, which can be used to upload firmware.

The Python tool [stc8usb.py](https://github.com/wagiminator/MCU-Flash-Tools) included with the example software can be used for this purpose. In order for this ISP tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [pyhidapi](https://pypi.org/project/hid/) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
python3 -m pip install hid
```

Because the USB bootloader functions as a Human Interface Device (HID), there's no need to install drivers. However, Linux doesn't initially grant sufficient permissions to access the bootloader. To resolve this issue, open a terminal and execute the following commands:

```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="34bf", ATTR{idProduct}=="1001", MODE="666"' | sudo tee /etc/udev/rules.d/99-STC-ISP.rules
sudo udevadm control --reload-rules
```

You can also utilize the manufacturer's proprietary programming tool, [STC ISP](https://www.stcmicro.com/rjxz.html), for programming the MCU.

To initiate new uploads, the bootloader needs to be manually started. Begin by unplugging the board from the USB port and disconnecting all power sources. Then, press and hold the BOOT button while reconnecting the board to your PC's USB port. This action triggers the chip to enter bootloader mode. Once in this mode, you can release the BOOT button and proceed to upload new firmware via USB.

## Arduino IDE Support
An Arduino core for the STC8H series can be found [here](https://github.com/ma6254/Arduino_Core_STC).

# References, Links and Notes
- [EasyEDA Design Files](https://oshwlab.com/wagiminator)
- [STC Product Page](https://stcmicro.com/stc/stc8h8k64u.html)
- [STC ISP Software](https://www.stcmicro.com/rjxz.html)
- [MCU Templates](https://github.com/wagiminator/MCU-Templates)
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [Arduino Support](https://github.com/ma6254/Arduino_Core_STC)

![STC8H8K64U_DevBoard_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STC8H8K64U_DevBoard/documentation/STC8H8K64U_DevBoard_pic2.jpg)
![STC8H8K64U_DevBoard_pic3.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/STC8H8K64U_DevBoard/documentation/STC8H8K64U_DevBoard_pic3.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
