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
        printf("CMD_Exiting\n");
    } 
    else if (strcmp(command, "h") == 0 || strcmp(command, "help") == 0) {
    printf("CMD_clear\n");
    printf("CMD_Commands:\n");
    printf("CMD_(s) %-10s: %s\n", "start", "start the oscilloscope");
    printf("CMD_(s) %-10s: %s\n", "stop", "stop the oscilloscope");
    printf("CMD_(f) %-10s: %s\n", "freq", "set the sampling frequency");
    printf("CMD_(c) %-10s: %s\n", "channel", "set the channel to sample");
    printf("CMD_(m) %-10s: %s\n", "mode", "set the operation mode (continuous/buffered)");
    printf("CMD_(t) %-10s: %s\n", "trigger", "enable trigger condition");
}
    else if ((strcmp(command, "s") == 0) | (strcmp(command, "start") == 0) | (strcmp(command, "stop") == 0)) {
        running = !running;
        printf("CMD_Oscilloscope %s%s\n", running ? "started" : "stopped", (running && wait_for_trigger) ? " (waiting for trigger)" : "");
    }
    else if (strcmp(command, "f") == 0 || strcmp(command, "freq") == 0) {
        printf("CMD_Enter frequency (Hz): \n");
        //char* c_freq = usart_getstring();
        freq = atoi(usart_getstring());//c_freq);
        printf("CMD_Sampling frequency set to %d Hz\n", freq);
        timer1_init(freq);
    } 
    else if (strcmp(command, "c") == 0 || strcmp(command, "channel") == 0) {
        printf("CMD_Enter channel: (e.g. 00000011 for channels 0 and 1)\n");

        char* ch = usart_getstring();
        // convert the string (e.g. 10110011) to a bit mask (e.g. 0b10110011)
        channels = 0;
        for (int i = 0; i < strlen(ch); i++) {
            if (ch[i] == '1') {
                channels |= 1 << (strlen(ch) - i - 1);
            }
        }

        printf("CMD_Channel set to ");
        binprintf(channels);
        printf("\n");
    } 
    else if (strcmp(command, "m") == 0 || strcmp(command, "mode") == 0) {
        printf("CMD_Enter mode (c: continuous, b: buffered): \n");
        mode =  *((char*) usart_getstring());
        if ((mode == 'c') | (mode == 'b'))
            printf("CMD_Mode set to %s\n", mode == 'c' ? "'continuous'" : "'buffered'");
        else printf("CMD_unknown mode '%c', type 'c' for continuous mode, 'b' for buffered mode\n", mode);
        if (mode == 'c') trigger = false;
    } 
    else if (strcmp(command, "t") == 0 || strcmp(command, "trigger") == 0) {
        trigger = !trigger;
        wait_for_trigger = !wait_for_trigger;
        printf("CMD_%s\n", wait_for_trigger ? "Enabled trigger mode\n" : "Disabled trigger mode\n");
        first_iter = true;
    } 
    else {
        printf("CMD_Unknown command: %s\n", command);
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
            if (first_iter) printf("CMD_Waiting for Trigger\n");
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
        strcpy(data, "DATA");
        for (int i = 0; i < CHANNELS; i++) {
            char temp[20]; // Temporary buffer for each sample string
            if (channels & (1 << i)) {
                curr_samples[i] = adc_read(i);
                snprintf(temp, sizeof(temp), "%d ", curr_samples[i]);
            } else {
                snprintf(temp, sizeof(temp), "-1 ");
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
        printf("CMD_Error: Unable to allocate memory\n");
        return -1;
    }
    strcpy(data, "DATA");
    for (int i = 0; i < CHANNELS; i++) {
        char temp[20]; // Temporary buffer for each sample string
        snprintf(temp, sizeof(temp), "ch_%d ", i);
        strncat(data, temp, data_size - strlen(data) - 1);
    }
    sei();

    printf("DONE\n"); // Signal the host that the oscilloscope is ready
    printf("DONE\n"); // Signal the host that the oscilloscope is ready
    printf("CMD_Oscilloscope ready\n");
    printf("CMD_Type 'h' for help\n");
    return 0;
}