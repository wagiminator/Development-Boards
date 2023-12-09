#!/usr/bin/env python3
# ===================================================================================
# Project:   stc8isp - Programming Tool for STC8G/8H Microcontrollers
# Version:   v0.3
# Year:      2023
# Author:    Stefan Wagner
# Github:    https://github.com/wagiminator
# License:   MIT License
# ===================================================================================
#
# Description:
# ------------
# Simple Python tool for flashing STC8G/8H microcontrollers via USB-to-serial 
# converter utilizing the factory built-in embedded boot loader.
# !!! THIS TOOL IS IN AN EARLY STAGE OF DEVELOPMENT !!!
#
# Dependencies:
# -------------
# - pyserial
#
# Operating Instructions:
# -----------------------
# You need to install PySerial to use stc8isp.
# Install it via "python3 -m pip install pyserial".
# You may need to install a driver for your USB-to-serial converter.
#
# Connect your USB-to-serial converter to your MCU:
# USB2SERIAL         STC8G/8H
#        VCC --/ --> VCC        interruptible (for power cycle)
#        RXD --|R|-- P3.1       resistor (100R - 1000R)
#        TXD --|<|-- P3.0       diode (e.g. 1N5819)
#        GND ------- GND        common ground
#
# Run "python3 stc8isp.py -p /dev/ttyUSB0 -t 24000000 -f firmware.bin".
# Perform a power cycle of your MCU (reconnect to power) when prompted.


# ===================================================================================
# Software Settings
# ===================================================================================

# If the PID/VID of the USB-to-Serial converter is known, it can be defined here.
# The specified COM port is then ignored, and all ports are automatically searched 
# for the device. Comment the lines to ignore PID/VID.
STC_VID = '1A86'
STC_PID = '7523'

# Define the default COM port here. This will be used if no VID/PID is defined and
# no COM port is specified within the command line arguments.
STC_PORT = '/dev/ttyUSB0'

# Define the default BAUD rates here. The minimum BAUD rate is used to calibrate the 
# oscillator and should be low to achieve higher accuracy. The maximum BAUD rate is 
# used to transfer the firmware and should be high to achieve higher speed.
STC_BAUD_MIN =   2400
STC_BAUD_MAX = 115200

# Define time to wait for power cycle in seconds, default: 10
STC_WAIT = 10

# Define minimum and maximum IRC frequency in Hz, that can be specified by the
# respective command line argument. Expanding the limit values can lead to problems!
STC_FCPU_MIN =   128000
STC_FCPU_MAX = 36000000

# ===================================================================================
# Libraries
# ===================================================================================

# Libraries
import sys
import time
import argparse
import serial
from serial import Serial
from serial.tools.list_ports import comports

# ===================================================================================
# Main Function
# ===================================================================================

def _main():
    # Parse command line arguments
    parser = argparse.ArgumentParser(description='Minimal command line interface for stc8isp')
    parser.add_argument('-p', '--port',  default=STC_PORT, help='set COM port')
    parser.add_argument('-t', '--trim',  help='trim IRC to frequency in Hz')
    parser.add_argument('-e', '--erase', action='store_true', help='perform chip erase (implied with -f)')
    parser.add_argument('-f', '--flash', help='write BIN file to flash')
    args = parser.parse_args(sys.argv[1:])

    # Checking command line arguments
    if (args.trim is not None) and (int(args.trim) < STC_FCPU_MIN or int(args.trim) > STC_FCPU_MAX):
        sys.stderr.write('ERROR: Trim value out of range (%d - %d)!\n' % (STC_FCPU_MIN, STC_FCPU_MAX))
        sys.exit(1)

    # Establish connection to USB-to-serial converter
    try:
        print('Connecting to USB-to-serial converter ...')
        isp = Programmer(args.port)
        print('SUCCESS: Connected via %s.' % isp.port)
    except Exception as ex:
        sys.stderr.write('ERROR: ' + str(ex) + '!\n')
        sys.exit(1)

    # Performing actions
    try:
        # Connect to and identify MCU
        print('Waiting for MCU power cycle ...')
        isp.connect()
        print('SUCCESS: Found %s v%s @ %s.' % (isp.chipname, isp.chipverstr, isp.foscstr))

        # Calibrate user IRC oscillator frequency
        if args.trim is not None:
            print('Calibrating user IRC frequency to %s Hz ...' % args.trim)
            isp.trim(int(args.trim))
            print('SUCCESS: Trimmed to %d Hz (error %.2f%%).' % (isp.osc_freq, 100 * isp.osc_error))

        # Switch BAUD rate
        print('Switching BAUD rate ...')
        isp.setbaud(STC_BAUD_MAX)
        print('SUCCESS: BAUD rate set to %d.' % STC_BAUD_MAX)

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

        # Write option bytes
        if args.trim is not None:
            print('Writing option bytes ...')
            isp.writeoptions()
            print('SUCCESS: Option bytes written.')

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

