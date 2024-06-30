#!/usr/bin/env python3
# ===================================================================================
# Project:   chprog - USB Programming Tool for WCH Microcontrollers
# Version:   v2.4
# Year:      2022
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   MIT License
# ===================================================================================
#
# Description:
# ------------
# Simple Python tool for flashing WCH microcontrollers
# (CH5xx, CH32Fxxx, CH32Vxxx, CH32Xxxx, CH32Lxxx)
# via USB with factory built-in bootloader versions 2.x.x.
#
# References:
# -----------
# chprog is based on isp55e0, chflasher, and wchprog:
# - Frank Zago, https://github.com/frank-zago/isp55e0
# - Aaron Christophel, https://github.com/atc1441/chflasher
# - juliuswwj, https://github.com/juliuswwj/wchprog
#
# Dependencies:
# -------------
# - pyusb
#
# Operating Instructions:
# -----------------------
# You need to install PyUSB to use chprog. Install it via "python3 -m pip install pyusb".
#
# Linux users need permission to access the device. Run:
# echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
# echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="1a86", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee -a /etc/udev/rules.d/99-ch55x.rules
# sudo udevadm control --reload-rules
#
# On Windows you will need the Zadig tool (https://zadig.akeo.ie/) to install the
# correct driver. Click "Options" and "List All Devices" to select the USB module.
# Then install the libusb-win32 driver.
#
# Connect the MCU via USB to your PC. The MCU must be in bootloader mode!
# Run "python3 chprog.py firmware.bin".


import usb.core
import usb.util
import sys

# ===================================================================================
# Main Function
# ===================================================================================

def _main():
    if len(sys.argv) != 2:
        sys.stderr.write('ERROR: No bin file selected!\n')
        sys.exit(1)

    try:
        print('Connecting to bootloader ...')
        isp = Programmer()
        isp.detect()
        print('Found %s with bootloader v%s.' % (isp.chipname, isp.bootloader))
        print('Flashing %s to %s ...' % (sys.argv[1], isp.chipname))
        with open(sys.argv[1], 'rb') as f: data = f.read()
        isp.flash(data)
        print('Verifying %d bytes ...' % len(data))
        isp.verify(data)
        isp.exit()
    except Exception as ex:
        sys.stderr.write('ERROR: %s!\n' % str(ex))
        sys.exit(1)
    print('DONE.')
    sys.exit(0)

# ===================================================================================
# Programmer Class
# ===================================================================================

