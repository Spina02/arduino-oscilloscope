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
#include "src/adc.h"
#include "src/buffer.h"
#include "src/trigger.h"
#include <string.h>
#include <util/delay.h>

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
    initialize_system(freq);

    while (1) {
        // Check if there are any interrupts to handle
        if (interrupts & (1 << RXINT)) {
            process_command(usart_getstring());
        } else if (interrupts & (1 << TIMINT)) {
            int res = handle_timer_interrupt();
            if (!res) continue;
        } else {
            // Enter sleep mode if no interrupts
            if (first_iter) {
                printf("Sleeping...\n");
                first_iter = false;
            }
            set_sleep_mode(SLEEP_MODE_IDLE);
            sleep_mode();
        }
        _delay_ms(50); // delay for readability
    }
}