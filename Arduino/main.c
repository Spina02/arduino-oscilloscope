#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/globals.h"
#include "src/utils.h"
#include "src/uart.h"
#include "src/adc.h"
#include "src/buffer.h"
#include <string.h>
#include <util/delay.h>

// oscilloscope
uint16_t freq = 100;           // default frequency
char interrupts = 0;            // 0b00000000
char channels = 0b11111111;     // shift register for channels (default channel 0)
char mode = 'c';                // default mode
bool wait_for_trigger = false;  // default trigger
bool trigger = false;           // trigger flag

// buffer
uint8_t idx = 0;                                    // index for the buffer
unsigned char buffer[BUFFER_SIZE][CHANNELS] = {0};  // buffer for the samples
uint8_t curr_samples[CHANNELS] = {0};               // current samples
uint8_t last_samples[CHANNELS] = {0};               // last samples

// genral
bool running = false;         // running flag
bool first_iter = true;

int data_size = CHANNELS*(6*sizeof(char)+1)+6; // 6 chars for "DATA:" and 1 for null terminator
char* data = 0;             // data buffer

// Timer interrupt
ISR(TIMER1_COMPA_vect) {
    interrupts |= 1 << TIMINT;
}

// Receive interrupt
ISR(USART0_RX_vect) {
    interrupts |= 1 << RXINT;
}

int main(int argc, char** argv) {
    // Initialize ADC, UART and timer
    if (initialize_system(freq) < 0) {
        printf("CMD_Error initializing system\n");
        return -1;
    };

    while (1) {
        // Check if there are any interrupts to handle
        if (interrupts & (1 << RXINT)) {
            if (process_command(usart_getchar())<0) 
                return 0;
        } else if (interrupts & (1 << TIMINT)) {
            if (handle_timer_interrupt()<0)
                return 0;
        } else {
            // Enter sleep mode if no interrupts
            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_mode();
        }
    }
}