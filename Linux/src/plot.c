#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include "plot.h"
#include "utils.h"
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

FILE* gnuplot_init(const char* data_file, const char *script_file, int start_sample, int total_samples) {
    close(gnuplot_pipe[0]); // Close read end
    FILE *gnuplot_fp = fdopen(gnuplot_pipe[1], "w");
    if (gnuplot_fp == NULL) {
        perror("fdopen");
        exit(EXIT_FAILURE);
    }
    
    fprintf(gnuplot_fp, "set terminal x11 size 1500, 500 font 'Helvetica,12'\n");
    fprintf(gnuplot_fp, "set title 'Oscilloscope'\n");
    fprintf(gnuplot_fp, "set xlabel 'samples'\n");
    fprintf(gnuplot_fp, "set ylabel 'value'\n");
    fprintf(gnuplot_fp, "set yrange [-5:260]\n");
    fprintf(gnuplot_fp, "set grid\n");
    fprintf(gnuplot_fp, "set style data lines\n");
    fprintf(gnuplot_fp, "set style line 1 linewidth 3\n");
    
    return gnuplot_fp;
}