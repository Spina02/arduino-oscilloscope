#include "src/serial.h"
#include "src/plot.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/select.h>

#define DEBUG true

// global variables
int fd;
pid_t pid;
char* device = "/dev/ttyUSB0";
int baudrate = 9600;
bool blocking = true;

int main(int argc, char** argv) {
    
    if(argc < 3){
        printf("\nUsing default values :\n\tdevice  \t: %s\n\tbaudrate\t: %d\n\tblocking\t: %s\n", device, baudrate, blocking ? "true" : "false");
        printf("\nOtherwise use the format './main.o <device> <baudrate> [blocking]'\n");
        printf("                                                        ^^^^^^^^\n");
        printf("                                                        optional\n");
    } else if (argc == 3) {
        if (DEBUG) printf("using values :\n\tdevice  \t: %s\n\tbaudrate\t: %d\n", argv[1], atoi(argv[2]));
        device = argv[1];
        baudrate = atoi(argv[2]);
    } else if (argc == 4){
        if (DEBUG) printf("using values :\n\tdevice  \t: %s\n\tbaudrate\t: %d\n", argv[1], atoi(argv[2]));
        device = argv[1];
        baudrate = atoi(argv[2]);
        blocking = ((strcmp(argv[3], "true") == 0) | (atoi(argv[3]) == 1)) ? true : false;
    } else {
        printf("too many arguments\n");
        return -1;
    }

    // open the serial port
    if (DEBUG) printf("-> Opening port \t: %s\n", device);
    fd = open_port(device);

    if (fd == -1) {
        perror("Unable to open port");
        return -1;
    }

    // set the attributes
    if (DEBUG) printf("-> Setting attributes \t: [baudrate = %d, blocking = %s]\n", baudrate, blocking ? "true" : "false");
    if (set_attributes(fd, baudrate, 0, blocking) == -1) {
        perror("Unable to set attributes");
        return -1;
    }

    // create a child process
    pid = fork();

    // error
    if (pid == -1) {
        perror("Unable to fork");
        return -1;
    } 
    // child process
    else if (pid == 0) { 
        //TODO: plot the data
        
    }
    // parent process
    else {
        //TODO: manage commands
        
    }
    return 0;
}