#pragma once
#include <stdint.h>
#include <stdbool.h>

bool is_triggered(uint8_t* curr_samples, uint8_t* last_samples, uint8_t channels);