#include "utils.h"
#include "globals.h"
#include "adc.h"
#include "buffer.h"
#include "timer_interrupt.h"
#include "uart.h"
#include "trigger.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

void process_command(char* command) {
    cli();
    if (strcmp(command, "") == 0) {
        interrupts &= ~(1 << RXINT);
        sei();
        return;
    }
    if (strcmp(command, "exit") == 0) {
        printf("Exiting\n");
    } 
    else if (strcmp(command, "h") == 0 || strcmp(command, "help") == 0) {
    printf("clear\n");
    printf("Commands:\n");
    printf(" (s) %-10s: %s\n", "start", "start the oscilloscope");
    printf(" (s) %-10s: %s\n", "stop", "stop the oscilloscope");
    printf(" (f) %-10s: %s\n", "freq", "set the sampling frequency");
    printf(" (c) %-10s: %s\n", "channel", "set the channel to sample");
    printf(" (m) %-10s: %s\n", "mode", "set the mode of operation");
    printf(" (t) %-10s: %s\n", "trigger", "enable trigger condition");
}
    else if ((strcmp(command, "s") == 0) | (strcmp(command, "start") == 0) | (strcmp(command, "stop") == 0)) {
        running = !running;
        printf("Oscilloscope %s%s\n", running ? "started" : "stopped", (running && wait_for_trigger) ? " (waiting for trigger)" : "");
    }
    else if (strcmp(command, "f") == 0 || strcmp(command, "freq") == 0) {
        printf("Enter frequency (Hz): \n");
        //char* c_freq = usart_getstring();
        freq = atoi(usart_getstring());//c_freq);
        printf("Sampling frequency set to %d Hz\n", freq);
        timer1_init(freq);
    } 
    else if (strcmp(command, "c") == 0 || strcmp(command, "channel") == 0) {
        printf("Enter channel: \n");
        channels = atoi(usart_getstring());
        printf("Channel set to ");
        binprintf(channels);
        printf("\n");
    } 
    else if (strcmp(command, "m") == 0 || strcmp(command, "mode") == 0) {
        printf("Enter mode (c: continuous, b: buffered): \n");
        mode =  *((char*) usart_getstring());
        if ((mode == 'c') | (mode == 'b'))
            printf("Mode set to %s\n", mode == 'c' ? "'continuous'" : "'buffered'");
        else printf("unknown mode '%c', type 'c' for continuous mode, 'b' for buffered mode\n", mode);
    } 
    else if (strcmp(command, "t") == 0 || strcmp(command, "trigger") == 0) {
        wait_for_trigger = !wait_for_trigger;
        printf("%s\n", wait_for_trigger ? "Enabled trigger mode\n" : "Disabled trigger mode\n");
        first_iter = true;
    } 
    else {
        printf("Unknown command: %s\n", command);
    }
    interrupts &= ~(1 << RXINT);
    sei();
}

int handle_timer_interrupt() {
    cli();
    if (running && wait_for_trigger) {
        interrupts &= ~(1 << TIMINT);
        if (running) {
            if (first_iter) printf("Waiting for Trigger\n");
            first_iter = false;
        }
        for (int i = 0; i < CHANNELS; i++) {
            if (channels & (1 << i)) {
                last_samples[i] = curr_samples[i];
                curr_samples[i] = adc_read(i);
            }
        }
        wait_for_trigger = !is_triggered(curr_samples, last_samples, channels);
        sei();
        return 0;
    }
    if (running && (mode == 'c')) {
        for (int i = 0; i < CHANNELS; i++) {
            if (channels & (1 << i)) {
                curr_samples[i] = adc_read(i);
                // send the sample to the PC
                printf("channel %d : %d \t", i, curr_samples[i]);
            }
        }
        printf("\n");
    } else if (running && (mode == 'b')) {
        for (int i = 0; i < CHANNELS; i++) {
            if (channels & (1 << i)) {
                curr_samples[i] = adc_read(i);
            }
        }
        add_buf(curr_samples);
    }
    sei();
    return 1;
}

void initialize_system(uint16_t freq) {
    adc_init();
    printf_init();
    timer1_init(freq);
    // initialize the samples with the first read
    for (int i = 0; i < CHANNELS; i++) {
        curr_samples[i] = adc_read(i);
        last_samples[i] = curr_samples[i];
    }
    sei();

    printf("Oscilloscope ready\nType 'h' for help\n");
}