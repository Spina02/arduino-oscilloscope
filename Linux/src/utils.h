#pragma once
#include <sys/types.h>
#include <stdbool.h>
#include <semaphore.h>

#define DEBUG false
#define SEM_NAME "gnuplot_sem"

extern pid_t pid;
extern char* device;
extern bool blocking;
extern bool ready;
extern int baudrate;
extern int fd;
extern int fd_write;
extern int gnuplot_pipe[2];
extern const char *dirPath;
extern const char *dataPath;
extern sem_t* sem;
extern int total_samples;

// Signal handler
void sig_handler(int signo);

// Initialize the program
int init(int argc, char* argv[]);

// Parent process function, used to manage the serial communication
void parent_fn();

// Child process function, used to plot data
void child_fn();