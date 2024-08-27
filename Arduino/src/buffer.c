#include "buffer.h"
#include "uart.h"
#include "globals.h"

void add_buf(uint8_t* samples) {
    // Add samples to the buffer
    for (int i = 0; i < CHANNELS; i++) {
        buffer[idx][i] = samples[i];
    }
    idx++;
    if (idx >= BUFFER_SIZE) {
        // Sends samples via UART if buffer is full
        send_buf();
    }
}

void send_buf() {
    // Send remaining samples via UART
    for (int i = 0; i < idx; i++) {
        for (int j = 0; j < CHANNELS; j++) {
            usart_putchar(buffer[i][j]);
        }
    }
    idx = 0;
}