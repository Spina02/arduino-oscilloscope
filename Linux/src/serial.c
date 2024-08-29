// imports
#include "serial.h"
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>

// baudrates
int int_speeds[] = {115200, 57600, 38400, 19200, 9600, 4800, 2400};
speed_t speeds[] = {B115200, B57600, B38400, B19200, B9600, B4800, B2400};


// open serial port
int open_port(char* portname) {
    int fd = open(portname, O_RDWR | O_NOCTTY | O_SYNC);
    if (fd < 0) {
        perror("Error opening port");
        return -1;
    }
    return fd;
}

// serial set interface attributes
int set_attributes(int fd, int speed, int parity, bool blocking) {
    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("Error from tcgetattr");
        return -1;
    }

    // check if speed is valid and convet to speed_t
    int i;
    for (i = 0; i < 7; i++) {
        if (speed == int_speeds[i]) {
            speed = speeds[i];
            break;
        }
    }
    // if not valid, return -1
    if (i == 7) {
        perror("Invalid speed\n");
        return -1;
    }

    cfsetospeed(&tty, speed); // set output speed
    cfsetispeed(&tty, speed); // set input speed
    cfmakeraw(&tty);          // set raw mode : no echo, canonical mode, etc.

    // set attributes
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;   // 8-bit chars 
    tty.c_cflag |= parity;                        // enable parity
    tty.c_cflag &= ~(PARENB | PARODD);            // no odd parity

    // set blocking mode
    tty.c_cc[VMIN] = blocking ? 1 : 0;
    tty.c_cc[VTIME] = 5; // 0.5 seconds read timeout

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Error from tcsetattr while setting attributes");
        return -1;
    }
    return 0;
}