class Programmer(Serial):
    def __init__(self, port=STC_PORT, baud=STC_BAUD_MIN):
        # Data frame: 1 start bit, 8 data bit, 1 parity bit set to even, 1 stop bit
        super().__init__(baudrate = baud, parity = serial.PARITY_EVEN, timeout = 0.01)

        # Use COM port to define device
        if 'STC_VID' not in globals() or 'STC_PID' not in globals():
            self.port = port

        # Use VID/PID to find device
        else:
            for p in comports():
                if (STC_VID in p.hwid) and (STC_PID in p.hwid):
                    self.port = p.device
                    break

        # Open connection
        try:
            self.open()
        except:
            raise Exception('Failed to connect to USB-to-serial converter')

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
        self.write(block)

    # Receive and check data block
    def receive(self):
        reply = self.read(1)
        if len(reply) == 0 or reply[0] != STC_PREFIX:
            return None
        self.timeout = 1
        reply = self.read(2)
        if len(reply) != 2 or reply[0] != (STC_PREFIX ^ 0xff) or reply[1] != STC_RX_CODE:
            raise Exception('Invalid data prefix from MCU')
        size = int.from_bytes(self.read(2), byteorder='big')
        if size < 7:
            raise Exception('Invalid data size from MCU')
        data = self.read(size - 6)
        if len(data) != size - 6:
            raise Exception('Missing data from MCU')
        check  = int.from_bytes(self.read(2), byteorder='big')
        suffix = self.read(1)[0]
        if suffix != STC_SUFFIX:
            raise Exception('Invalid data suffix from MCU')
        parity = STC_RX_CODE + (size >> 8) + (size & 0xff)
        for x in range(len(data)):
            parity += data[x]
        if (parity & 0xffff) != check:
            raise Exception('Invalid data checksum from MCU')
        return data

    # Repeat sending sync character until reply or timeout
    def pulse(self, char, pulsetime=0.01, timeout=10):
        counter = timeout
        while self.in_waiting == 0:
            self.write([char])
            time.sleep(pulsetime)
            counter -= pulsetime
            if counter <= 0:
                raise Exception('Timout waiting for response')

    #--------------------------------------------------------------------------------

    # Connect to and identify MCU
    def connect(self):
        # Ping MCU, wait for power cycle, read info bytes
        self.reset_input_buffer()
        waitcounter = STC_WAIT * 100
        reply = None
        while (waitcounter > 0) and (reply is None):
            self.write([STC_CHAR_SYNC])
            reply = self.receive()
            waitcounter -= 1
        if waitcounter == 0:
            raise Exception('Timeout, failed to connect to MCU')
        if reply is None or len(reply) < 42 or reply[0] != STC_RPL_INFO:
            raise Exception('Invalid response from MCU')

        # Get chip ID
        self.chipid = int.from_bytes(reply[20:22], byteorder='big')
        
        # Find chip in dictionary
        self.device = None
        for d in DEVICES:
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

        # Get ISP-IRC and USER-IRC oscillator frequencies
        self.fisp = int.from_bytes(reply[13:15], byteorder='big') * self.baudrate
        self.fosc = int.from_bytes(reply[1:5], byteorder='big')
        if self.fosc == 0xffffffff:
            self.foscstr = 'untrimmed frequency'
        else:
            self.foscstr = str(self.fosc) + ' Hz'

        # Get option bytes
        self.options = reply[9:12] + reply[15:17]

    # Disconnect from MCU and USB-to-serial converter
    def disconnect(self):
        self.transmit([STC_CMD_EXIT])
        self.close()

    #--------------------------------------------------------------------------------

    # Set and calibrate USER-IRC oscillator frequency
    def trim(self, freq):
        # Get global trim values
        self.transmit(STC_BLOCK_TRIM)
        self.pulse(STC_CHAR_TRIM)
        reply = self.receive()
        if reply is None or reply[0] != STC_CMD_TRIM:
                raise Exception('Invalid responde from MCU')

        # Calculate mean trim value for specified frequency
        freq1 = int.from_bytes(reply[ 2: 4], byteorder='big') * self.baudrate
        freq2 = int.from_bytes(reply[ 4: 6], byteorder='big') * self.baudrate
        freq3 = int.from_bytes(reply[ 6: 8], byteorder='big') * self.baudrate
        freq4 = int.from_bytes(reply[ 8:10], byteorder='big') * self.baudrate
        freq5 = int.from_bytes(reply[10:12], byteorder='big') * self.baudrate
        self.osc_div = 1
        while freq * self.osc_div < freq1: self.osc_div += 1
        if freq * self.osc_div < freq5:
            self.osc_trimx = round( (freq * self.osc_div - freq1) * 
                (STC_BLOCK_TRIM[4] - STC_BLOCK_TRIM[2]) / (freq2 - freq1) + STC_BLOCK_TRIM[2] )
            self.osc_bandx = STC_BLOCK_TRIM[3]
        else:
            self.osc_trimx = round( (freq * self.osc_div - freq3) * 
                (STC_BLOCK_TRIM[8] - STC_BLOCK_TRIM[6]) / (freq4 - freq3) + STC_BLOCK_TRIM[2] )
            self.osc_bandx = STC_BLOCK_TRIM[7]

        # Get fine tuned trim values for specified frequency
        count = 24
        block = [0x00, count]
        for x in range(count):
            block += [self.osc_trimx - 2 + (x>>2), self.osc_bandx + (x & 0x03)]
        self.transmit(block)
        self.pulse(STC_CHAR_TRIM)
        reply = self.receive()
        if reply is None or reply[0] != STC_CMD_TRIM:
                raise Exception('Invalid responde from MCU')

        # Find optimal trim value
        best = sys.maxsize
        for x in range(count):
            freq1 = int.from_bytes(reply[2*x+2:2*x+4], byteorder='big') * self.baudrate
            if abs(freq * self.osc_div - freq1) < best:
                best = abs(freq * self.osc_div - freq1)
                self.osc_freq  = round(freq1 / self.osc_div)
                self.osc_trim  = self.osc_trimx - 2 + (x>>2)
                self.osc_band  = self.osc_bandx + (x & 0x03)
                self.osc_error = (self.osc_freq - freq) / freq

    # Set/switch BAUD rate
    def setbaud(self, baud=STC_BAUD_MAX):
        # Set BAUD rate
        count  = round(65536 - (self.fisp / (4 * baud)))
        block  = [STC_CMD_BAUD_SET, self.fosc & 0xff, 0x40]
        block += count.to_bytes(2, byteorder='big')
        block += [0x00, 0x00, 0x97]
        self.transmit(block)
        reply = self.receive()
        if reply is None or reply[0] != STC_CMD_BAUD_SET:
            raise Exception('Failed to set BAUD rate')
        self.baudrate = baud
        time.sleep(0.01)

        # Check BAUD rate
        if self.chipversion < 0x72:
            self.transmit([STC_CMD_BAUD_CHECK])
        else:
            self.transmit([STC_CMD_BAUD_CHECK, 0, 0, STC_BREAK, STC_BREAK ^ 0xff])
        reply = self.receive()
        if reply is None or reply[0] != STC_CMD_BAUD_CHECK:
            raise Exception('Failed to check BAUD rate')

    #--------------------------------------------------------------------------------

    # Erase flash
    def erase(self):
        self.transmit([STC_CMD_ERASE, 0, 0, STC_BREAK, STC_BREAK ^ 0xff])
        reply = self.receive()
        if reply is None or reply[0] != STC_CMD_ERASE:
            raise Exception('Failed to erase flash')

    # Write data to flash
    def writeflash(self, addr, data):
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

    # Write option bytes
    def writeoptions(self):
        block  = [STC_CMD_OPTIONS, 0, 0, STC_BREAK, STC_BREAK ^ 0xff]
        block += STC_BLOCK_OPTIONS1
        block += self.osc_freq.to_bytes(4, byteorder='big')
        block += [self.osc_trim, self.osc_band, self.osc_div]
        block += STC_BLOCK_OPTIONS2
        self.transmit(block)
        reply = self.receive()
        if reply is None or reply[0] != STC_CMD_OPTIONS:
            raise Exception('Failed to write option bytes')
               
