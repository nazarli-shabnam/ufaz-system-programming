/* ex2.c
 * Very small implementation of "tail".
 * Usage: ./mytail [-n number] filename
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUF_SIZE 1024

/* Find the position in the file where we should start printing
 * to get the last n lines. Returns a byte offset. */
long find_start_pos(FILE *f, int n_lines) {
    char buf[BUF_SIZE];
    long file_size;
    int newline_count = 0;

    fseek(f, 0, SEEK_END);
    file_size = ftell(f);

    long pos = file_size;

    while (pos > 0 && newline_count <= n_lines) {
        long to_read = (pos >= BUF_SIZE) ? BUF_SIZE : pos;
        pos -= to_read;

        fseek(f, pos, SEEK_SET);
        size_t bytes = fread(buf, 1, to_read, f);

        /* scan backwards */
        for (long i = bytes - 1; i >= 0; i--) {
            if (buf[i] == '\n') {
                newline_count++;
                if (newline_count > n_lines) {
                    return pos + i + 1;
                }
            }
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    int n_lines = 10;  

    while ((opt = getopt(argc, argv, "n:")) != -1) {
        switch (opt) {
        case 'n':
            n_lines = atoi(optarg);
            if (n_lines <= 0) {
                fprintf(stderr, "Invalid number of lines: %s\n", optarg);
                return 1;
            }
            break;
        default:
            fprintf(stderr, "Usage: %s [-n number] file\n", argv[0]);
            return 1;
        }
    }

    if (optind >= argc) {
        fprintf(stderr, "No input file given.\n");
        fprintf(stderr, "Usage: %s [-n number] file\n", argv[0]);
        return 1;
    }

    const char *filename = argv[optind];
    FILE *f = fopen(filename, "r");
    if (f == NULL) {
        perror("Error opening file");
        return 1;
    }

    long start = find_start_pos(f, n_lines);

    fseek(f, start, SEEK_SET);

    int ch;
    while ((ch = getc(f)) != EOF) {
        putc(ch, stdout);
    }

    fclose(f);
    return 0;
}
