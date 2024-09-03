#include "comm.h"
#include "utils.h"

#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

char msg[MSG_SIZE];

void manage_msg(char* new_msg) {
    char* tag = malloc(5);
    tag = strncpy(tag, new_msg, 4);

    if (strcmp(tag, "DONE") == 0) {
        system("clear");
        ready = true;
    }

    if (ready) {
        if (strcmp(tag, "CMD_") == 0)
            printf("%s\n", new_msg + 4);

        else if (strcmp(tag, "DATA") == 0) {
            
            // remove the newline character
            if (new_msg[strlen(new_msg) - 1] == '\r') {
                new_msg[strlen(new_msg) - 1] = '\n';
            }
            
            if (write(fd_write, new_msg + 4, strlen(new_msg) - 4) < 0) {
                perror("write error");
                return;
            }
            
            total_samples++;
            int start_sample = (total_samples > 100) ? (total_samples - 100) : 0;
            
            if (total_samples > 1) { // Controlla che ci siano dati da plottare
                fprintf(gnuplot_fp, "plot ");
                for (int i = 1; i <= 8; i++) {
                    fprintf(gnuplot_fp, "'data.txt' every ::%d::%d using ($0+1):%d with lines title 'Channel %d'%s", start_sample, total_samples, i, i, i < 8 ? ", " : "\n");
                }
                fflush(gnuplot_fp);
            }
        }
    }
}

void receive_msg(int fd) {
    static int received = 0; // Keep track of the received bytes across calls
    // Read data into the buffer
    int n = read(fd, msg + received, sizeof(msg) - received - 1);
    if (n > 0) {
        received += n;
        msg[received] = '\0'; // Ensure null-terminated string

        // Process each line in the received message
        char *line_start = msg;
        char *newline_pos = strchr(line_start, '\n');
        while (newline_pos != NULL) {
            *newline_pos = '\0'; // Null-terminate the current line
            manage_msg(line_start); // Process the complete line
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

void redirect_msg() {
    char cmd[256];
    int n = 0;
    if (fgets(cmd, sizeof(cmd), stdin) != NULL) {
        if (strcmp(cmd, "exit\n") == 0) { // Adjusted to include newline
            sig_handler(SIGINT);
        } else {
            // Send the command to the device
            // manage partially sent commands
            int sent  = 0;
            while (sent < strlen(cmd)) {
                n = write(fd, cmd + sent, strlen(cmd) - sent);
                fflush(stdout);
                if (n < 0) {
                    perror("write error");
                    return;
                } else {
                    sent += n;
                }
                if (DEBUG) printf("Command sent: %s", cmd); // Adjusted to include newline
            }
        }
    }
}