# ===================================================================================
# Device Constants
# ===================================================================================

STC_PAGE_SIZE      = 128

STC_CHAR_SYNC      = 0x7f
STC_CHAR_TRIM      = 0x66

STC_PREFIX         = 0x46
STC_BREAK          = 0x5a
STC_SUFFIX         = 0x16
STC_TX_CODE        = 0x6a
STC_RX_CODE        = 0x68
STC_RPL_INFO       = 0x50

STC_CMD_TRIM       = 0x00
STC_CMD_BAUD_SET   = 0x01
STC_CMD_BAUD_CHECK = 0x05
STC_CMD_ERASE      = 0x03
STC_CMD_OPTIONS    = 0x04
STC_CMD_WRITE      = 0x22
STC_CMD_WRITE_CONT = 0x02
STC_CMD_EXIT       = 0xff

STC_BLOCK_TRIM     = [0x00, 0x05, 0x02, 0x00, 0x80, 0x00, 0x00, 0x80, 
                      0x80, 0x80, 0xfd, 0x00]
STC_BLOCK_OPTIONS1 = [0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0x00, 0xff, 
                      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 
                      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0xff]
STC_BLOCK_OPTIONS2 = [0xff, 0xff, 0xff, 0xff, 0xff, 0xbf, 0xbf, 0xf7, 0xff]

