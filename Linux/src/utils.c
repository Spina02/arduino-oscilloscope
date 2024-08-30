#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/select.h>
#include <errno.h>
#include "utils.h"

void sig_handler(int signo) {
    if (signo == SIGINT) {
        printf("Received SIGINT\n");
        close(fd);
        exit(0);
    }
}

void parent_fn() {
    // clear the terminal
    system("clear");
    fd_set readfds;
    char msg[BUFFER_SIZE];
    char cmd[256];
    int received = 0;

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
        if (FD_ISSET(fd, &readfds)) {
            int n = read(fd, msg + received, sizeof(msg) - received - 1);
            if (n > 0) {
                received += n;
                msg[received] = '\0'; // Ensure null-terminated string

                // Process each line in the received message
                char *line_start = msg;
                char *newline_pos = strchr(line_start, '\n');
                while (newline_pos != NULL) {
                    *newline_pos = '\0'; // Null-terminate the current line
                    if (strcmp(line_start, "cls") == 0) {
                        system("clear");
                    } else {
                        printf("%s\n", line_start);
                    }
                    // Move to the next line
                    line_start = newline_pos + 1;
                    newline_pos = strchr(line_start, '\n');
                }

                // Move the remaining part of the message to the beginning of the buffer
                received = strlen(line_start);
                memmove(msg, line_start, received);
                msg[received] = '\0'; // Ensure null-terminated string
            }
        }

        // Check if there is input from the user
        if (FD_ISSET(STDIN_FILENO, &readfds)) {
            if (fgets(cmd, sizeof(cmd), stdin) != NULL) {
                if (strcmp(cmd, "exit\n") == 0) { // Adjusted to include newline
                    sig_handler(SIGINT);
                } else {
                    // Send the command to the device
                    // manage partially sent commands
                    int sent  = 0;
                    while (sent < strlen(cmd)) {
                        int n = write(fd, cmd + sent, strlen(cmd) - sent);
                        fflush(stdout);
                        if (n < 0) {
                            perror("write error");
                        } else {
                            sent += n;
                        }

                        if (DEBUG) printf("Command sent: %s", cmd); // Adjusted to include newline
                    }
                }
            }
        }
    }
}

void child_fn() {
    return;
}