#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include "utils.h"
#include "plot.h"
#include "comm.h"
#include "serial.h"

const char *dirPath = "./";
const char *dataPath = "data.txt";
int pipefd[2];

// global variables
int fd;
int fd_write;
int fd_read;
pid_t pid;
char* device = "/dev/ttyUSB0";
int baudrate = 9600;
bool blocking = true;
bool ready = false;
int total_samples = 0;
int gnuplot_pipe[2];
FILE* gnuplot_fp;

void sig_handler(int signo) {
    if (signo == SIGINT) {
        printf("Exiting...\n");
        // close the serial port
        close(fd_write);
        // signal the avr to stop sending data
        char c = 'q';
        write(fd, &c, 1);
        // close the serial port
        close(fd);
        // close the pipe
        close(gnuplot_pipe[1]);
        // close the gnuplot file
        if (gnuplot_fp) fclose(gnuplot_fp);
        // remove the data file
        //remove(dataPath);
        exit(0);
    }
}

int init(int argc, char* argv[]) {
    system("clear");

    // Create a pipe to communicate with gnuplot
    if (pipe(gnuplot_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    struct stat st = {0};
    if (stat(dirPath, &st) == -1) {
        if (mkdir(dirPath, 0700) != 0) {
            perror("Errore nella creazione della directory");
            exit(EXIT_FAILURE);
        }
    }

    // open the file for writing
    fd_write = open(dataPath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (fd_write == -1) {
        perror("Error while opening the file for writing");
        exit(EXIT_FAILURE);
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
            exit(EXIT_FAILURE);
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
        exit(EXIT_FAILURE);
    }

    // set the attributes
    if (DEBUG) printf(":-> Setting attributes \t: [baudrate = %d, blocking = %s]\n", baudrate, blocking ? "true" : "false");
    if (set_attributes(fd, baudrate, 0, blocking) == -1) {
        perror("Unable to set attributes");
        exit(EXIT_FAILURE);
    }
    return 0;
}

void parent_fn() {
    fd_set readfds;
    close(gnuplot_pipe[0]); // Close read end
    gnuplot_fp = gnuplot_init(dataPath, "plot.gp", 0, 50);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(fd, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int max_fd = fd > STDIN_FILENO ? fd : STDIN_FILENO;

        int activity = select(max_fd + 1, &readfds, NULL, NULL, NULL);

        if (activity < 0 && errno != EINTR) {
            perror("select error");
        }

        // Check if there is data to read from the serial port
        if (FD_ISSET(fd, &readfds))
            receive_msg(fd);

        // Check if there is input from the user
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            redirect_msg();
        }
    }
}

void child_fn() {
    // execute gnuplot
    close(gnuplot_pipe[1]); // Close write end
    dup2(gnuplot_pipe[0], STDIN_FILENO); // Redirect stdin to read end of the pipe
    close(gnuplot_pipe[0]); // Close original read end after duplication
    execlp("gnuplot", "gnuplot", NULL);
    perror("Error while executing gnuplot");
    exit(EXIT_FAILURE);
}