# ===================================================================================
# Device Dictionary
# ===================================================================================

DEVICES = [
    {'name': 'STC8H1K16',        'id': 0xF721, 'flash_size':  16384},
    {'name': 'STC8H1K20',        'id': 0xF722, 'flash_size':  20480},
    {'name': 'STC8H1K24',        'id': 0xF723, 'flash_size':  24576},
    {'name': 'STC8H1K28',        'id': 0xF724, 'flash_size':  28672},
    {'name': 'STC8H1K33',        'id': 0xF725, 'flash_size':  33792},
    {'name': 'STC8H1K02',        'id': 0xF731, 'flash_size':   2048},
    {'name': 'STC8H1K04',        'id': 0xF732, 'flash_size':   4096},
    {'name': 'STC8H1K06',        'id': 0xF733, 'flash_size':   6144},
    {'name': 'STC8H1K08',        'id': 0xF734, 'flash_size':   8192},
    {'name': 'STC8H1K10',        'id': 0xF735, 'flash_size':  10240},
    {'name': 'STC8H1K12',        'id': 0xF736, 'flash_size':  12288},
    {'name': 'STC8H1K17',        'id': 0xF737, 'flash_size':  17408},
    {'name': 'STC8H3K16S4',      'id': 0xF741, 'flash_size':  16384},
    {'name': 'STC8H3K32S4',      'id': 0xF742, 'flash_size':  32768},
    {'name': 'STC8H3K60S4',      'id': 0xF743, 'flash_size':  61440},
    {'name': 'STC8H3K64S4',      'id': 0xF744, 'flash_size':  65024},
    {'name': 'STC8H3K48S4',      'id': 0xF745, 'flash_size':  49152},
    {'name': 'STC8H3K16S2',      'id': 0xF749, 'flash_size':  16384},
    {'name': 'STC8H3K32S2',      'id': 0xF74A, 'flash_size':  32768},
    {'name': 'STC8H3K60S2',      'id': 0xF74B, 'flash_size':  61440},
    {'name': 'STC8H3K64S2',      'id': 0xF74C, 'flash_size':  65024},
    {'name': 'STC8H3K48S2',      'id': 0xF74D, 'flash_size':  49152},
    {'name': 'STC8G1K02-20/16P', 'id': 0xF751, 'flash_size':   2048},
    {'name': 'STC8G1K04-20/16P', 'id': 0xF752, 'flash_size':   4096},
    {'name': 'STC8G1K06-20/16P', 'id': 0xF753, 'flash_size':   6144},
    {'name': 'STC8G1K08-20/16P', 'id': 0xF754, 'flash_size':   8192},
    {'name': 'STC8G1K10-20/16P', 'id': 0xF755, 'flash_size':  10240},
    {'name': 'STC8G1K12-20/16P', 'id': 0xF756, 'flash_size':  12288},
    {'name': 'STC8G1K17-20/16P', 'id': 0xF757, 'flash_size':  17408},
    {'name': 'STC8G2K16S4',      'id': 0xF761, 'flash_size':  16384},
    {'name': 'STC8G2K32S4',      'id': 0xF762, 'flash_size':  32768},
    {'name': 'STC8G2K60S4',      'id': 0xF763, 'flash_size':  61440},
    {'name': 'STC8G2K64S4',      'id': 0xF764, 'flash_size':  65024},
    {'name': 'STC8G2K48S4',      'id': 0xF765, 'flash_size':  49152},
    {'name': 'STC8G2K16S2',      'id': 0xF769, 'flash_size':  16384},
    {'name': 'STC8G2K32S2',      'id': 0xF76A, 'flash_size':  32768},
    {'name': 'STC8G2K60S2',      'id': 0xF76B, 'flash_size':  61440},
    {'name': 'STC8G2K64S2',      'id': 0xF76C, 'flash_size':  65024},
    {'name': 'STC8G2K48S2',      'id': 0xF76D, 'flash_size':  49152},
    {'name': 'STC8G1K02T',       'id': 0xF771, 'flash_size':   2048},
    {'name': 'STC8G1K04T',       'id': 0xF772, 'flash_size':   4096},
    {'name': 'STC8G1K06T',       'id': 0xF773, 'flash_size':   6144},
    {'name': 'STC8G1K08T',       'id': 0xF774, 'flash_size':   8192},
    {'name': 'STC8G1K10T',       'id': 0xF775, 'flash_size':  10240},
    {'name': 'STC8G1K12T',       'id': 0xF776, 'flash_size':  12288},
    {'name': 'STC8G1K17T',       'id': 0xF777, 'flash_size':  17408},
    {'name': 'STC8H8K16U',       'id': 0xF781, 'flash_size':  16384},
    {'name': 'STC8H8K32U',       'id': 0xF782, 'flash_size':  32768},
    {'name': 'STC8H8K60U',       'id': 0xF783, 'flash_size':  61440},
    {'name': 'STC8H8K64U',       'id': 0xF784, 'flash_size':  65024},
    {'name': 'STC8H8K48U',       'id': 0xF785, 'flash_size':  49152},
    {'name': 'STC8G1K02A-8P',    'id': 0xF791, 'flash_size':   2048},
    {'name': 'STC8G1K04A-8P',    'id': 0xF792, 'flash_size':   4096},
    {'name': 'STC8G1K06A-8P',    'id': 0xF793, 'flash_size':   6144},
    {'name': 'STC8G1K08A-8P',    'id': 0xF794, 'flash_size':   8192},
    {'name': 'STC8G1K10A-8P',    'id': 0xF795, 'flash_size':  10240},
    {'name': 'STC8G1K12A-8P',    'id': 0xF796, 'flash_size':  12288},
    {'name': 'STC8G1K17A-8P',    'id': 0xF797, 'flash_size':  17408},
    {'name': 'STC8G1K02-8P',     'id': 0xF7A1, 'flash_size':   2048},
    {'name': 'STC8G1K04-8P',     'id': 0xF7A2, 'flash_size':   4096},
    {'name': 'STC8G1K06-8P',     'id': 0xF7A3, 'flash_size':   6144},
    {'name': 'STC8G1K08-8P',     'id': 0xF7A4, 'flash_size':   8192},
    {'name': 'STC8G1K10-8P',     'id': 0xF7A5, 'flash_size':  10240},
    {'name': 'STC8G1K12-8P',     'id': 0xF7A6, 'flash_size':  12288},
    {'name': 'STC8G1K17-8P',     'id': 0xF7A7, 'flash_size':  17408}
]

# ===================================================================================

if __name__ == "__main__":
    _main()
