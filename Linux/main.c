#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include "src/plot.h"
#include "src/serial.h"
#include "src/utils.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/select.h>
#include <unistd.h>
#include <ctype.h>

// global variables
int fd;
int fd_write;
int fd_read;
pid_t pid;
char* device = "/dev/ttyUSB0";
int baudrate = 9600;
bool blocking = true;
bool ready = false;

int main(int argc, char** argv) {
    // Verifica se la directory esiste, altrimenti creala
    struct stat st = {0};
    if (stat(dirPath, &st) == -1) {
        // La directory non esiste, la creiamo
        if (mkdir(dirPath, 0700) != 0) {
            perror("Errore nella creazione della directory");
            return 1;
        }
    }

    // Aprire (o creare) il file per la scrittura, azzerando il contenuto se già esiste
    fd_write = open(dataPath, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_write == -1) {
        perror("Errore nell'apertura del file per la scrittura");
        return 1;
    }

    // Aprire lo stesso file per la lettura
    fd_read = open(dataPath, O_RDONLY);
    if (fd_read == -1) {
        perror("Errore nell'apertura del file per la lettura");
        close(fd_write); // Chiudiamo il file descriptor aperto precedentemente
        return 1;
    }
    // set the signal handler
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_handler = sig_handler;
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);
    
    // check the arguments
    if (argc == 1) {
        printf("\nUsing default values :\n\tdevice  \t: %s\n\tbaudrate\t: %d\n\tblocking\t: %s\n", device, baudrate, blocking ? "true" : "false");
        printf("\nOtherwise use the format './main.o <device> <baudrate> [blocking]'\n");
        printf("                                                        ^^^^^^^^\n");
        printf("                                                        optional\n");
    }
    else { 
        if (argc == 2) {
            device = argv[1];
        }
        else if (argc == 3) {
            device = argv[1];
            baudrate = atoi(argv[2]);
        }
        else if (argc == 4){
            device = argv[1];
            baudrate = atoi(argv[2]);
            blocking = ((strcmp(argv[3], "true") == 0) | (atoi(argv[3]) == 1)) ? true : false;
        } else {
            printf("too many arguments\n");
            return -1;
        }
        printf("Using values :\n\tdevice  \t: %s\n\tbaudrate\t: %d\n\tblocking\t: %s\n", device, baudrate, blocking ? "true" : "false");
    }
    printf("Do you want to start the oscilloscope with this attributes? [Y/n]\n");
    char c = getchar();
    //check condition
    if (c != '\n' && c != 'y' && c != 'Y') {
        //simulate sigint
        sig_handler(SIGINT);
    } 

    // open the serial port
    if (DEBUG) printf("-> Opening port \t: %s\n", device);
    fd = open_port(device);

    if (fd == -1) {
        perror("Unable to open port");
        return -1;
    }

    // set the attributes
    if (DEBUG) printf(":-> Setting attributes \t: [baudrate = %d, blocking = %s]\n", baudrate, blocking ? "true" : "false");
    if (set_attributes(fd, baudrate, 0, blocking) == -1) {
        perror("Unable to set attributes");
        return -1;
    }

    // create a child process
    pid = fork();

    // error
    if (pid == -1) {
        perror("CMD:Unable to fork");
        return -1;
    } 
    // child process
    else if (pid == 0) { 
        //TODO: plot the data
        child_fn();
    }
    // parent process
    else {
        parent_fn();
    }
    // close everything
    close(fd);
    return 0;
}