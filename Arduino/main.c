#include "adc.h"
#include "timer_interrupt.h"
#include "uart.h"

int main(int argc, char** argv) {
    // Initialize the ADC
    adc_init();
    // Initialize the UART
    printf_init();
    // Initialize Timer1 with a certain sampling frequency
    timer1_init(1000); // 1 KHz

    // Enable global interrupts
    sei();

    while (1) {
        // TODO: handle mode switching, trigger checking etc.
    }
}