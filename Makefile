##############################################################################
# @Author: ThanNT
# @Date:   20/12/2016
##############################################################################
#direction define
NAME_MODULE = lora_water_sensor_node
PROJECT = $(NAME_MODULE)
Print = @echo "~"

MCU_OPTION	=	msp430g2553
PLATFORM_OPTION	=
#RELEASE_OPTION	= -DRELEASE
OPTIMIZE_OPTION = -O2

OBJECTS_DIR = build_$(NAME_MODULE)_msp430g
TARGET = $(OBJECTS_DIR)/$(NAME_MODULE).elf

GCC_PATH = /home/ngb/workpage/FPT_iot/software/software_install/MSP430_gcc

-include sources/app/Makefile.mk
-include sources/platform/Makefile.mk

SOURCES_	 = $(shell find $(SOURCES) -type f -printf "%f\n")
OBJECTS		+= $(patsubst %.c, $(OBJECTS_DIR)/%.o, $(SOURCES_))

# The command for calling the compiler.
CC		=	$(GCC_PATH)/bin/msp430-elf-gcc
CPP		=	$(GCC_PATH)/bin/msp430-elf-g++
AR		=	$(GCC_PATH)/bin/msp430-elf-ar
LD		=	$(GCC_PATH)/bin/msp430-elf-ld
OBJCOPY		=	$(GCC_PATH)/bin/msp430-elf-objcopy
MSP430_GDB	=	$(GCC_PATH)/bin/msp430-elf-gdb
MSP430_SIZE	=	$(GCC_PATH)/bin/msp430-elf-size

# ASM compiler flags
AFLAGS +=

# C compiler flags
CFLAGS +=	-g -ffunction-sections -fdata-sections -DDEPRECATED	\
		$(OPTIMIZE_OPTION)	\
		-D__$(MCU_OPTION)__	\
		-mmcu=$(MCU_OPTION)	\
		$(RELEASE_OPTION)

# linker flags
LDFLAGS	+=	-L sources/platform -T sources/platform/$(MCU_OPTION).ld -mmcu=$(MCU_OPTION)

all: create $(TARGET)

create:
	$(Print) CREATE $(OBJECTS_DIR) folder
	@mkdir -p $(OBJECTS_DIR)

$(TARGET): $(OBJECTS)
	$(Print) LD $@
	@$(CC) $(LDFLAGS) -o $(@) $(^)
	@$(MSP430_SIZE) $(@)

$(OBJECTS_DIR)/%.o: %.c
	$(Print) CC $@
	@$(CC) $(CFLAGS) -c $< -o $@

flash: 	all
	$(Print) BURNING $(TARGET) to target
	@mspdebug rf2500 'erase' 'load $(TARGET)' 'exit'

clean:
	$(Print) CLEAN $(OBJECTS_DIR) folder
	@rm -rf $(OBJECTS_DIR)
