#pragma once
#include <semaphore.h>

#define MSG_SIZE 1024

// manage_msg processes the received message
void manage_msg(char* new_msg);
// receive_msg reads data from the serial port and processes it
void receive_msg(int fd);
// redirect_msg reads input from the user and sends it to the device
void redirect_msg();