#include <avr/io.h>
#include <avr/interrupt.h>
#include "globals.h"

void timer1_init(uint16_t freq) {
    // Calculate the value for the Output Compare Register
    uint16_t ocr_value = (16000000 / (2 * 8 * freq)) - 1;

    // Set Timer1 to CTC mode
    TCCR1B |= (1 << WGM12);

    // Set prescaler to 8
    TCCR1B |= (1 << CS11);

    // Set the Output Compare Register
    OCR1A = ocr_value;

    // Enable Output Compare A Match Interrupt
    TIMSK1 |= (1 << OCIE1A);
}

