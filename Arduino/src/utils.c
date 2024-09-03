#include "utils.h"
#include "globals.h"
#include "adc.h"
#include "buffer.h"
#include "uart.h"
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>

bool is_triggered(uint8_t* curr_samples, uint8_t* last_samples, uint8_t channels) {
    for (int i = 0; i < CHANNELS; i++) {
        if (channels & (1 << i)) { // if the channel is active
            uint8_t diff = abs(curr_samples[i] - last_samples[i]);
            last_samples[i] = curr_samples[i];
            curr_samples[i] = adc_read(i);
            diff = abs(curr_samples[i] - last_samples[i]);
            if (running && diff > TRIGGER_THRESHOLD) {
                printf("CMD_Triggered on channel %d\n", i);
                return true;
            }
        }
    }
    return false;
    first_iter = true;
}

int timer1_init(uint16_t freq) {
    cli();
    // set prescaler to 8
    int prescaler = 1024;
    // Calculate the value for the Output Compare Register
    uint16_t ocr_value = (F_CPU/prescaler/freq)-1;
    if (ocr_value < 1) {
        printf("CMD_Error: Frequency too high\n");
        return -1;
    } else if (ocr_value > UINT16_MAX) {
        printf("CMD_Error: Frequency too low\n");
        return -1;
    }

    // Reset Timer1 Control Register A and B
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;

    // Set Timer1 to CTC mode
    TCCR1B |= (1 << WGM12);
    // Set prescaler to 1024
    TCCR1B |= (1 << CS12) | (1 << CS10);
    // Set the Output Compare Register
    OCR1A = ocr_value;
    // Enable Output Compare A Match Interrupt
    TIMSK1 |= (1 << OCIE1A);

    sei();
    return OCR1A;
}

int initialize_system(uint16_t freq) {
    adc_init();
    printf_init();
    if (timer1_init(freq)<0) 
        return -1;
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
    sei();

    //printf("DONE\n"); // Signal the host that the oscilloscope is ready
    printf("DONE\n"); // Signal the host that the oscilloscope is ready
    printf("CMD_Oscilloscope ready\n");
    printf("CMD_Type 'h' for help\n");
    return 0;
}

int process_command(char command) {
    // consume newline
    usart_getchar();
    if (command == '\n') {
        interrupts &= ~(1 << RXINT);
        sei();
        return -1;
    }
    if (command == 'q') {
        printf("CMD_Exiting\n");
        // close everything
        adc_close();
        free(data);
        exit(0);
    } 
    else if (command == 'h') {
        printf("CMD_clear\n");
        printf("CMD_Commands:\n");
        printf("CMD_(s) %-10s: %s\n", "start", "start the oscilloscope");
        printf("CMD_(s) %-10s: %s\n", "stop", "stop the oscilloscope");
        printf("CMD_(f) %-10s: %s\n", "freq", "set the sampling frequency");
        printf("CMD_(c) %-10s: %s\n", "channel", "set the channel to sample");
        printf("CMD_(m) %-10s: %s\n", "mode", "set the operation mode (continuous/buffered)");
        printf("CMD_(t) %-10s: %s\n", "trigger", "enable trigger condition");
    }
    else if (command == 's') {
        running = !running;
        printf("CMD_Oscilloscope %s%s\n", (running ? "started" : "stopped"), (running && wait_for_trigger) ? " (waiting for trigger)" : "");
    }
    else if (command == 'f') {
        printf("CMD_Enter frequency (Hz): \n");
        freq = atoi(usart_getstring());
        if (timer1_init(freq) > 0)
            printf("CMD_Sampling frequency set to %d Hz\n", freq);
        else
            printf("CMD_Error setting frequency\n");  
    } 
    else if (command == 'c') {
        printf("CMD_Enter channel: (e.g. 00000011 for channels 0 and 1)\n");

        char* ch = usart_getstring();
        // convert the string (e.g. 10110011) to a bit mask (e.g. 0b10110011)
        channels = 0;
        for (int i = 0; i < strlen(ch); i++) {
            if (ch[i] == '1') {
                channels |= 1 << (strlen(ch) - i - 1);
            }
        }

        printf("CMD_Channel set to %s \n", ch);
    } 
    else if (command == 'm') {
        printf("CMD_Enter mode (c: continuous, b: buffered): \n");
        mode =  *((char*) usart_getstring());
        if ((mode == 'c') | (mode == 'b'))
            printf("CMD_Mode set to %s\n", mode == 'c' ? "'continuous'" : "'buffered'");
        else printf("CMD_unknown mode '%c', type 'c' for continuous mode, 'b' for buffered mode\n", mode);
        if (mode == 'c') trigger = false;
    } 
    else if (command == 't') {
        if (running) {
            if (trigger) {
                wait_for_trigger = !wait_for_trigger;
            } 
            else {
                trigger = true;
                wait_for_trigger = trigger;
                printf("CMD_Trigger mode Enabled\n");
            }
            if (wait_for_trigger) {
                printf("CMD_Waiting for trigger\n");
            }
        }
        else {
            trigger = !trigger;
            wait_for_trigger = trigger;
            printf("CMD_Trigger mode %s\n", wait_for_trigger ? "Enabled" : "Disabled");
        }
        first_iter = true;
    } 
    else {
        printf("CMD_Unknown command: %c\n", command);
    }
    interrupts &= ~(1 << RXINT);
    return 0;
}

int handle_timer_interrupt() {
    if (running) {
        if (wait_for_trigger) {
            wait_for_trigger = !is_triggered(curr_samples, last_samples, channels);
        }
        if (!wait_for_trigger) {
            if (mode == 'c') {
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
            } else {
                for (int i = 0; i < CHANNELS; i++) {
                    if (channels & (1 << i)) {
                        curr_samples[i] = adc_read(i);
                    }
                }
                add_buf(curr_samples);
            }
        }
    }
    interrupts &= ~(1 << TIMINT);
    return 0;
}