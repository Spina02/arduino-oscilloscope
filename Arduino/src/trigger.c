#include <stdbool.h>
#include "globals.h"
#include <stdint.h>
#include "trigger.h"
#include <stdlib.h>
#include <stdio.h>

bool is_triggered(uint8_t* curr_samples, uint8_t* last_samples, uint8_t channels) {
    for (int i = 0; i < CHANNELS; i++) {
        if (channels & (1 << i)) { // if the channel is active
            uint8_t diff = abs(curr_samples[i] - last_samples[i]);
            if (running && diff > TRIGGER_THRESHOLD) {
                printf("Triggered on channel %d\n", i);
                return true;
            }
        }
    }
    return false;
    first_iter = true;
}