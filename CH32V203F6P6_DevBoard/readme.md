# CH32V203F6P6 Development Board
Development board for the CH32V203F6P6 low-cost 32-bit RISC-V microcontroller with native USB 2.0 and Arduino IDE support.

![CH32V203F6P6_DevBoard_pic1.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V203F6P6_DevBoard/documentation/CH32V203F6P6_DevBoard_pic1.jpg)

# Pinout
![CH32V203F6P6_DevBoard_pinout.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V203F6P6_DevBoard/documentation/CH32V203F6P6_DevBoard_pinout.png)

# The CH32V203 Family of 32-bit RISC-V USB Microcontrollers
## Overview
The CH32V203 is an industrial-grade microcontroller (MCU) designed for general-purpose use, with low power consumption and enhanced performance. It is based on a 32-bit RISC-V core, supports the RV32IMAC instruction set, and can operate at clock frequencies of up to 144MHz. The MCU's power consumption is highly efficient, reaching as low as 45uA per MHz. Additionally, the CH32V203 integrates a USB interface with two channels, enabling both USB Host and USB Device functionalities. It also provides a CAN interface (2.0B active) with one channel, up to two OPA channels, four UARTs, two I²Cs, a 12-bit ADC, a 10-channel TouchKey, and various other peripheral resources.

## Block Diagram
![CH32V203_block.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V203F6P6_DevBoard/documentation/CH32V203_block.png)

## Features
- Qingke 32-bit RISC-V4B processor
- Up to 144MHz system clock frequency
- Single-cycle multiplication and hardware division
- Up to 20KB SRAM, 64KB Flash
- Supply voltage: 2.5V/3.3V, GPIO unit is supplied independently
- Multiple low-power modes: sleep/stop/standby
- Down to 44.65uA/MHz in Run mode
- Down to 17.90uA/MHz in Sleep mode
- Power-on/power-down reset (POR/PDR), programmable voltage detector (PVD)
- 1 x 18-channel general DMA controllers
- 2 OPA amplifiers
- 10-channel 12-bit ADC, 10-channel TouchKey
- 4 timers
- One USB2.0 full-speed host/device interface and/or one USB2.0 full-speed device interface
- Up to 4 UARTs
- Up to ne CAN interface (2.0B active)
- Up to 2 I²C interfaces, up to 2 SPI interfaces
- Up to 37 I/O ports, can be mapped to 16 external interrupts
- Up to one CRC calculation unit, 96-bit unique ID
- Serial 2-wire debug interface

## Family Members
![CH32V203_table.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V203F6P6_DevBoard/documentation/CH32V203_table.png)

Note that only the CH32V203F6P6 can be used on this development board. Although the CH32V203F8P6 comes in the same package (TSSOP20), it has a completely different pinout.

# Programming and Debugging
## WCH-LinkE
To program the CH32V203 microcontroller, you will need a special programming device. The [WCH-LinkE](http://www.wch-ic.com/products/WCH-Link.html) (pay attention to the "E" in the name) is a suitable option for this purpose and can be purchased commercially for around $3. This debugging tool is not only compatible with the CH32V203 but also with other WCH RISC-V and ARM-based microcontrollers. WCH offers free but closed-source software options for the PC side, such as [MounRiver Studio](http://www.mounriver.com/) and [WCH-LinkUtility](https://www.wch.cn/downloads/WCH-LinkUtility_ZIP.html).

You can also use the Python tool rvprog.py for flashing the CH32V203 microcontroller with the WCH-LinkE. It is provided with the examples in the software folder. Alternatively, there is a platform-independent open-source tool called minichlink developed by Charles Lohr (CNLohr), which can be found [here](https://github.com/cnlohr/ch32v003fun/tree/master/minichlink). Additionally, a customized version of [OpenOCD](https://github.com/karlp/openocd-hacks) can be used for uploading firmware. More information can be found [here](https://github.com/wuxx/nanoCH32V003).

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
|  SWCLK| ---> |CLK   |
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
You can download a complete toolchain (GCC and OpenOCD) for Linux and Mac from the MounRiver Studio website. However, on Linux you can also install a GCC-based toolchain by running the following command:

```
sudo apt install build-essential libnewlib-dev gcc-riscv64-unknown-elf
```

## Arduino IDE and PlatformIO Support
There are projects around to make the CH32V203 compatible with the Arduino IDE ([arduino_core_ch32](https://github.com/openwch/arduino_core_ch32)) and PlatformIO ([platform-ch32v](https://github.com/Community-PIO-CH32V/platform-ch32v)).

# References, Links and Notes
- [EasyEDA Design Files](https://oshwlab.com/wagiminator)
- [MounRiver Studio](http://www.mounriver.com/)
- [arduino_core_ch32](https://github.com/openwch/arduino_core_ch32)
- [CH32V203 with PlatformIO](https://github.com/Community-PIO-CH32V/platform-ch32v)
- [CH32V203 on openwch](https://github.com/openwch/ch32v203)
- [CH32V203 datasheets](http://www.wch-ic.com/products/CH32V203.html)
- [WCH-Link user manual](http://www.wch-ic.com/downloads/WCH-LinkUserManual_PDF.html)
- [WCH Official Store on AliExpress](https://wchofficialstore.aliexpress.com)

![CH32V203F6P6_DevBoard_pic2.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V203F6P6_DevBoard/documentation/CH32V203F6P6_DevBoard_pic2.jpg)
![CH32V203F6P6_DevBoard_pic3.jpg](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V203F6P6_DevBoard/documentation/CH32V203F6P6_DevBoard_pic3.jpg)

# License

![license.png](https://i.creativecommons.org/l/by-sa/3.0/88x31.png)

This work is licensed under Creative Commons Attribution-ShareAlike 3.0 Unported License. 
(http://creativecommons.org/licenses/by-sa/3.0/)
