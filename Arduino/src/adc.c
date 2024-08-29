#include <avr/io.h>
#include <stdint.h>
#include "globals.h"

void adc_init() {
    // Set the ADC to use 5V as the reference voltage and left adjust the result (and use only ADCH)
    ADMUX = (1 << REFS0) | (1 << ADLAR);
    // set the prescaler to 128 (16 MHz / 128 = 125 KHz)
    ADCSRA = (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
    // Enable the ADC
    ADCSRA |= (1 << ADEN);
}

uint16_t adc_read(uint8_t channel) {
    // Select the channel
    ADMUX = (ADMUX & 0xF0) | (channel & 0x0F);
    // Start the conversion
    ADCSRA |= (1 << ADSC);
    // Wait for the conversion to complete
    while (ADCSRA & (1 << ADSC));
    // Return the result (8-bit left adjusted)
    return ADCH;
}