.PHONY: all build clean linux avr run

all: clean linux avr run

build: linux avr

linux:
	$(MAKE) -C Linux build

avr:
	$(MAKE) -C Arduino build

clean:
	$(MAKE) -C Linux clean
	$(MAKE) -C Arduino clean

ARGS="$(filter-out $@,$(MAKECMDGOALS))"

run:
	$(MAKE) -C Linux run ARGS=$(ARGS)

# Prevent make from interpreting the arguments as targets
%:
    @: