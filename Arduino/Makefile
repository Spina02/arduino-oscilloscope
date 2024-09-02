AVR_BINS=main.elf 

# put here the additional .o files you want to generate
# one .c file for each .o should be present

AVR_OBJS=./src/uart.o
AVR_OBJS+=./src/timer_interrupt.o
AVR_OBJS+=./src/adc.o
AVR_OBJS+=./src/trigger.o
AVR_OBJS+=./src/buffer.o
AVR_OBJS+=./src/utils.o
AVR_OBJS+=./src/globals.o
AVR_OBJS+=./main.o

# put here the additional header files needed for compilation
AVR_HEADERS=./src/uart.h
AVR_HEADERS+=./src/timer_interrupt.h
AVR_HEADERS+=./src/adc.h
AVR_HEADERS+=./src/const.h
AVR_HEADERS+=./src/trigger.h 
AVR_HEADERS+=./src/buffer.h
AVR_HEADERS+=./src/utils.h
AVR_HEADERS+=./src/globals.h


# the file below contains the actual rules
include ./src/avr.mk

.PHONY: all build clean
all: build_avr main.elf

build: build_avr main.elf
clean: clean_avr