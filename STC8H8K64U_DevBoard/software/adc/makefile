# ===================================================================================
# Project Makefile
# ===================================================================================
# Project:  STC8H8K64U Example
# Author:   Stefan Wagner
# Year:     2026
# URL:      https://github.com/wagiminator
# ===================================================================================         
# Install toolchain:
#   sudo apt install build-essential sdcc
#   sudo apt install python3 python3-pip
#   pip install stc8usb
#
# Provide access permission to USB bootloader:
#   echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="34bf", ATTR{idProduct}=="1001", MODE="666"' | sudo tee /etc/udev/rules.d/99-STC-ISP.rules
#   sudo udevadm control --reload-rules
#
# Type "make flash" in the command line.
# ===================================================================================

# Files and Folders
TARGET    = adc
INCLUDE   = include
SOURCE    = src
BIN       = bin

# Microcontroller Settings
F_CPU     = 24000000
IRAM_SIZE = 0x0100
XRAM_SIZE = 0x2000
CODE_SIZE = 0xfe00

# Toolchain
CC        = sdcc
OBJCOPY   = objcopy
PACK_HEX  = packihx
ISPTOOL   = stc8usb -t $(F_CPU) -f $(BIN)/$(TARGET).bin

# Compiler Flags
CFLAGS  = -mmcs51 --model-small --no-xinit-opt -DF_CPU=$(F_CPU) -I$(INCLUDE) -I$(SOURCE) -I.
CFLAGS += --iram-size $(IRAM_SIZE) --xram-size $(XRAM_SIZE) --code-size $(CODE_SIZE)
CFILES  = $(wildcard ./*.c) $(wildcard $(SOURCE)/*.c) $(wildcard $(INCLUDE)/*.c)
RFILES  = $(CFILES:.c=.rel)
CLEAN   = rm -f *.ihx *.lk *.map *.mem *.lst *.rel *.rst *.sym *.asm *.adb $(BIN)/*.ihx $(BIN)/*.lk $(BIN)/*.map $(BIN)/*.mem

# Symbolic Targets
help:
	@echo "Use the following commands:"
	@echo "make all     compile, build and keep all files"
	@echo "make hex     compile and build $(TARGET).hex"
	@echo "make bin     compile and build $(TARGET).bin"
	@echo "make flash   compile, build and upload $(TARGET).bin to device"
	@echo "make clean   remove all build files"

%.rel : %.c
	@echo "Compiling $< ..."
	@$(CC) -c $(CFLAGS) $<

$(BIN)/$(TARGET).ihx: $(RFILES)
	@echo "Building $(BIN)/$(TARGET).ihx ..."
	@mkdir -p $(BIN)
	@$(CC) $(notdir $(RFILES)) $(CFLAGS) -o $(BIN)/$(TARGET).ihx

$(BIN)/$(TARGET).hex: $(BIN)/$(TARGET).ihx
	@echo "Building $(BIN)/$(TARGET).hex ..."
	@$(PACK_HEX) $(BIN)/$(TARGET).ihx > $(BIN)/$(TARGET).hex

$(BIN)/$(TARGET).bin: $(BIN)/$(TARGET).ihx
	@echo "Building $(BIN)/$(TARGET).bin ..."
	@$(OBJCOPY) -I ihex -O binary $(BIN)/$(TARGET).ihx $(BIN)/$(TARGET).bin
	
flash: $(BIN)/$(TARGET).bin size removetemp
	@echo "Uploading to MCU ..."
	@$(ISPTOOL)

all: $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex size

hex: $(BIN)/$(TARGET).hex size removetemp

bin: $(BIN)/$(TARGET).bin size removetemp

bin-hex: $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex size removetemp

install: flash

size:
	@echo "------------------"
	@echo "FLASH: $(shell awk '$$1 == "ROM/EPROM/FLASH"    {print 0+$$4}' $(BIN)/$(TARGET).mem) bytes"
	@echo "IRAM:  $(shell awk '$$1 == "Stack"           {print 248-$$10}' $(BIN)/$(TARGET).mem) bytes"
	@echo "XRAM:  $(shell awk '$$1 == "EXTERNAL"           {print 0+$$5}' $(BIN)/$(TARGET).mem) bytes"
	@echo "------------------"

removetemp:
	@echo "Removing temporary files ..."
	@$(CLEAN)

clean:
	@echo "Cleaning all up ..."
	@$(CLEAN)
	@rm -f $(BIN)/$(TARGET).hex $(BIN)/$(TARGET).bin