class Programmer:
    # Init programming interface
    def __init__(self):
        # Find device
        self.dev = usb.core.find(idVendor = CH_USB_VENDOR_ID1, idProduct = CH_USB_PRODUCT_ID)
        if self.dev is None:
            self.dev = usb.core.find(idVendor = CH_USB_VENDOR_ID2, idProduct = CH_USB_PRODUCT_ID)
            if self.dev is None:
                raise Exception('MCU not found. Check if device is in BOOT mode')

        # Set configuration
        try:
            self.dev.set_configuration()
        except:
            raise Exception('Failed to access USB Device. Check permissions')

        # Clear USB receive buffer
        try:    self.dev.read(CH_USB_EP_IN, 1024, 1)
        except: None

        # Set initial chip parameters
        self.device    = None
        self.uidlen    = 4
        self.lastwrite = False
        self.wpremove  = False
        self.xorkey    = bytearray(CH_XOR_KEY_LEN)

    # Send command and return reply
    def sendcommand(self, stream):
        self.dev.write(CH_USB_EP_OUT, stream)
        try:    return self.dev.read(CH_USB_EP_IN, CH_USB_PACKET_SIZE, CH_USB_TIMEOUT)
        except: return None

    # Detect, identify and setup chip
    def detect(self):
        # Read type of chip
        identanswer = self.sendcommand(CH_STR_CHIP_DETECT)
        if len(identanswer) != 6:
            raise Exception('Failed to identify chip')

        # Read configuration
        cfganswer = self.sendcommand(CH_STR_CONFIG_READ)
        if len(cfganswer) != 30:
            raise Exception('Failed to read chip configuration')

        # Set chip and configuration values
        self.configdata = cfganswer[6:18]
        self.bootloader = '%d.%d.%d' % (cfganswer[19], cfganswer[20], cfganswer[21])
        self.chiptype   = identanswer[4]
        self.chipfamily = identanswer[5]
        self.chipid     = (self.chipfamily << 8) + self.chiptype

        # Find device in dictionary
        for d in DEVICES:
            if d['id'] == self.chipid:
                self.device = d
        if self.device is None:
            raise Exception('Unsupported chip (ID: 0x%04x)' % self.chipid)

        # Set parameters according to dictionary
        self.chipname        = self.device['name']
        self.code_flash_size = self.device['code_size']
        self.data_flash_size = self.device['data_size']
        if self.chipfamily > 0x11:
            self.uidlen = 8
        if self.chipfamily in LASTWRITELIST:
            self.lastwrite = True
        if self.chipfamily in WPREMOVELIST:
            self.wpremove = True
        self.chipuid = cfganswer[22:(22 + self.uidlen)]

        # Create local encryption key
        sum = 0
        for x in range(self.uidlen):
            sum += self.chipuid[x]
        for x in range(CH_XOR_KEY_LEN - 1):
            self.xorkey[x] = sum & 0xff
        self.xorkey[CH_XOR_KEY_LEN - 1] = (sum + self.chiptype) & 0xff

        # Send encryption key
        sum = 0
        for x in range(CH_XOR_KEY_LEN):
            sum += self.xorkey[x]
        stream = bytes((CH_CMD_KEY_SET, 0x1e, 0x00)) + bytes(0x1e)
        reply = self.sendcommand(stream)
        if reply[4] != (sum & 0xff):
            raise Exception('Failed to set encryption key')

        # Unlock chip (remove read protection)
        if (self.wpremove) and (self.configdata[0] == 0xff):
            self.configdata[0] = 0xa5
            self.sendcommand(b'\xa8\x0e\x00\x07\x00' + self.configdata)
        if self.chipid == 0x1379:
            self.configdata[8] &= 0x7f
            self.sendcommand(b'\xa8\x0e\x00\x07\x00' + self.configdata)

    # Erase code flash
    def erase(self, size):
        if size > self.code_flash_size:
            raise Exception('Not enough memory')
        size = (size + 1023) // 1024
        if size < 8:  size = 8
        stream = (CH_CMD_CODE_ERASE, 0x04, 0x00, size & 0xff, size >> 8, 0x00, 0x00)
        reply = self.sendcommand(stream)
        if reply[4] != 0x00:
            raise Exception('Failed to erase chip')

    # Write data stream to code flash
    def flash(self, data):
        self.erase(len(data))
        self.writeflash(data, CH_CMD_CODE_WRITE)

    # Verify data stream in code flash
    def verify(self, data):
        if len(data) > self.code_flash_size:
            raise Exception('Not enough memory')
        self.writeflash(data, CH_CMD_CODE_VERIFY)

    # Reboot and exit
    def exit(self):
        self.sendcommand(CH_STR_REBOOT)

    # Perform flash operations
    def writeflash(self, data, mode):
        # Encrypt data
        if (len(data) % CH_XOR_KEY_LEN) > 0:
            data += b'0xff' * (CH_XOR_KEY_LEN - (len(data) % CH_XOR_KEY_LEN))
        data = bytearray(data)
        for x in range(len(data)):
            data[x] ^= self.xorkey[x % CH_XOR_KEY_LEN]

        # Send data in chunks of 56 bytes
        rest    = len(data)
        offset  = 0
        pkt_len = 0
        while rest > 0:
            pkt_len = min(rest, 0x38)
            stream  = bytes((mode, pkt_len + 5, 0))
            stream += offset.to_bytes(4, byteorder='little')
            stream += (rest & 0xff).to_bytes(1, byteorder='little')
            stream += data[offset:(offset + pkt_len)]
            reply   = self.sendcommand(stream)
            if reply[4] != 0x00 and reply[4] != 0xfe and reply[4] != 0xf5:
                if mode == CH_CMD_CODE_WRITE:
                    raise Exception('Failed to write at offset 0x%08x' % offset)
                elif mode == CH_CMD_CODE_VERIFY:
                    raise Exception('Failed to verify at offset 0x%08x' % offset)
            offset += pkt_len
            rest   -= pkt_len

        # Some chips need a last empty write
        if (self.lastwrite) and (mode == CH_CMD_CODE_WRITE):
            stream = bytes((mode, 5, 0)) + offset.to_bytes(4, byteorder='little') + b'\x00'
            self.sendcommand(stream)

# ===================================================================================
# Protocol Constants
# ===================================================================================

