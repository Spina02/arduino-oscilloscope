#pragma once
#include <sys/types.h>
#include <stdbool.h>

#define DEBUG false
#define BUFFER_SIZE 1024

extern int fd;
extern pid_t pid;
extern char* device;
extern int baudrate;
extern bool blocking;

void sig_handler(int signo);

void parent_fn();

void child_fn();