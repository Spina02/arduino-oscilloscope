#include "globals.h"
#include <stdint.h>
#include <stdbool.h>
#include "globals.h"

// parse the command received from the serial port
int process_command(char* command);

// handle the timer interrupt
int handle_timer_interrupt();

// initialize the system
int initialize_system(uint16_t freq);