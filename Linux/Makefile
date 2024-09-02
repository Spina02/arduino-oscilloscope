LINUX_BINS=main
# put here the additional .o files you want to generate
# one .c file for each .o should be present
LINUX_OBJS=./src/serial.o
LINUX_OBJS+=./src/plot.o
LINUX_OBJS+=./src/utils.o
LINUX_OBJS+=./src/comm.o
LINUX_OBJS+=./main.o

# put here the additional header files needed for compilation
LINUX_HEADERS=./src/serial.h
LINUX_HEADERS+=./src/plot.h
LINUX_HEADERS+=./src/utils.h
LINUX_HEADERS+=./src/comm.h

# the file below contains the actual rules
include ./src/linux.mk

.PHONY: clean
clean: clean_linux

.PHONY: build
build: build_linux