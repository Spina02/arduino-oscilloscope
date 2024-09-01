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
extern bool ready;
extern int fd_write;
extern int fd_read;

extern const char *dirPath;
extern const char *dataPath;


extern char msg[BUFFER_SIZE];

void sig_handler(int signo);

void parent_fn();

void child_fn();

void manage_msg(char* msg);

void receive_msg(int fd);

void send_msg();