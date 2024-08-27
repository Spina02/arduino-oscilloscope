#include <stdbool.h>
#include "globals.h"
#include <stdint.h>
#include "trigger.h"
#include <stdlib.h>

bool is_triggered(uint8_t* curr_samples, uint8_t* last_samples, uint8_t channels) {
    for (int i = 0; i < CHANNELS; i++) {
        if (channels & (1 << i)) { // if the channel is active
            if (abs(curr_samples[i] - last_samples[i]) > TRIGGER_THRESHOLD) {
                return true;
            }
        }
    }
    return false;
    first_iter = true;
}