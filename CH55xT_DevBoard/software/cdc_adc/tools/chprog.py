#!/usr/bin/env python3
# ===================================================================================
# Project:   chprog - Programming Tool for CH55x Microcontrollers
# Version:   v1.2
# Year:      2022
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   MIT License
# ===================================================================================
#
# Description:
# ------------
# Simple Python tool for flashing CH55x series microcontrollers with bootloader 
# versions 1.x and 2.x.x.
#
# References:
# -----------
# chprog is based on chflasher and wchprog:
# - https://ATCnetz.de (Aaron Christophel)
# - https://github.com/atc1441/chflasher (Aaron Christophel)
# - https://github.com/juliuswwj/wchprog
#
# Dependencies:
# -------------
# - pyusb
#
# Operating Instructions:
# -----------------------
# You need to install PyUSB to use chprog. Install it via "python -m pip install pyusb".
#
# Linux users need permission to access the device. Run:
# echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="4348", ATTR{idProduct}=="55e0", MODE="666"' | sudo tee /etc/udev/rules.d/99-ch55x.rules
# sudo udevadm control --reload-rules
#
# On Windows you will need the Zadig tool (https://zadig.akeo.ie/) to install the
# correct driver. Click "Options" and "List All Devices" to select the USB module.
# Then install the libusb-win32 driver.
#
# Connect the CH55x via USB to your PC. The CH55x must be in bootloader mode!
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
        print('Connecting to device ...')
        isp = Programmer()
        isp.detect()
        print('FOUND:', isp.chipname, 'with bootloader v' + isp.bootloader + '.')
        print('Erasing chip ...')
        isp.erase()
        print('Flashing', sys.argv[1], 'to', isp.chipname, '...')
        with open(sys.argv[1], 'rb') as f: data = f.read()
        isp.flash_data(data)
        print('SUCCESS:', len(data), 'bytes written.')
        print('Verifying ...')
        isp.verify_data(data)
        print('SUCCESS:', len(data), 'bytes verified.')
        isp.exit()
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)
    print('DONE.')
    sys.exit(0)

# ===================================================================================
# Programmer Class
# ===================================================================================

