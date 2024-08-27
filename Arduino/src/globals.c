#include "globals.h"

// oscilloscope
uint16_t freq = 1000;        // default frequency
char interrupts = 0;         // 0b00000000
char channels = 0b10001001;  // shift register for the channel
char mode = 'c';             // default mode
bool trigger = true;         // default trigger

// buffer
uint8_t idx = 0;                                    // index for the buffer
unsigned char buffer[BUFFER_SIZE][CHANNELS] = {0};  // buffer for the samples
uint8_t curr_samples[CHANNELS] = {0};               // current samples
uint8_t last_samples[CHANNELS] = {0};               // last samples

// genral
bool running = false;         // running flag
bool first_iter = true;