CH_USB_VENDOR_ID1   = 0x4348    # VID 1
CH_USB_VENDOR_ID2   = 0x1a86    # VID 2
CH_USB_PRODUCT_ID   = 0x55e0    # PID
CH_USB_INTERFACE    = 0         # interface number
CH_USB_EP_OUT       = 0x02      # endpoint for command transfer out
CH_USB_EP_IN        = 0x82      # endpoint for reply transfer in
CH_USB_PACKET_SIZE  = 64        # packet size
CH_USB_TIMEOUT      = 5000      # timeout for USB operations

CH_CMD_CHIP_TYPE    = 0xa1
CH_CMD_REBOOT       = 0xa2
CH_CMD_KEY_SET      = 0xa3
CH_CMD_CODE_ERASE   = 0xa4
CH_CMD_CODE_WRITE   = 0xa5
CH_CMD_CODE_VERIFY  = 0xa6
CH_CMD_CONFIG_READ  = 0xa7
CH_CMD_CONFIG_WRITE = 0xa8
CH_CMD_DATA_ERASE   = 0xa9
CH_CMD_DATA_WRITE   = 0xaa
CH_CMD_DATA_READ    = 0xab

CH_STR_CHIP_DETECT  = b'\xa1\x12\x00\x52\x11MCU ISP & WCH.CN'
CH_STR_CONFIG_READ  = (0xa7, 0x02, 0x00, 0x1f, 0x00)
CH_STR_CODE_ERASE   = (0xa4, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00)
CH_STR_DATA_ERASE   = (0xa9, 0x05, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02)
CH_STR_REBOOT       = (0xa2, 0x01, 0x00, 0x01)

CH_XOR_KEY_LEN      = 8

# ===================================================================================
# Device definitions
# ===================================================================================

