#include "buffer.h"
#include "uart.h"
#include "globals.h"
#include <stdio.h>

void add_buf(uint8_t* samples) {
    // Add samples to the buffer
    for (int i = 0; i < CHANNELS; i++) {
        if (channels & (1 << i))
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
        printf("DATA");
        for (int j = 0; j < CHANNELS; j++) {
            if (channels & (1 << j)) {
                printf("%d ", buffer[i][j]);
            }
            else {
                printf("-1 ");
            }
        }
        printf("\n");
    }
    printf("CMD_Buffer sent\n\n");
    idx = 0;
    if (trigger)
        wait_for_trigger = true;
}