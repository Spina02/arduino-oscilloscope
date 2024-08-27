#pragma once
#include <stdint.h>

void printf_init(void);
char* usart_getstring(void);
char usart_getchar(void);
void usart_putstr(char* data);
void usart_putchar(char data);