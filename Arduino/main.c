#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "src/globals.h"
#include "src/utils.h"
#include "src/uart.h"
#include "src/timer_interrupt.h"
#include <util/delay.h>
#include "src/adc.h"
#include "src/buffer.h"
#include "src/trigger.h"
#include <string.h>

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

// Timer interrupt
ISR(TIMER1_COMPA_vect){
  interrupts |= 1 << TIMINT;
}

// Receive interrupt
ISR(USART0_RX_vect) {
  interrupts |= 1 << RXINT;
}

int main(int argc, char** argv) {
    // Initialize ADC, UART and timer
    initialize_system(freq);

    while (1) {
        // Received message from UART
        if (interrupts & (1 << RXINT)) {
            process_command(usart_getstring());
        } 
        // Check if the timer interrupt has occurred
        else if (interrupts & (1 << TIMINT)) {
            // Check if the trigger condition is met
            int res = handle_timer_interrupt();
            if (!res) continue;
        } 
        // Sleep if no interrupts
        else {
            if (first_iter)
                printf("Sleeping...\n");
            first_iter = false;
            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_mode();
        }
        _delay_ms(50); // delay for readability
    }
}