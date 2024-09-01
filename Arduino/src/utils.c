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

int process_command(char* command) {
    cli();
    if (strcmp(command, "") == 0) {
        interrupts &= ~(1 << RXINT);
        sei();
        return -1;
    }
    if (strcmp(command, "exit") == 0) {
        printf("CMD:Exiting\n");
    } 
    else if (strcmp(command, "h") == 0 || strcmp(command, "help") == 0) {
    printf("CMD:clear\n");
    printf("CMD:Commands:\n");
    printf("CMD:(s) %-10s: %s\n", "start", "start the oscilloscope");
    printf("CMD:(s) %-10s: %s\n", "stop", "stop the oscilloscope");
    printf("CMD:(f) %-10s: %s\n", "freq", "set the sampling frequency");
    printf("CMD:(c) %-10s: %s\n", "channel", "set the channel to sample");
    printf("CMD:(m) %-10s: %s\n", "mode", "set the mode of operation");
    printf("CMD:(t) %-10s: %s\n", "trigger", "enable trigger condition");
}
    else if ((strcmp(command, "s") == 0) | (strcmp(command, "start") == 0) | (strcmp(command, "stop") == 0)) {
        running = !running;
        printf("CMD:Oscilloscope %s%s\n", running ? "started" : "stopped", (running && wait_for_trigger) ? " (waiting for trigger)" : "");
    }
    else if (strcmp(command, "f") == 0 || strcmp(command, "freq") == 0) {
        printf("CMD:Enter frequency (Hz): \n");
        //char* c_freq = usart_getstring();
        freq = atoi(usart_getstring());//c_freq);
        printf("CMD:Sampling frequency set to %d Hz\n", freq);
        timer1_init(freq);
    } 
    else if (strcmp(command, "c") == 0 || strcmp(command, "channel") == 0) {
        printf("CMD:Enter channel: \n");
        channels = atoi(usart_getstring());
        printf("CMD:Channel set to ");
        binprintf(channels);
        printf("\n");
    } 
    else if (strcmp(command, "m") == 0 || strcmp(command, "mode") == 0) {
        printf("CMD:Enter mode (c: continuous, b: buffered): \n");
        mode =  *((char*) usart_getstring());
        if ((mode == 'c') | (mode == 'b'))
            printf("CMD:Mode set to %s\n", mode == 'c' ? "'continuous'" : "'buffered'");
        else printf("CMD:unknown mode '%c', type 'c' for continuous mode, 'b' for buffered mode\n", mode);
    } 
    else if (strcmp(command, "t") == 0 || strcmp(command, "trigger") == 0) {
        trigger = !trigger;
        wait_for_trigger = !wait_for_trigger;
        printf("CMD:%s\n", wait_for_trigger ? "Enabled trigger mode\n" : "Disabled trigger mode\n");
        first_iter = true;
    } 
    else {
        printf("CMD:Unknown command: %s\n", command);
        return -1;
    }
    interrupts &= ~(1 << RXINT);
    sei();
    return 0;
}

int handle_timer_interrupt() {
    cli();
    if (running && trigger && wait_for_trigger) {
        interrupts &= ~(1 << TIMINT);
        if (running) {
            if (first_iter) printf("CMD:Waiting for Trigger\n");
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
        return -1;
    }
    if (running && (mode == 'c')) {
        strcpy(data, "DATA:");
        for (int i = 0; i < CHANNELS; i++) {
            char temp[20]; // Temporary buffer for each sample string
            if (channels & (1 << i)) {
                curr_samples[i] = adc_read(i);
                snprintf(temp, sizeof(temp), " %d", curr_samples[i]);
            } else {
                snprintf(temp, sizeof(temp), " -1");
            }
            strncat(data, temp, data_size - strlen(data) - 1);
        }
        printf("%s\n", data);
        free(data);
    } else if (running && (mode == 'b')) {
        for (int i = 0; i < CHANNELS; i++) {
            if (channels & (1 << i)) {
                curr_samples[i] = adc_read(i);
            }
        }
        add_buf(curr_samples);
    }
    sei();
    return 0;
}

int initialize_system(uint16_t freq) {
    cli();
    adc_init();
    printf_init();
    timer1_init(freq);
    // initialize the samples with the first read
    for (int i = 0; i < CHANNELS; i++) {
        curr_samples[i] = adc_read(i);
        last_samples[i] = curr_samples[i];
    }
    // init data
    data = calloc(data_size, sizeof(char));
    if (data == NULL) {
        printf("CMD:Error: Unable to allocate memory\n");
        return -1;
    }
    strcpy(data, "DATA:");
    for (int i = 0; i < CHANNELS; i++) {
        char temp[20]; // Temporary buffer for each sample string
        snprintf(temp, sizeof(temp), "ch_%d ", i);
        strncat(data, temp, data_size - strlen(data) - 1);
    }
    sei();

    printf("GO\n"); // Signal the host that the oscilloscope is ready
    printf("GO\n"); // double signal to ensure the host receives it
    printf("%s\n", data);
    printf("CMD:Oscilloscope ready\n");
    printf("CMD:Type 'h' for help\n");
    return 0;
}