#include <stdint.h>

void adc_init();

uint16_t adc_read(uint8_t channel);

void adc_close();