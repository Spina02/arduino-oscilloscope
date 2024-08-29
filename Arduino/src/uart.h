#pragma once
#include <stdint.h>

// get a string from the UART
char* usart_getstring(void);

// get a character from the UART
char usart_getchar(void);

// send a character to the UART
void usart_putchar(char data);

// send a string to the UART
void usart_putstr(char* data);

// initialize the uart printf function
void printf_init(void);

// custom binary print
void binprintf(char num);
