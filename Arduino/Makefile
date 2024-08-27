BINS=main.elf 

# put here the additional .o files you want to generate
# one .c file for each .o should be present

OBJS=./src/uart.o
OBJS+=./src/timer_interrupt.o
OBJS+=./src/adc.o
OBJS+=./src/trigger.o
OBJS+=./src/buffer.o
# OBJS+=./src/utils.o
OBJS+=./src/globals.o

# put here the additional header files needed for compilation
HEADERS=./src/uart.h
HEADERS+=./src/timer_interrupt.h
HEADERS+=./src/adc.h
HEADERS+=./src/const.h
HEADERS+=./src/trigger.h 
HEADERS+=./src/buffer.h
# HEADERS+=./src/utils.h
HEADERS+=./src/globals.h


# the file below contains the actual rules

# !!!!!!!!!!!!!!!! MODIFY HERE !!!!!!!!!!!!!!!! 

include ./src/avr.mk