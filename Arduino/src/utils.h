#include "globals.h"
#include <stdint.h>
#include <stdbool.h>

// initialize the system
int initialize_system(uint16_t freq);

// parse the command received from the serial port
int process_command(char command);

// handle the timer interrupt
int handle_timer_interrupt();

bool is_triggered(uint8_t* curr_samples, uint8_t* last_samples, uint8_t channels);

int timer1_init(uint16_t freq);