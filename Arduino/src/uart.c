#include "uart.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include "globals.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

void usart_init(uint16_t ubrr);
char usart_getchar( void );
void usart_putchar( char data );
void usart_putstr (char *data);
unsigned char usart_kbhit(void);
int usart_putchar_printf(char var, FILE *stream);

static FILE mystdout = FDEV_SETUP_STREAM(usart_putchar_printf, NULL, _FDEV_SETUP_WRITE);

void usart_init( uint16_t ubrr) {
    // Set baud rate
    UBRR0H = (uint8_t) (ubrr>>8);
    UBRR0L = (uint8_t) ubrr;

    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);                /* 8-bit data */ 
    UCSR0B = _BV(RXEN0)  | _BV(TXEN0) | _BV(RXCIE0);   /* Enable RX and TX */  
}
void usart_putchar(char data) {
    // Wait for empty transmit buffer
    while ( !(UCSR0A & (_BV(UDRE0))) );
    // Start transmission
    UDR0 = data; 
}
char usart_getchar(void) {
    // Wait for incoming data
    while ( !(UCSR0A & (_BV(RXC0))) );
    // Return the data
    return UDR0;
}

char* usart_getstring(void) {
    static char buffer[64];
    char *ptr = buffer;
    char c;

    while ((c = usart_getchar()) != '\n') {
        if (c == '\r') break;
        *ptr++ = c;
    }
    *ptr = '\0';
    return buffer;
}

unsigned char usart_kbhit(void) {
    //return nonzero if char waiting polled version
    unsigned char b;
    b=0;
    if(UCSR0A & (1<<RXC0)) b=1;
    return b;
}
void usart_putstr(char *data) {
    // loop through entire string
    while (*data) { 
        usart_putchar(*data); // send the character
        data++;
    }
}
 
// this function is called by printf as a stream handler
int usart_putchar_printf(char var, FILE *stream) {
    // translate \n to \r for br@y++ terminal
    if (var == '\n') usart_putchar('\r');
    usart_putchar(var);
    return 0;
}

void printf_init(void){
  stdout = &mystdout;
  // fire up the usart
  usart_init(MYUBRR);
}

void binprintf(char num)
{
    unsigned char mask = 1 << ((sizeof(char) << 3) - 1);
    while (mask) {
        printf("%d", (num & mask ? 1 : 0));
        mask >>= 1;
    }
}