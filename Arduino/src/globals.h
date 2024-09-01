#pragma once
#include <stdint.h>
#include <stdbool.h>

#define TIMINT 1 // timer interrupt
#define RXINT  2 // receive interrupt

#define BAUD 9600              // baud rate
#define MYUBRR F_CPU/16/BAUD-1 // UBRR value for 9600 baud

#define CHANNELS 8      // number of channels
#define BUFFER_SIZE 16  // buffer size

#define TRIGGER_THRESHOLD 100 // threshold for the trigger

extern uint16_t freq;         // default frequency
extern char interrupts;       // 0b00000000
extern char channels;         // shift register for the channel
extern char mode;             // default mode
extern bool wait_for_trigger; // default trigger
extern bool trigger;          // trigger flag

extern uint8_t idx;                                 // index for the buffer
extern uint8_t buffer[BUFFER_SIZE][CHANNELS]; // buffer for the samples
extern uint8_t curr_samples[CHANNELS];              // current samples
extern uint8_t last_samples[CHANNELS];              // last samples

extern bool running;         // running flag
extern bool first_iter;      // first iteration flag

extern int data_size;   // size of the data buffer
extern char* data;             // data buffer