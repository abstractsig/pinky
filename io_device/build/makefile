#
# build pinky executable
#
APPLICATION_NAME = pinky
BINARY_NAME = $(APPLICATION_NAME).elf
APPLICATION_VERSION = "2020-02"

APPLICATION_UID = "'u','i','d','0','0','0','0','0','0','0','0','p','i','n','k','y'"

#
# file locations
#
IO_DIR = ../../io_core
CPU_DIR = ../../io_cpu
BOARD_DIR = ../../io_board
COMPONENTS_DIR = ../../io_components
BBT_DIR = ../../bbt_components
DEVICE_DIR = ..
DEVICE_SOURCE_DIR = $(DEVICE_DIR)/source
#
# TOOLS
#
TOOL_PREFIX = arm-none-eabi-
CC = $(TOOL_PREFIX)gcc
#
# flags
#
OPTIMISATION = -O0
WARNINGS = -Wall
DEFS = -DDEVICE_NAME="\"$(APPLICATION_NAME)\"" -DSTATIC_UID=$(APPLICATION_UID)
TARGET_FLAGS = -mcpu=cortex-m4 -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb 
GENERIC_FLAGS = -fomit-frame-pointer -ffunction-sections -fdata-sections -nostdlib -nostartfiles -std=c99 -g3 -Wfatal-errors
COMPILE_FLAGS = $(DEFS) \
	$(TARGET_FLAGS) \
	$(GENERIC_FLAGS) \
	$(OPTIMISATION) \
	$(WARNINGS) \
	-fno-diagnostics-color \
	-Wno-address-of-packed-member
LINK_FLAGS = $(TARGET_FLAGS) \
		--specs=nosys.specs \
		-T$(DEVICE_DIR)/linker/nrf52840.ld \
		-Wl,--gc-sections \
		-Wl,-Map=$(APPLICATION_NAME).map
#
#
#
OBJECTS = main.o test.o
DEPENDS = $(IO_DIR)/*.h \
	$(CPU_DIR)/*.h \
	$(BBT_DIR)/xnet/*.h\
	$(CPU_DIR)/cmsis/*.h \
	$(DEVICE_DIR)/include/*.h \
	$(BOARD_DIR)/*.h \
	$(COMPONENTS_DIR)/graphics/*.h \
	$(COMPONENTS_DIR)/layers/*.h \
	makefile
	
INCLUDE_DIRS = -I$(DEVICE_DIR)/include \
	-I$(IO_DIR) \
	-I$(BBT_DIR) \
	-I$(CPU_DIR) \
	-I$(CPU_DIR)/cmsis \
	-I$(BOARD_DIR) \
	-I$(COMPONENTS_DIR)

all: $(APPLICATION_NAME).elf

$(APPLICATION_NAME).elf: $(OBJECTS)  $(DEVICE_DIR)/linker/nrf52840.ld makefile
	@echo link $<
	@$(CC) $(LINK_FLAGS) -o $@ $(OBJECTS) -lm -lgcc
	
%.o: $(DEVICE_SOURCE_DIR)/%.c $(DEPENDS)
	@echo compile $<
	@$(CC) $(INCLUDE_DIRS) -o $@ -c $(COMPILE_FLAGS) $<

size:
	@$(TOOL_PREFIX)size.exe $(APPLICATION_NAME).elf
	
clean:
	rm -rf *.o *.map *.elf
	
.PHONY: all clean


