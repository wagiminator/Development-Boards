#!/usr/bin/env python3
# ===================================================================================
# Project:   stc8usb - USB Programming Tool for STC8H8KxxU Microcontrollers
# Version:   v0.2
# Year:      2023
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   MIT License
# ===================================================================================
#
# Description:
# ------------
# Simple Python tool for flashing STC8H8KxxU microcontrollers via USB utilizing the 
# factory built-in embedded USB boot loader.
# !!! THIS TOOL IS IN AN EARLY STAGE OF DEVELOPMENT !!!
#
# Dependencies:
# -------------
# - pyhidapi
#
# Operating Instructions:
# -----------------------
# You need to install pyhidapi to use stc8usb.
# Install it via "python3 -m pip install hid".
#
# By default Linux will not expose enough permission to access the USB bootloader.
# In order to fix this, open a terminal and run the following commands:
# echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="34bf", ATTR{idProduct}=="1001", MODE="666"' | sudo tee /etc/udev/rules.d/99-STC-ISP.rules
# sudo udevadm control --reload-rules
#
# The bootloader must be started manually for new uploads. To do this, the board must
# first be disconnected from the USB port and all voltage sources. Now press the BOOT
# button and keep it pressed while reconnecting the board to the USB port of your PC.
# The chip now starts in bootloader mode, the BOOT button can be released and new
# firmware can be uploaded via USB. If there is no BOOT button on the board, you have
# to short pin P3.2 to ground while connecting.
#
# Run "python3 stc8usb.py -t 24000000 -f firmware.bin".

# ===================================================================================
# Libraries
# ===================================================================================

import sys
import hid
import argparse

# ===================================================================================
# Main Function
# ===================================================================================

def _main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Minimal command line interface for stc8usb')
    parser.add_argument('-t', '--trim',  help='trim IRC to frequency in Hz')
    parser.add_argument('-e', '--erase', action='store_true', help='perform chip erase (implied with -f)')
    parser.add_argument('-f', '--flash', help='write BIN file to flash')
    args = parser.parse_args(sys.argv[1:])

    # Establish connection to USB-to-serial converter
    try:
        print('Connecting to MCU via USB ...')
        isp = Programmer()
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)

    # Performing actions
    try:
        # Connect to and identify MCU
        isp.connect()
        print('SUCCESS: Found %s v%s @ %s.' % (isp.chipname, isp.chipverstr, isp.foscstr))

        # Perform chip erase
        if (args.erase) or (args.flash is not None) or (args.trim is not None):
            print('Performing chip erase ...')
            isp.erase()
            print('SUCCESS: Chip is erased.')

        # Flash binary file
        if args.flash is not None:
            print('Flashing %s to %s ...' % (args.flash, isp.chipname))
            with open(args.flash, 'rb') as f: data = f.read()
            isp.writeflash(0, data)
            print('SUCCESS: %d bytes written.' % len(data))

        # Set frequency
        if args.trim is not None:
            print('Setting frequency ...')
            isp.setfreq(int(args.trim))
            print('SUCCESS: Frequency set to %s Hz.' % args.trim)

        # Close connection
        isp.disconnect()

    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        isp.close()
        sys.exit(1)

    print('DONE.')
    sys.exit(0)

# ===================================================================================
# Programmer Class
# ===================================================================================

