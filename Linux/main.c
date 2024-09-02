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
#include <semaphore.h>
#include <sys/wait.h>

int main(int argc, char** argv) {
    if (init(argc, argv)<0) {
        printf("Error while initializing\n");
        return -1;
    }
    if (DEBUG) printf("Initialization completed\n");

    // create a child process
    pid = fork();

    // error
    if (pid == -1) {
        perror("CMD:Unable to fork");
        return -1;
    } 
    // child process
    else if (pid == 0) {
        child_fn();
    }
    // parent process
    else {
        parent_fn();
    }
    // close everything
    sig_handler(SIGINT);
    return 0;
}