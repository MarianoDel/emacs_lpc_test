##
#    Makefile for Firmware Tests on LPC2292 Microprocessor
##

##
#    Default Section
##
TRGT = arm-none-eabi-
CC   = $(TRGT)gcc
CP   = $(TRGT)objcopy
AS   = $(TRGT)gcc
HEX  = $(CP) -O ihex
BIN  = $(CP) -O binary -S
MCU  = arm7tdmi-s

# List of used directories
LINKER_DIR = ./startup_src

# List all default C defines here, like -D_DEBUG=1
# DDEFS = -DSTM32F10X_MD

# Define project name and 
PROJECT        = blinky

## Sources Files
SRC  = ./src/main.c
SRC += ./src/lpc2294.c
# SRC += ./src/it.c
# SRC += ./src/usart.c
SRC += ./src/timer.c

## Assembler Sources Files
ASRC = ./startup_src/crt0.s
# ASRC += ./startup_src/config_crt0.s
# ASRC +=

# Optimization
 # Optimization-setting of the compiler. Chose -O0 when testing, -O1 or -O2
 # when everything seems to work OK. Other possible settings are -Os (when
 # size is important) or -O3 (when you feel like breaking stuff...)
OPT              = -O0

LDSCRIPT = $(LINKER_DIR)/lpc_flash.ld

MCFLAGS = -mcpu=$(MCU) -mtune=$(MCU)

ASFLAGS = $(MCFLAGS) -g -gdwarf-2 -mthumb

# Instrucciones Thumb
# CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -mthumb -fomit-frame-pointer -Wall -fdata-sections -ffunction-sections -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DDEFS)

# Instrucciones Arm
CPFLAGS = $(MCFLAGS) $(OPT) -g -gdwarf-2 -fomit-frame-pointer -Wall -fdata-sections -ffunction-sections -fverbose-asm -Wa,-ahlms=$(<:.c=.lst) $(DDEFS)

LDFLAGS = $(MCFLAGS) -mthumb -lm --specs=nano.specs -Wl,--gc-sections -nostartfiles -T$(LDSCRIPT) -Wl,-Map=$(PROJECT).map,--cref,--no-warn-mismatch $(LIBDIR)


assemblersources = $(ASRC)
sources = $(SRC)
objects = $(sources:.c=.o)
assobjects = $(assemblersources:.s=.o)


all: $(assobjects) $(objects) $(PROJECT).elf $(PROJECT).bin
	arm-none-eabi-size $(PROJECT).elf
	gtags -q

$(assobjects): %.o: %.s
	$(AS) -c $(ASFLAGS) $< -o $@

$(objects): %.o: %.c
	$(CC) -c $(CPFLAGS) -I. $(INCDIR) $< -o $@

%elf: $(assobjects) $(objects)
	$(CC) $(assobjects) $(objects) $(LDFLAGS) $(LIBS) -o $@

%hex: %elf
	$(HEX) $< $@

%bin: %elf
	$(BIN) $< $@

clean:
	rm -f $(objects)
	rm -f $(assobjects)
	rm -f $(PROJECT).elf
	rm -f $(PROJECT).map
	rm -f $(PROJECT).hex
	rm -f $(PROJECT).bin
	rm -f $(SRC:.c=.lst)
	rm -f $(ASRC:.s=.lst)

# *** EOF ***
