AVR_INCLUDE_DIRS=-I. -I../avr_common
AVR_CXX=avr-g++
AVR_CC=avr-gcc
AVR_AS=avr-gcc
AVRDUDE=avrdude

CC_OPTS_GLOBAL=\
-O3\
-funsigned-char\
-funsigned-bitfields\
-fshort-enums\
-Wall\
$(AVR_INCLUDE_DIRS)\
-DF_CPU=16000000UL\

AVR_TARGET?=mega
AVRDUDE_PORT?=/dev/ttyUSB0

ifeq ($(AVR_TARGET), mega)
	CC_OPTS_GLOBAL += -mmcu=atmega2560 -D__AVR_3_BYTE_PC__
	AVRDUDE_FLAGS  += -p m2560
	AVRDUDE_BAUDRATE = 9600
	AVRDUDE_BOOTLOADER = wiring
endif

ifeq ($(AVR_TARGET), uno)
	CC_OPTS_GLOBAL += -mmcu=atmega328p 
	AVRDUDE_FLAGS  += -p m328p
	AVRDUDE_BAUDRATE = 115200
	AVRDUDE_BOOTLOADER = arduino
endif

CC_OPTS=$(CC_OPTS_GLOBAL) --std=gnu99 
CXX_OPTS=$(CC_OPTS_GLOBAL) --std=c++17 
AS_OPTS=-x assembler-with-cpp $(CC_OPTS)

AVRDUDE_WRITE_FLASH = -U flash:w:$(AVR_TARGET):i
AVRDUDE_FLAGS += -P $(AVRDUDE_PORT) -c $(AVRDUDE_PROGRAMMER) -b $(AVRDUDE_BAUDRATE)
AVRDUDE_FLAGS += -D -q -V -C /etc/avrdude.conf
AVRDUDE_FLAGS += -c $(AVRDUDE_BOOTLOADER)

.PHONY: build_avr
build_avr: $(AVR_BINS)

#common objects
%.o: %.c 
	$(AVR_CC) $(CC_OPTS) -c  -o $@ $<

%.o: %.s
	$(AVR_AS) $(AS_OPTS) -c  -o $@ $<

%.hex: %.elf
	avr-objcopy -O ihex -R .eeprom $< $@
	$(AVRDUDE) $(AVRDUDE_FLAGS) -U flash:w:$@:i #$(AVRDUDE_WRITE_EEPROM) 

# Rule for building main.elf
main.elf: $(AVR_OBJS)
	$(AVR_CC) $(CC_OPTS) -o $@ $(AVR_OBJS) $(LIBS)

.PHONY: clean_avr
clean_avr:	
	rm -rf $(AVR_OBJS) $(AVR_BINS) *.hex *~ *.o Arduino/main.elf
	
.SECONDARY: $(AVR_OBJS)