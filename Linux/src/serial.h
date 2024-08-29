#include <termios.h>
#include <stdint.h>
#include <stdbool.h>

// set serial interface attributes
int set_attributes(int fd, int speed, int parity, bool blocking);

// open serial port
int open_port(char* portname);