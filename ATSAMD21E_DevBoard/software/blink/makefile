# ===================================================================================
# Project Makefile
# ===================================================================================
# Project:  ATSAMD21E Example
# Author:   Stefan Wagner
# Year:     2026
# URL:      https://github.com/wagiminator    
# ===================================================================================
# Install toolchain:
#   sudo apt install build-essential gcc-arm-none-eabi
#   sudo apt install python3 python3-pip
#   pip install tinyocd
#
# Connect an SWD programmer (e.g. CMSIS-DAP) to the MCU board.
# Type "make flash" in the command line.
#
# To upload via an installed SAM-BA/UF2 bootloader (8KB, e.g. SAMD21E DevBoard, Arduino
# Zero) instead, set LDSCRIPT = ld/samd21xNN_boot8k.ld and ISPTOOL = $(BOOTTOOL) below.
# ===================================================================================

# Files and Folders
TARGET   = blink
INCLUDE  = include
SOURCE   = src
BIN      = bin

# Microcontroller Settings
DEVICE   = __SAMD21E18A__
F_CPU    = 48000000
LDSCRIPT = ld/samd21x18.ld
CPUARCH  = -mcpu=cortex-m0plus -mthumb

# Toolchain
PREFIX   = arm-none-eabi
CC       = $(PREFIX)-gcc
OBJCOPY  = $(PREFIX)-objcopy
OBJDUMP  = $(PREFIX)-objdump
OBJSIZE  = $(PREFIX)-size
ISPTOOL  = tinyocd -f $(BIN)/$(TARGET).bin
BOOTTOOL = bossac -i -U true -e -w -v -R --offset=0x2000 $(BIN)/$(TARGET).bin
CLEAN    = rm -f *.lst *.obj *.cof *.list *.map *.eep.hex *.o *.d

# Compiler Flags
CFLAGS   = -g -Os -flto $(CPUARCH) -DF_CPU=$(F_CPU) -D$(DEVICE) -I$(INCLUDE) -I$(SOURCE) -I.
CFLAGS  += -fdata-sections -ffunction-sections -fno-builtin -fno-common -Wall
LDFLAGS  = -T$(LDSCRIPT) -static -nostartfiles -lc -lm
LDFLAGS += -Wl,--gc-sections,--build-id=none --specs=nano.specs --specs=nosys.specs
LDFLAGS += $(shell $(CC) -Wl,--no-warn-rwx-segments -Wl,--version >/dev/null 2>&1 && echo -Wl,--no-warn-rwx-segments)
CFILES   = $(wildcard ./*.c) $(wildcard $(SOURCE)/*.c) $(wildcard $(SOURCE)/*.S)

# Symbolic Targets
help:
	@echo "Use the following commands:"
	@echo "make all       compile and build $(TARGET).elf/.bin/.hex/.asm"
	@echo "make hex       compile and build $(TARGET).hex"
	@echo "make asm       compile and disassemble to $(TARGET).asm"
	@echo "make bin       compile and build $(TARGET).bin"
	@echo "make flash     compile and upload to MCU"
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
	@echo "Uploading to MCU ..."
	@$(ISPTOOL)

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
