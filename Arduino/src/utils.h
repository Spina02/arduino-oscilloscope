#include "globals.h"
#include <stdint.h>
#include <stdbool.h>
#include "globals.h"

// parse the command received from the serial port
void process_command(char* command);

// handle the timer interrupt
int handle_timer_interrupt();

// initialize the system
void initialize_system(uint16_t freq);