CFLAGS += -I./sources/platform
CFLAGS += -I./sources/platform/spi
CFLAGS += -I./sources/platform/adc
CFLAGS += -I./sources/platform/flash
CFLAGS += -I./sources/platform/button

VPATH += sources/platform/spi
VPATH += sources/platform/adc
VPATH += sources/platform/flash
VPATH += sources/platform/button

SOURCES	+=  sources/platform/spi/spi.c
SOURCES	+=  sources/platform/adc/adc.c
SOURCES	+=  sources/platform/flash/flash.c
SOURCES	+=  sources/platform/button/button.c
