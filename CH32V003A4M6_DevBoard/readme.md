# CH32V003 A4M6 Development Board
Development board for the CH32V003A4M6 ultra-cheap 32-bit RISC-V microcontroller.

![CH32V003A4M6_DevBoard_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V003A4M6_DevBoard/documentation/CH32V003A4M6_DevBoard_pic1.jpg)

# Pinout
![CH32V003A4M6_DevBoard_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V003A4M6_DevBoard/documentation/CH32V003A4M6_DevBoard_pinout.png)

# The CH32V003 Family of 32-bit RISC-V Microcontrollers
## Overview
The CH32V003 series is a collection of industrial-grade general-purpose microcontrollers that utilize the QingKe RISC-V2A core design supporting the RV32EC instruction set. These microcontrollers are equipped with various features such as a 48MHz system main frequency, wide voltage support, a single-wire serial debug interface, low power consumption, and an ultra-small package. Additionally, the CH32V003 series includes a built-in set of components including a DMA controller, a 10-bit ADC, op-amp comparators, multiple timers, and standard communication interfaces such as USART, I2C, and SPI.

## Block Diagram
![CH32V003_block.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V003F4P6_DevBoard/documentation/CH32V003_block.png)

## Features
- QingKe 32-bit RISC-V2A processor, supporting 2 levels of interrupt nesting
- Maximum 48MHz system main frequency
- 2KB SRAM, 16KB Flash
- Power supply voltage: 3.3/5V
- Multiple low-power modes: Sleep, Standby
- Power on/off reset, programmable voltage detector
- 1 group of 1-channel general-purpose DMA controller
- 1 group of op-amp comparator
- 1 group of 10-bit ADC
- 1×16-bit advanced-control timer, 1×16-bit general-purpose timer
- 2 WDOG, 1×32-bit SysTick
- 1 USART interface, 1 group of I2C interface, 1 group of SPI interface
- 18 I/O ports, mapping an external interrupt
- 64-bit chip unique ID
- Single-wire serial debug interface (SDI)

# Programming and Debugging
## WCH-LinkE
To program the CH32V003 microcontroller, you will need a special programming device which utilizes the proprietary single-wire serial debug interface (SDI). The [WCH-LinkE](http://www.wch-ic.com/products/WCH-Link.html) (pay attention to the "E" in the name) is a suitable device for this purpose and can be purchased commercially for around $3. This debugging tool is not only compatible with the CH32V003 but also with other WCH RISC-V and ARM-based microcontrollers. WCH offers free but closed-source software options for the PC side, such as [MounRiver Studio](http://www.mounriver.com/) and [WCH-LinkUtility](https://www.wch.cn/downloads/WCH-LinkUtility_ZIP.html).

You can also use the Python tool rvprog.py for flashing the CH32V003 microcontroller with the WCH-LinkE. It is provided with the examples in the software folder. Alternatively, there is a platform-independent open-source tool called minichlink developed by Charles Lohr (CNLohr), which can be found [here](https://github.com/cnlohr/ch32v003fun/tree/master/minichlink). Additionally, a customized version of [OpenOCD](https://github.com/karlp/openocd-hacks) can be used for uploading firmware. More information can be found [here](https://github.com/wuxx/nanoCH32V003).

![CH32V003_wch-linke.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V003F4P6_DevBoard/documentation/CH32V003_wch-linke.jpg)

To use the WCH-LinkE on Linux, you need to grant access permissions beforehand by executing the following commands:
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8010", MODE="666"' | sudo tee /etc/udev/rules.d/99-WCH-LinkE.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8012", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-WCH-LinkE.rules
sudo udevadm control --reload-rules
```

To upload firmware and perform debugging, you need to ensure that the development board is disconnected from any power sources. Then, you should make the following connections to the WCH-LinkE:

```
WCH-LinkE      DevBoard
+-------+      +------+
|  SWDIO| <--> |DIO   |
|    GND| ---> |GND   |
|    3V3| ---> |3V3   |
+-------+      +------+
```

If the blue LED on the WCH-LinkE remains illuminated once it is connected to the USB port, it means that the device is currently in ARM mode and must be switched to RISC-V mode initially. There are a few ways to accomplish this:
- You can utilize the Python tool called rvmode.py, which is provided with the examples in the software folder.
- Alternatively, you can select "WCH-LinkRV" in the software provided by WCH, such as MounRiver Studio or WCH-LinkUtility.
- Another option is to hold down the ModeS button on the device while plugging it into the USB port.

More information can be found in the [WCH-Link User Manual](http://www.wch-ic.com/downloads/WCH-LinkUserManual_PDF.html).

## Open-Source Debugging Devices
As part of his [ch32v003fun](https://github.com/cnlohr/ch32v003fun) project, Charles Lohr has also developed open-source programmers/debuggers based on STM32F042 and ESP32S2. Furthermore, the schematic diagram of the WCH-LinkE based on the CH32V305F is available on the manufacturer's [website](https://www.wch.cn/products/WCH-Link.html), but the [firmware](https://github.com/openwch/ch32v003) can only be downloaded as a binary file.

# Software Development Tools
## MounRiver Studio IDE
The Eclipse-based closed-source [MounRiver Studio IDE](http://www.mounriver.com/) is the official development environment provided by WCH. It supports Windows, Linux, and Mac. MounRiver Studio can be downloaded for free.

## Open-Source GCC Toolchain
You can download a complete toolchain (GCC and OpenOCD) for Linux and Mac from the MounRiver Studio website. However, the provided examples here are based on the [ch32v003fun](https://github.com/cnlohr/ch32v003fun) project by CNLohr. Follow the instructions on the Github page to install the toolchain.

To install the GCC compiler on Linux, you can use the following commands:

```
sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf
```

## Arduino IDE and PlatformIO Support
There are projects around to make the CH32V003 compatible with the Arduino IDE ([arduino_core_ch32](https://github.com/openwch/arduino_core_ch32) and [arduino-wch32v003](https://github.com/AlexanderMandera/arduino-wch32v003)) and PlatformIO ([platform-ch32v](https://github.com/Community-PIO-CH32V/platform-ch32v)).

# References, Links and Notes
- [EasyEDA Design Files](https://oshwlab.com/wagiminator)
- [MCU Templates](https://github.com/wagiminator/MCU-Templates)
- [MCU Flash Tools](https://github.com/wagiminator/MCU-Flash-Tools)
- [MounRiver Studio](http://www.mounriver.com/)
- [ch32003fun SDK](https://github.com/cnlohr/ch32v003fun)
- [arduino_core_ch32](https://github.com/openwch/arduino_core_ch32)
- [arduino-wch32v003](https://github.com/AlexanderMandera/arduino-wch32v003)
- [CH32V003 with PlatformIO](https://github.com/Community-PIO-CH32V/platform-ch32v)
- [CH32V003 on openwch](https://github.com/openwch/ch32v003)
- [MuseLab nanoCH32V003](https://github.com/wuxx/nanoCH32V003)
- [CH32V003 datasheets](http://www.wch-ic.com/products/CH32V003.html)
- [WCH-Link user manual](http://www.wch-ic.com/downloads/WCH-LinkUserManual_PDF.html)
- [WCH Official Store on AliExpress](https://wchofficialstore.aliexpress.com)

![CH32V003A4M6_DevBoard_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V003A4M6_DevBoard/documentation/CH32V003A4M6_DevBoard_pic2.jpg)
![CH32V003A4M6_DevBoard_pic3.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V003A4M6_DevBoard/documentation/CH32V003A4M6_DevBoard_pic3.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
