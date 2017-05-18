all : vive_robot

OPTS=-g -O3 -flto 
CC:=gcc


INCLUDES:=-I../include -Ilibsurvive/include/libsurvive -Ilibsurvive -Ilibsurvive/include -Ilibsurvive/redist 
DEFINES:=-DUSE_DOUBLE

CFLAGS:=$(INCLUDES) $(OPTS) -std=gnu99 $(DEFINES) 
LDFLAGS:=-L/usr/local/lib -lpthread -lusb-1.0 -lz -lm -flto -g

#----------
# Platform specific changes to CFLAGS/LDFLAGS
#----------
UNAME=$(shell uname)

# Mac OSX
ifeq ($(UNAME), Darwin)
DRAWFUNCTIONS=redist/CNFGFunctions.c redist/CNFGNullDriver.c
GRAPHICS_LOFI:=redist/CNFGFunctions.o redist/CNFGNullDriver.o

# Linux / FreeBSD
else
LDFLAGS:=$(LDFLAGS) -lX11
DRAWFUNCTIONS=redist/CNFGFunctions.c redist/CNFGXDriver.c
GRAPHICS_LOFI:=redist/CNFGFunctions.o redist/CNFGXDriver.o

endif


POSERS:=src/poser_dummy.o src/poser_daveortho.o src/poser_charlesslow.o
REDISTS:=redist/json_helpers.o redist/linmath.o redist/jsmn.o redist/os_generic.o
LIBSURVIVE_CORE:=src/survive.o src/survive_usb.o src/survive_data.o src/survive_process.o src/ootx_decoder.o src/survive_driverman.o src/survive_vive.o src/survive_config.o src/survive_cal.o
LIBSURVIVE_CORE:=$(LIBSURVIVE_CORE)
LIBSURVIVE_O:=$(POSERS) $(REDISTS) $(LIBSURVIVE_CORE)
LIBSURVIVE_C:=$(LIBSURVIVE_O:.o=.c)


#If you want to use HIDAPI on Linux.
#CFLAGS:=$(CFLAGS) -DHIDAPI
#REDISTS:=$(REDISTS) redist/hid-linux.o
#LDFLAGS:=$(LDFLAGS) -ludev

#Useful Preprocessor Directives:
# -DUSE_DOUBLE = use double instead of float for most operations.
# -DNOZLIB = use puff.c
# -DTCC = various things needed for TCC.
# -DWINDOWS -DWIN32 = Building for Windows
# -DHIDAPI = Build vive driver to use USBHID instead of interrupt/control messages.
# -DRUNTIME_SYMNUM = Don't assume __attribute__((constructor)) works.  Instead comb for anything starting with REGISTER.



# unused: redist/crc32.c

lib: ./libsurvive/src/*.c ./libsurvive/redist/*.c
	gcc $(CFLAGS) -c $^ $(LDFLAGS) 

vive_robot :  main.cpp lib
	g++ -Wall -std=c++11 -Iinclude $(INCLUDES) -o $@ main.cpp *.o $(LDFLAGS) $(DEFINES) $(OPTS)

clean :
	rm -rf *.o vive_robot




