# ===================================================================================
# Project Makefile
# ===================================================================================
# Project:  ATtiny3216 Example
# Author:   Stefan Wagner
# Year:     2021
# URL:      https://github.com/wagiminator    
# ===================================================================================
# Install toolchain:
#   sudo apt install build-essential gcc-avr avr-libc
#   sudo apt install python3 python3-pip
#   pip install tinyupdi
#
# Type "make flash" in the command line.
# ===================================================================================

# Files and Folders
TARGET   = i2c_oled
INCLUDE  = include
SOURCE   = src
BIN      = bin
AVRDEV   = avr

# Microcontroller Settings
DEVICE  ?= attiny3216
F_CPU    = 20000000
FUSE0    = 0x00
FUSE1    = 0x00
FUSE4    = 0x00
FUSE5    = 0xC5
FUSE6    = 0x04
FUSE7    = 0x00
FUSE8    = 0x00

# Toolchain
PREFIX   = avr
CC       = $(PREFIX)-gcc
OBJCOPY  = $(PREFIX)-objcopy
OBJDUMP  = $(PREFIX)-objdump
OBJSIZE  = $(PREFIX)-size
ISPTOOL  = tinyupdi
CLEAN    = rm -f *.lst *.obj *.cof *.list *.map *.eep.hex *.o *.d

# Compiler Flags
CFLAGS   = -Os -flto -Wall -mmcu=$(DEVICE) -DF_CPU=$(F_CPU) -ffunction-sections -fdata-sections
CFLAGS  += -I$(AVRDEV) -I$(INCLUDE) -I$(SOURCE) -I.
LDFLAGS  = -B$(AVRDEV)/dev/$(DEVICE) -Wl,--gc-sections
CFILES   = $(wildcard ./*.c) $(wildcard $(SOURCE)/*.c) $(wildcard $(SOURCE)/*.S)

.PHONY: help all elf bin hex asm flash fuses clean size removetemp removeelf

# Symbolic Targets
help:
	@echo "Use the following commands:"
	@echo "make all       compile and build $(TARGET).elf/.bin/.hex/.asm"
	@echo "make hex       compile and build $(TARGET).hex"
	@echo "make asm       compile and disassemble to $(TARGET).asm"
	@echo "make bin       compile and build $(TARGET).bin"
	@echo "make flash     compile and upload to MCU"
	@echo "make fuses     write fuses to MCU"
	@echo "make clean     remove all build files"

$(BIN)/$(TARGET).elf: $(CFILES)
	@echo "Building $(BIN)/$(TARGET).elf ..."
	@mkdir -p $(BIN)
	@$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)

$(BIN)/$(TARGET).lst: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).lst ..."
	@$(OBJDUMP) -S $^ > $(BIN)/$(TARGET).lst

$(BIN)/$(TARGET).map: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).map ..."
	@$(OBJDUMP) -t $^ > $(BIN)/$(TARGET).map

$(BIN)/$(TARGET).bin: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).bin ..."
	@$(OBJCOPY) -O binary $< $(BIN)/$(TARGET).bin

$(BIN)/$(TARGET).hex: $(BIN)/$(TARGET).elf
	@echo "Building $(BIN)/$(TARGET).hex ..."
	@$(OBJCOPY) -O ihex $< $(BIN)/$(TARGET).hex

$(BIN)/$(TARGET).asm: $(BIN)/$(TARGET).elf
	@echo "Disassembling to $(BIN)/$(TARGET).asm ..."
	@$(OBJDUMP) -d $(BIN)/$(TARGET).elf > $(BIN)/$(TARGET).asm

all:	$(BIN)/$(TARGET).lst $(BIN)/$(TARGET).map $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex $(BIN)/$(TARGET).asm size

elf:	$(BIN)/$(TARGET).elf removetemp size

bin:	$(BIN)/$(TARGET).bin removetemp size removeelf

hex:	$(BIN)/$(TARGET).hex removetemp size removeelf

asm:	$(BIN)/$(TARGET).asm removetemp size removeelf

flash:	$(BIN)/$(TARGET).bin size removeelf
	@echo "Uploading to $(DEVICE) ..."
	@$(ISPTOOL) -d $(DEVICE) -t $(F_CPU) -f $(BIN)/$(TARGET).bin -fs 6:$(FUSE6) 7:$(FUSE7) 8:$(FUSE8)

fuses:
	@echo "Writing fuses to $(DEVICE) ..."
	@$(ISPTOOL) -t $(F_CPU) -fs 0:$(FUSE0) 1:$(FUSE1) 4:$(FUSE4) 5:$(FUSE5) 6:$(FUSE6) 7:$(FUSE7) 8:$(FUSE8)

clean:
	@echo "Cleaning all up ..."
	@$(CLEAN)
	@rm -f $(BIN)/$(TARGET).elf $(BIN)/$(TARGET).lst $(BIN)/$(TARGET).map $(BIN)/$(TARGET).bin $(BIN)/$(TARGET).hex $(BIN)/$(TARGET).asm

size:
	@echo "------------------"
	@echo "FLASH: $(shell $(OBJSIZE) -d $(BIN)/$(TARGET).elf | awk '/[0-9]/ {print $$1 + $$2}') bytes"
	@echo "SRAM:  $(shell $(OBJSIZE) -d $(BIN)/$(TARGET).elf | awk '/[0-9]/ {print $$2 + $$3}') bytes"
	@echo "------------------"

removetemp:
	@echo "Removing temporary files ..."
	@$(CLEAN)

removeelf:
	@echo "Removing $(BIN)/$(TARGET).elf ..."
	@rm -f $(BIN)/$(TARGET).elf
