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
- Factory built-in USB bootloader

## Family Members
![CH32V203_table.png](https://raw.githubusercontent.com/wagiminator/Development-Boards/main/CH32V203F6P6_DevBoard/documentation/CH32V203_table.png)

Note that only the CH32V203F6P6 can be used on this development board. Although the CH32V203F8P6 comes in the same package (TSSOP20), it has a completely different pinout.

# Programming and Debugging
There are two ways to program the microcontroller: via USB using the built-in bootloader and via the serial 2-wire debug interface using a WCH-Link programming device.

## Built-in USB Bootloader
### Installing Drivers for the Bootloader
On Linux you do not need to install a driver. However, by default Linux will not expose enough permission to upload your code with the USB bootloader. In order to fix this, open a terminal and run the following commands:

```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
sudo udevadm control --reload-rules
```

For Windows, you need the [CH372 driver](http://www.wch-ic.com/downloads/CH372DRV_EXE.html). Alternatively, you can also use the [Zadig Tool](https://zadig.akeo.ie/) to install the correct driver. Here, click "Options" -> "List All Devices" and select the USB module. Then install the libusb-win32 driver. To do this, the board must be connected and the CH55x must be in bootloader mode.

### Entering Bootloader Mode
The bootloader must be started manually for new uploads. To do this, the board must first be disconnected from the USB port and all voltage sources. Now press the BOOT0 button and keep it pressed while reconnecting the board to the USB port of your PC. The chip now starts in bootloader mode, the BOOT0 button can be released and new firmware can be uploaded via USB. Alternatively, you can leave the board connected to the USB port, press and hold the BOOT0 button, press and release the RESET button and then release the BOOT0 button to enter the bootloader mode.

### WCHISPTool
WCH offers the free but closed-source software [WCHISPTool](https://www.wch.cn/downloads/WCHISPTool_Setup_exe.html) to upload firmware with Windows via the USB bootloader.

### chprog.py
You can also use chprog.py, a simple Python command line tool provided with the example software in this repo, to flash the microcontroller via the built-in USB bootloader. In order for this tool to work, Python3 must be installed on your system. To do this, follow these [instructions](https://www.pythontutorial.net/getting-started/install-python/). In addition [PyUSB](https://github.com/pyusb/pyusb) must be installed. On Linux (Debian-based), all of this can be done with the following commands:

```
sudo apt install python3 python3-pip
python3 -m pip install pyusb
```

```
Usage example:
python3 chprog.py firmware.bin
```

### Alternative Software Tools
- [isp55e0](https://github.com/frank-zago/isp55e0)
- [wchisp](https://github.com/ch32-rs/wchisp)

## Serial 2-Wire Debug Interface
### WCH-LinkE
To program the CH32V203 microcontroller via the serial debug interface, you will need a special programming device. The [WCH-LinkE](http://www.wch-ic.com/products/WCH-Link.html) (pay attention to the "E" in the name) is a suitable option for this purpose and can be purchased commercially for around $3. This debugging tool is not only compatible with the CH32V203 but also with other WCH RISC-V and ARM-based microcontrollers.

To use the WCH-Link on Linux, you need to grant access permissions beforehand by executing the following commands:
```
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8010", MODE="666"' | sudo tee /etc/udev/rules.d/99-WCH-LinkE.rules
echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="8012", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-WCH-LinkE.rules
sudo udevadm control --reload-rules
```

On Windows, if you need to you can install the WinUSB driver over the WCH interface 1.

To upload firmware, you should make the following connections to the WCH-LinkE:

```
WCH-LinkE      MCU Board
+-------+      +------+
|  SWCLK| ---> |CLK   |
|  SWDIO| <--> |DIO   |
|    GND| ---> |GND   |
|    3V3| ---> |3V3   |
+-------+      +------+
```

If the blue LED on the WCH-LinkE remains illuminated once it is connected to the USB port, it means that the device is currently in ARM mode and must be switched to RISC-V mode initially. There are a few ways to accomplish this:
- You can utilize the Python tool called rvmode.py (refer to the instructions below).
- Alternatively, you can select "WCH-LinkRV" in the software provided by WCH, such as MounRiver Studio or WCH-LinkUtility.
- Another option is to hold down the ModeS button on the device while plugging it into the USB port.

More information can be found in the [WCH-Link User Manual](http://www.wch-ic.com/downloads/WCH-LinkUserManual_PDF.html).

### WCH-LinkUtility
WCH offers the free but closed-source software [WCH-LinkUtility](https://www.wch.cn/downloads/WCH-LinkUtility_ZIP.html) to upload firmware with Windows using the WCH-LinkE.

### rvprog.py
You can also use rvprog.py, a simple Python command line tool provided with the example software in this repo, to flash the microcontroller via the WCH-LinkE or compatible programmers/debuggers. In order for this tool to work, Python3 must be installed on your system (see chprog.py above for details).

```
Usage example:
python3 rvprog.py -f firmware.bin
```

### rvmode.py
The Python tool rvmode.py, which is provided with the example software, offers a simple way to switch the WCH-LinkE to RISC-V mode (LinkRV mode). Python3 and PyUSB must be installed on your system for the tool to work.
```
Usage example:
python3 rvmode.py
```

### Alternative Software Tools
- [minichlink](https://github.com/cnlohr/ch32v003fun/tree/master/minichlink)
- [wlink](https://github.com/ch32-rs/wlink)

### Alternative Debugging Devices
As part of his [ch32v003fun](https://github.com/cnlohr/ch32v003fun) project, Charles Lohr has also developed open-source programmers/debuggers based on STM32F042 and ESP32S2. Furthermore, the schematic diagram of the WCH-LinkE based on the CH32V305 is available on the manufacturer's [website](https://www.wch.cn/products/WCH-Link.html), but the [firmware](https://github.com/openwch/ch32v003) can only be downloaded as a binary file.

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
- [CH32V203 with Arduino IDE](https://github.com/openwch/arduino_core_ch32)
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
