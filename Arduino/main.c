#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "src/adc.h"
#include "src/const.h"
#include "src/timer_interrupt.h"
#include "src/uart.h"
#include <string.h>
#include <util/delay.h>

/*
    Possibility of configuring (from PC)
    - which channel(s) are being sampled
    - the sampling frequency

    Possibility of operating in two modes
    - continuous sampling: (each sample is sent
        to the PC as it becomes ready)
    - buffered mode: a set of samples are stored 
        locally and then sent in bulk

    Implementation of "trigger"
    (a condition that decides when to start sampling, in buffered mode)

    Serial communication should be asynchronous (use an interrupt
    on the UART)
*/

// command parsing logic

bool running = 0;
uint16_t freq = 1000;
uint8_t channel = 0;
char mode = 'c'; // continuous mode
bool trigger = false;
char interrupts = 0; // 0b00000000

ISR(TIMER1_COMPA_vect){
  interrupts |= 1 << TIMINT;
}

ISR(USART0_RX_vect) {
  interrupts |= 1 << RXINT;
}

//! not working
void process_command(char* command) {
    if (strcmp(command, "h") == 0 || strcmp(command, "help") == 0) {
        printf("Commands:\n");
        printf("start (s): start sampling\n");
        printf("stop (s): stop sampling\n");
        printf("freq (f): set the sampling frequency\n");
        printf("channel (c): set the channel to sample\n");
        printf("mode (m): set the mode of operation\n");
        printf("trigger (t): set the trigger condition\n");
    } else if (strcmp(command, "s") == 0) {
        running = !running;
        printf("Sampling %s\n", running ? "started" : "stopped");
    } else if (strcmp(command, "start") == 0) {
        running = 1;
        printf("Sampling started\n");
    } else if (strcmp(command, "stop") == 0) {
        running = 0;
        printf("Sampling stopped\n");
    } else if (strcmp(command, "f") == 0 || strcmp(command, "freq") == 0) {
        printf("Enter frequency (Hz): \n");
        char* c_freq = usart_getstring();
        freq = atoi(c_freq);
        printf("Sampling frequency set to %d Hz\n", freq);
        timer1_init(freq);
    } else if (strcmp(command, "c") == 0 || strcmp(command, "channel") == 0) {
        printf("Enter channel: \n");
        channel = atoi(usart_getstring());
        printf("Channel set to %d\n", channel);
    } else if (strcmp(command, "m") == 0 || strcmp(command, "mode") == 0) {
        printf("Enter mode (c: continuous, b: buffered): \n");
        mode =  *((char*) usart_getstring());
        printf("Mode set to %c\n", mode);
    } else if (strcmp(command, "t") == 0 || strcmp(command, "trigger") == 0) {
        printf("Set trigger: \n");
        trigger = atoi(usart_getstring());
        printf("Trigger set to %d\n", trigger);
    } else {
        printf("Unknown command: %s\n", command);
    }
}

int main(int argc, char** argv) {
    // Initialize the ADC
    adc_init();
    // Initialize the UART
    printf_init();
    // Initialize Timer1 with a certain sampling frequency
    timer1_init(1000); // 1 KHz

    printf("Oscilloscope ready\n");
    printf("Type 'h' for help\n");

    // Enable global interrupts
    sei();

    uint32_t i = 1;
    while (i) {
        // Check if the timer interrupt has occurred
        if (interrupts & (1 << TIMINT)) {
            // process command
            process_command(usart_getstring());

            // if running, sample the ADC
            if (running) {
                if (mode == 'c') {
                    // Continuous mode
                    uint16_t adc_value = adc_read(channel);
                    printf("Channel %d: %d\n", channel, adc_value);
                } else if (mode == 'b') {
                    // Buffered mode
                    if (trigger) {
                        // Triggered sampling
                        printf("Triggered sampling\n");
                        // TODO: implement triggered sampling
                    }
                }
            }
        } else { // Sleep mode
            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_mode();
        }
    }
}