class Programmer:
    # Init programmer
    def __init__(self):
        # Find programmer
        try:
            self.dev = hid.Device(STC_VID, STC_PID)
        except:
            raise Exception('Unable to open device')

    # Close connection   
    def close(self):
        self.dev.close()

    #--------------------------------------------------------------------------------

    # Transmit data block
    def transmit(self, data):
        size   = len(data) + 6
        parity = STC_TX_CODE + (size >> 8) + (size & 0xff)
        for x in range(len(data)):
            parity += data[x]
        block  = [STC_PREFIX, STC_PREFIX ^ 0xff, STC_TX_CODE]
        block += size.to_bytes(2, byteorder='big')
        block += data
        block += parity.to_bytes(2, byteorder='big')
        block += [STC_SUFFIX]
        block  = self.paddata(bytes(block), STC_HID_SIZE, 0x00)
        self.dev.write(block)

    # Receive data block
    def receive(self):
        reply = self.dev.read(STC_HID_SIZE, STC_TIMEOUT)
        if len(reply) != STC_HID_SIZE:
            raise Exception('No response from device')
        size = int.from_bytes(reply[3:5], byteorder='big')
        return reply[5:size-1]

    #--------------------------------------------------------------------------------

    # Connect to and identify MCU
    def connect(self):
        self.transmit([STC_CMD_INIT])
        reply = self.receive()

        # Get chip ID
        self.chipid = int.from_bytes(reply[20:22], byteorder='big')
        
        # Find chip in dictionary
        self.device = None
        for d in STC_DEVICES:
            if d['id'] == self.chipid:
                self.device = d
        if self.device is None:
            raise Exception('Unsupported chip (ID: 0x%04x)' % self.chipid)
        self.chipname   = self.device['name']
        self.flash_size = self.device['flash_size']

        # Get chip version
        self.chipversion  = reply[17]
        self.chipverstr   = '%d.%d.%d%c' % (reply[17] >> 4, reply[17] & 0x0f, \
                                            reply[22] & 0x0f, reply[18])

        # Get USER-IRC oscillator frequency
        self.fosc = int.from_bytes(reply[1:5], byteorder='big')
        if self.fosc == 0xffffffff:
            self.foscstr = 'untrimmed frequency'
        else:
            self.foscstr = str(self.fosc) + ' Hz'
        
        # Set BAUD (dummy)
        self.transmit([STC_CMD_BAUD_SET, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80])
        self.receive()
        self.transmit([STC_CMD_BAUD_CHECK, 0x00, 0x00, 0x5a, 0xa5])
        self.receive()

    # Disconnect from MCU
    def disconnect(self):
        self.transmit([STC_CMD_EXIT])
        self.dev.close()

    #--------------------------------------------------------------------------------

    # Erase flash
    def erase(self):
        self.transmit([STC_CMD_ERASE, 0, 0, STC_BREAK, STC_BREAK ^ 0xff])
        reply = self.receive()
        if reply is None or reply[0] != STC_CMD_ERASE:
            raise Exception('Failed to erase flash')

    # Write data to flash
    def writeflash(self, addr, data):
        data = self.paddata(data, STC_PAGE_SIZE, 0xff)
        size = len(data)
        if size > (self.flash_size - addr):
            raise Exception('Not enough memory')
        cmd = STC_CMD_WRITE
        while size > 0:
            blocksize = min(size, STC_PAGE_SIZE)
            block  = [cmd]
            block += addr.to_bytes(2, byteorder='big')
            block += [STC_BREAK, STC_BREAK ^ 0xff]
            block += data[:blocksize]
            self.transmit(block)
            reply = self.receive()
            if reply is None or reply[0] != STC_CMD_WRITE_CONT:
                raise Exception('Failed to write to address 0x%04x' % addr)
            cmd   = STC_CMD_WRITE_CONT
            data  = data[blocksize:]
            addr += blocksize
            size -= blocksize

    # Set frequency by writing option bytes
    def setfreq(self, freq):
        trim = None
        for f in STC_FREQUENCIES:
            if f['freq'] == freq:
                trim = f['trim']
        if trim is None:
            raise Exception('Unsupported frequency')
        block  = [STC_CMD_OPTIONS, 0, 0, STC_BREAK, STC_BREAK ^ 0xff]
        block += STC_BLOCK_OPTIONS1
        block += freq.to_bytes(4, byteorder='big')
        block += trim
        block += STC_BLOCK_OPTIONS2
        self.transmit(block)
        reply = self.receive()
        if reply is None or reply[0] != STC_CMD_OPTIONS:
            raise Exception('Failed to write option bytes')

    # Pad data
    def paddata(self, data, pagesize, filler):
        if (len(data) % pagesize) > 0:
            data += bytes([filler]) * (pagesize - (len(data) % pagesize))
        return data

# ===================================================================================
# Device Constants
# ===================================================================================

STC_VID            = 0x34bf
STC_PID            = 0x1001
STC_HID_SIZE       = 64
STC_PAGE_SIZE      = 128
STC_TIMEOUT        = 1000

STC_PREFIX         = 0x46
STC_BREAK          = 0x5a
STC_SUFFIX         = 0x16
STC_TX_CODE        = 0x6a
STC_RX_CODE        = 0x68
STC_RPL_INFO       = 0x50

STC_CMD_INIT       = 0x00
STC_CMD_BAUD_SET   = 0x01
STC_CMD_BAUD_CHECK = 0x05
STC_CMD_ERASE      = 0x03
STC_CMD_OPTIONS    = 0x04
STC_CMD_WRITE      = 0x32
STC_CMD_WRITE_CONT = 0x02
STC_CMD_EXIT       = 0xff

STC_BLOCK_OPTIONS1 = [0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 
                      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff]
STC_BLOCK_OPTIONS2 = [0xff, 0xff, 0xff, 0xff, 0x3e, 0xbf, 0xaf, 0xf7, 0xfe]

# ===================================================================================
# Device Dictionary
# ===================================================================================

STC_DEVICES = [
    {'name': 'STC8H8K16U', 'id': 0xF781, 'flash_size':  16384},
    {'name': 'STC8H8K32U', 'id': 0xF782, 'flash_size':  32768},
    {'name': 'STC8H8K60U', 'id': 0xF783, 'flash_size':  61440},
    {'name': 'STC8H8K64U', 'id': 0xF784, 'flash_size':  65024},
    {'name': 'STC8H8K48U', 'id': 0xF785, 'flash_size':  49152}
]

STC_FREQUENCIES = [
    {'freq':  1000000, 'trim': [0x48, 0x20, 24]},
    {'freq':  2000000, 'trim': [0x48, 0x20, 12]},
    {'freq':  3000000, 'trim': [0x48, 0x20,  8]},
    {'freq':  4000000, 'trim': [0x48, 0x20,  6]},
    {'freq':  5000000, 'trim': [0x73, 0x30,  8]},
    {'freq':  6000000, 'trim': [0x48, 0x20,  4]},
    {'freq':  8000000, 'trim': [0x48, 0x20,  3]},
    {'freq': 10000000, 'trim': [0x73, 0x30,  4]},
    {'freq': 12000000, 'trim': [0x48, 0x20,  2]},
    {'freq': 16000000, 'trim': [0xc6, 0x30,  3]},
    {'freq': 20000000, 'trim': [0x73, 0x30,  2]},
    {'freq': 24000000, 'trim': [0x48, 0x20,  1]},
    {'freq': 27000000, 'trim': [0x7a, 0x20,  1]},
    {'freq': 30000000, 'trim': [0xb1, 0x20,  1]},
    {'freq': 35000000, 'trim': [0x44, 0x30,  1]},
    {'freq': 40000000, 'trim': [0x73, 0x30,  1]},
    {'freq': 48000000, 'trim': [0xc6, 0x30,  1]}
]

# ===================================================================================

if __name__ == "__main__":
    _main()