class Programmer:
    def __init__(self):
        # Find device
        dev = usb.core.find(idVendor = CH_USB_VENDOR_ID, idProduct = CH_USB_PRODUCT_ID)
        if dev is None:
            raise Exception('CH55x not found. Check if device is in BOOT mode')

        # Set configuration, find endpoints
        try:
            dev.set_configuration()
        except:
            raise Exception('Failed to access USB Device. Check permissions')
        cfg = dev.get_active_configuration()
        intf = cfg[(0,0)]
        self.epout = usb.util.find_descriptor(intf, custom_match = lambda e: \
                     usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_OUT)
        self.epin  = usb.util.find_descriptor(intf, custom_match = lambda e: \
                     usb.util.endpoint_direction(e.bEndpointAddress) == usb.util.ENDPOINT_IN)
        assert self.epout is not None
        assert self.epin is not None

        # Clear USB receive buffer
        try:    self.epin.read(CH_USB_PACKET_SIZE, 1)
        except: None

        # Set initial chip parameters
        self.device = None
        self.xorkey = bytearray(CH_XOR_KEY_LEN)

    # Detect, identify and setup chip
    def detect(self):
        # Identify chip
        identanswer = self.__sendcmd(CH_STR_CHIP_DETECT_V2)
        if len(identanswer) == 2:
            self.chipversion = 1
            self.__identchipv1()
        elif len(identanswer) == 6:
            self.chipversion = 2
            self.__identchipv2()
        else:
            raise Exception('Failed to identify chip')

        # Find device in dictionary
        self.chipid = (self.chipfamily << 8) + self.chiptype
        for d in DEVICES:
            if d['id'] == self.chipid:
                self.device = d
        if self.device is None:
            raise Exception('Unsupported chip (ID: 0x%04x)' % self.chipid)

        # Set parameters according to dictionary
        self.chipname        = self.device['name']
        self.code_flash_size = self.device['code_size']
        self.data_flash_size = self.device['data_size']

    # Erase code flash
    def erase(self):
        if self.chipversion == 1:
            self.__erasev1()
        else:
            self.__erasev2()

    # Write binary file to code flash
    def flash_bin(self, filename):
        with open(filename, 'rb') as f: data = f.read()
        self.erase()
        self.flash_data(data)
        return len(data)

    # Compare binary file with code flash
    def verify_bin(self, filename):
        with open(filename, 'rb') as f: data = f.read()
        self.verify_data(data)
        return len(data)

    # Write data stream to code flash
    def flash_data(self, data):
        if len(data) > self.code_flash_size:
            raise Exception('Not enough memory')
        if self.chipversion == 1:
            self.__writev1(data, CH_CMD_CODE_WRITE_V1)
        else:
            self.__writev2(data, CH_CMD_CODE_WRITE_V2)

    # Verify data stream in code flash
    def verify_data(self, data):
        if len(data) > self.code_flash_size:
            raise Exception('Not enough memory')
        if self.chipversion == 1:
            self.__writev1(data, CH_CMD_CODE_VERIFY_V1)
        else:
            self.__writev2(data, CH_CMD_CODE_VERIFY_V2)

    # Reboot and exit
    def exit(self):
        if self.chipversion == 1:
            self.__exitv1()
        else:
            self.__exitv2()


    # Send command and return reply
    def __sendcmd(self, cmd):
        self.epout.write(cmd)
        try:    return self.epin.read(CH_USB_PACKET_SIZE, CH_USB_TIMEOUT)
        except: return None

    def __identchipv1(self):
        # Read type of chip
        identanswer = self.__sendcmd(CH_STR_CHIP_DETECT_V1)
        if len(identanswer) != 2:
            raise Exception('Failed to identify chip')

        # Read configuration
        cfganswer = self.__sendcmd(CH_STR_CONFIG_READ_V1)
        if len(cfganswer) != 2:
            raise Exception('Failed to read chip configuration')

        # Set chip and configuration values
        self.chiptype   = identanswer[0]
        self.chipfamily = identanswer[1]
        self.bootloader = str(cfganswer[0] >> 4) + '.' + str(cfganswer[0] & 0xf)

    def __identchipv2(self):
        # Read type of chip
        identanswer = self.__sendcmd(CH_STR_CHIP_DETECT_V2)
        if len(identanswer) != 6:
            raise Exception('Failed to identify chip')

        # Read configuration
        cfganswer = self.__sendcmd(CH_STR_CONFIG_READ_V2)
        if len(cfganswer) != 30:
            raise Exception('Failed to read chip configuration')

        # Set chip and configuration values
        self.chiptype   = identanswer[4]
        self.chipfamily = identanswer[5]
        self.configdata = cfganswer[6:18]
        self.chipuid    = cfganswer[22:26]
        self.bootloader = '%d.%d.%d' % (cfganswer[19], cfganswer[20], cfganswer[21])
        if self.chipfamily != 0x11:
            raise Exception('Unsupported chip (ID: 0x%02x%02x)' % (self.chipfamily, self.chiptype))

        # Create local encryption key
        sum = 0
        for x in range(4):
            sum += self.chipuid[x]
        for x in range(CH_XOR_KEY_LEN - 1):
            self.xorkey[x] = sum & 0xff
        self.xorkey[CH_XOR_KEY_LEN - 1] = (sum + self.chiptype) & 0xff

        # Send encryption key
        sum = 0
        for x in range(CH_XOR_KEY_LEN):
            sum += self.xorkey[x]
        stream = bytes((CH_CMD_KEY_SET_V2, 0x1e, 0x00)) + bytes(0x1e)
        reply = self.__sendcmd(stream)
        if reply[4] != (sum & 0xff):
            raise Exception('Failed to set encryption key')

    def __erasev1(self):
        self.__sendcmd((0xa6, 0x04, 0x00, 0x00, 0x00, 0x00))
        for x in range(self.code_flash_size // 1024):
            reply = __sendcmd((0xa9, 0x02, 0x00, x * 4))
            if reply[0] != 0x00:
                raise Exception('Failed to erase chip')

    def __erasev2(self):
        reply = self.__sendcmd((0xa4, 0x01, 0x00, self.code_flash_size // 1024))
        if reply[4] != 0x00:
            raise Exception('Failed to erase chip')

    def __exitv1(self):
        self.epout.write(CH_STR_REBOOT_V1)

    def __exitv2(self):
        self.epout.write(CH_STR_REBOOT_V2)

    def __writev1(self, data, mode):
        # Send data in chunks of 60 bytes
        data   = bytearray(data)
        rest   = len(data)
        offset = 0
        while rest > 0:
            if rest >= 0x3c:  pkt_len = 0x3c
            else:             pkt_len = rest
            stream  = bytes((mode, pkt_len))
            stream += offset.to_bytes(2, byteorder='little')
            stream += data[offset:(offset + pkt_len)]
            reply   = self.__sendcmd(stream)
            if reply[0] != 0x00:
                if mode == CH_CMD_CODE_WRITE_V1:
                    raise Exception('Failed to write at offset 0x%08x' % offset)
                elif mode == CH_CMD_CODE_VERIFY_V1:
                    raise Exception('Failed to verify at offset 0x%08x' % offset)
            offset += pkt_len
            rest   -= pkt_len

    def __writev2(self, data, mode):
        # Encrypt data
        if (len(data) % CH_XOR_KEY_LEN) > 0:
            data += b'0xff' * (CH_XOR_KEY_LEN - (len(data) % CH_XOR_KEY_LEN))
        data = bytearray(data)
        for x in range(len(data)):
            data[x] ^= self.xorkey[x % CH_XOR_KEY_LEN]

        # Send data in chunks of 56 bytes
        rest   = len(data)
        offset = 0
        while rest > 0:
            if rest >= 0x38:  pkt_len = 0x38
            else:             pkt_len = rest
            stream  = bytes((mode, pkt_len + 5, 0))
            stream += offset.to_bytes(4, byteorder='little')
            stream += (rest & 0xff).to_bytes(1, byteorder='little')
            stream += data[offset:(offset + pkt_len)]
            reply   = self.__sendcmd(stream)
            if reply[4] != 0x00 and reply[4] != 0xfe and reply[4] != 0xf5:
                if mode == CH_CMD_CODE_WRITE_V2:
                    raise Exception('Failed to write at offset 0x%08x' % offset)
                elif mode == CH_CMD_CODE_VERIFY_V2:
                    raise Exception('Failed to verify at offset 0x%08x' % offset)
            offset += pkt_len
            rest   -= pkt_len

# ===================================================================================
# CH55x Protocol Constants
# ===================================================================================

CH_USB_VENDOR_ID      = 0x4348    # VID
CH_USB_PRODUCT_ID     = 0x55e0    # PID
CH_USB_PACKET_SIZE    = 64        # packet size
CH_USB_TIMEOUT        = 5000      # timeout for USB operations

CH_CMD_CHIP_TYPE_V2   = 0xa2
CH_CMD_REBOOT_V2      = 0xa5
CH_CMD_CODE_ERASE_V2  = 0xa6
CH_CMD_CODE_VERIFY_V1 = 0xa7
CH_CMD_CODE_WRITE_V1  = 0xa8

CH_CMD_CHIP_TYPE_V2   = 0xa1
CH_CMD_REBOOT_V2      = 0xa2
CH_CMD_KEY_SET_V2     = 0xa3
CH_CMD_CODE_ERASE_V2  = 0xa4
CH_CMD_CODE_WRITE_V2  = 0xa5
CH_CMD_CODE_VERIFY_V2 = 0xa6

CH_STR_CHIP_DETECT_V1 = (0xa2, 0x13, 0x55, 0x53, 0x42, 0x20, 0x44, 0x42, 0x47, 0x20, 0x43, 
                         0x48, 0x35, 0x35, 0x39, 0x20, 0x26, 0x20, 0x49, 0x53, 0x50, 0x00)
CH_STR_CONFIG_READ_V1 = (0xbb, 0x00)
CH_STR_CODE_ERASE_V1  = (0xa6, 0x04, 0x00, 0x00, 0x00, 0x00)
CH_STR_REBOOT_V1      = (0xa5, 0x02, 0x01, 0x00)

CH_STR_CHIP_DETECT_V2 = (0xa1, 0x12, 0x00, 0x52, 0x11, 0x4d, 0x43, 0x55, 0x20, 0x49, 0x53, 
                         0x50, 0x20, 0x26, 0x20, 0x57, 0x43, 0x48, 0x2e, 0x43, 0x4e)
CH_STR_CONFIG_READ_V2 = (0xa7, 0x02, 0x00, 0x1f, 0x00)
CH_STR_CODE_ERASE_V2  = (0xa4, 0x04, 0x00, 0x08, 0x00, 0x00, 0x00)
CH_STR_REBOOT_V2      = (0xa2, 0x01, 0x00, 0x01)

CH_XOR_KEY_LEN        = 8

# ===================================================================================
# Device definitions
# ===================================================================================

DEVICES = [
    {'name': 'CH551', 'id': 0x1151, 'code_size': 10240, 'data_size':  128},
    {'name': 'CH552', 'id': 0x1152, 'code_size': 14336, 'data_size':  128},
    {'name': 'CH554', 'id': 0x1154, 'code_size': 14336, 'data_size':  128},
    {'name': 'CH555', 'id': 0x1155, 'code_size': 61440, 'data_size': 1024},
    {'name': 'CH556', 'id': 0x1156, 'code_size': 61440, 'data_size': 1024},
    {'name': 'CH557', 'id': 0x1157, 'code_size': 61440, 'data_size': 1024},
    {'name': 'CH558', 'id': 0x1158, 'code_size': 61440, 'data_size': 1024},
    {'name': 'CH559', 'id': 0x1159, 'code_size': 61440, 'data_size': 1024}
]

# ===================================================================================

if __name__ == "__main__":
    _main()