DEVICES = [
    {'name': 'CH543', 'id': 0x1243, 'code_size':  14336, 'data_size':   256},
    {'name': 'CH544', 'id': 0x1244, 'code_size':  61440, 'data_size':  1024},
    {'name': 'CH545', 'id': 0x1245, 'code_size':  61440, 'data_size':  1024},
    {'name': 'CH546', 'id': 0x1246, 'code_size':  32768, 'data_size':  1024},
    {'name': 'CH547', 'id': 0x1247, 'code_size':  61440, 'data_size':  1024},
    {'name': 'CH548', 'id': 0x1248, 'code_size':  32768, 'data_size':  1024},
    {'name': 'CH549', 'id': 0x1249, 'code_size':  61440, 'data_size':  1024},

    {'name': 'CH551', 'id': 0x1151, 'code_size':  10240, 'data_size':   128},
    {'name': 'CH552', 'id': 0x1152, 'code_size':  14336, 'data_size':   128},
    {'name': 'CH554', 'id': 0x1154, 'code_size':  14336, 'data_size':   128},
    {'name': 'CH555', 'id': 0x1155, 'code_size':  61440, 'data_size':  1024},
    {'name': 'CH556', 'id': 0x1156, 'code_size':  61440, 'data_size':  1024},
    {'name': 'CH557', 'id': 0x1157, 'code_size':  61440, 'data_size':  1024},
    {'name': 'CH558', 'id': 0x1158, 'code_size':  61440, 'data_size':  1024},
    {'name': 'CH559', 'id': 0x1159, 'code_size':  61440, 'data_size':  1024},

    {'name': 'CH573', 'id': 0x1373, 'code_size': 458752, 'data_size': 32768},
    {'name': 'CH579', 'id': 0x1379, 'code_size': 256000, 'data_size':  2048},
    {'name': 'CH582', 'id': 0x1682, 'code_size': 458752, 'data_size': 32768},

    {'name': 'CH32F103x6',   'id': 0x1432, 'code_size':  32768, 'data_size': 0},
    {'name': 'CH32F103x8',   'id': 0x143f, 'code_size':  65536, 'data_size': 0},

    {'name': 'CH32F203VCT6', 'id': 0x1830, 'code_size': 262144, 'data_size': 0},
    {'name': 'CH32F203RCT6', 'id': 0x1831, 'code_size': 262144, 'data_size': 0},
    {'name': 'CH32F203CBT6', 'id': 0x1832, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32F203RBT6', 'id': 0x1833, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32F205RBT6', 'id': 0x1850, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32F207VCT6', 'id': 0x1870, 'code_size': 262144, 'data_size': 0},

    {'name': 'CH32F203C8U6', 'id': 0x1a30, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32F203C8T6', 'id': 0x1a31, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32F203K8T6', 'id': 0x1a32, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32F203C6T6', 'id': 0x1a33, 'code_size':  32768, 'data_size': 0},
    {'name': 'CH32F208WBU6', 'id': 0x1a80, 'code_size': 491520, 'data_size': 0},
    {'name': 'CH32F208RBT6', 'id': 0x1a81, 'code_size': 131072, 'data_size': 0},

    {'name': 'CH32V103x6',   'id': 0x1532, 'code_size':  32768, 'data_size': 0},
    {'name': 'CH32V103x8',   'id': 0x153f, 'code_size':  65536, 'data_size': 0},

    {'name': 'CH32V203C8U6', 'id': 0x1930, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32V203C8T6', 'id': 0x1931, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32V203K8T6', 'id': 0x1932, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32V203C6T6', 'id': 0x1933, 'code_size':  32768, 'data_size': 0},
    {'name': 'CH32V203RBT6', 'id': 0x1934, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32V203K6T6', 'id': 0x1935, 'code_size':  32768, 'data_size': 0},
    {'name': 'CH32V203G6U6', 'id': 0x1936, 'code_size':  32768, 'data_size': 0},
    {'name': 'CH32V203F6P6', 'id': 0x1937, 'code_size':  32768, 'data_size': 0},
    {'name': 'CH32V203F8P6', 'id': 0x193a, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32V203G8R6', 'id': 0x193b, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32V208WBU6', 'id': 0x1980, 'code_size': 491520, 'data_size': 0},
    {'name': 'CH32V208RBT6', 'id': 0x1981, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32V208CBU6', 'id': 0x1982, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32V208GBU6', 'id': 0x1983, 'code_size': 131072, 'data_size': 0},

    {'name': 'CH32V303VCT6', 'id': 0x1730, 'code_size': 262144, 'data_size': 0},
    {'name': 'CH32V303RCT6', 'id': 0x1731, 'code_size': 262144, 'data_size': 0},
    {'name': 'CH32V303RBT6', 'id': 0x1733, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32V303CBT6', 'id': 0x1734, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32V305RBT6', 'id': 0x1750, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32V305FBP6', 'id': 0x1752, 'code_size': 131072, 'data_size': 0},
    {'name': 'CH32V307VCT6', 'id': 0x1770, 'code_size': 262144, 'data_size': 0},
    {'name': 'CH32V307RCT6', 'id': 0x1771, 'code_size': 262144, 'data_size': 0},
    {'name': 'CH32V307WCU6', 'id': 0x1773, 'code_size': 262144, 'data_size': 0},

    {'name': 'CH32X033F8P6', 'id': 0x235a, 'code_size':  63488, 'data_size': 0},
    {'name': 'CH32X035R8T6', 'id': 0x2350, 'code_size':  63488, 'data_size': 0},
    {'name': 'CH32X035C8T6', 'id': 0x2351, 'code_size':  63488, 'data_size': 0},
    {'name': 'CH32X035F8U6', 'id': 0x235e, 'code_size':  63488, 'data_size': 0},
    {'name': 'CH32X035G8U6', 'id': 0x2356, 'code_size':  63488, 'data_size': 0},
    {'name': 'CH32X035G8R6', 'id': 0x235b, 'code_size':  63488, 'data_size': 0},
    {'name': 'CH32X035F7P6', 'id': 0x2357, 'code_size':  49152, 'data_size': 0},

    {'name': 'CH32L103C8U6', 'id': 0x2530, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32L103C8T6', 'id': 0x2531, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32L103F8P6', 'id': 0x253a, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32L103G8R6', 'id': 0x253b, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32L103K8U6', 'id': 0x2532, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32L103F8U6', 'id': 0x253d, 'code_size':  65536, 'data_size': 0},
    {'name': 'CH32L103F7P6', 'id': 0x2537, 'code_size':  49152, 'data_size': 0}
]

LASTWRITELIST = (0x12, 0x14, 0x15, 0x17, 0x18,0x19, 0x1a, 0x23, 0x25)
WPREMOVELIST  = (0x14, 0x15, 0x17, 0x18, 0x19, 0x1a, 0x23, 0x25)

# ===================================================================================

if __name__ == "__main__":
    _main()
