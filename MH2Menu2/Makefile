TARGET = mh2menu
OBJS = main.o exports.o pspmem.o

CFLAGS = -O2 -G0 -Wall -Wwrite-strings -Wdiscarded-qualifiers
CXXFLAGS = $(CFLAGS) -fno-exceptions -fno-rtti
ASFLAGS = $(CFLAGS)

BUILD_PRX = 1
PRX_EXPORTS = exports.exp


USE_PSPSDK_LIBC = 1

LIBS = -lpspsystemctrl_kernel

PSPSDK = $(shell psp-config --pspsdk-path)
include $(PSPSDK)/lib/build_prx.mak