/* ex3_buffered_getchar.c
 * Buffered version of getchar using read() and a static buffer.
 */

#include <unistd.h>
#include <stdio.h>

#define BUF_SIZE 1024

int my_buffered_getchar(void) {
    static char buffer[BUF_SIZE]; //buffer
    static ssize_t buf_size = 0;  // number of bytesin buffer
    static ssize_t buf_pos = 0;   //unread position

    /* if buffer empty, read new data */
    if (buf_pos >= buf_size) {
        buf_size = read(STDIN_FILENO, buffer, BUF_SIZE);
        buf_pos = 0;
        if (buf_size <= 0)
            return EOF;  // end  file || error
    }

    /* return next character */
    return (unsigned char)buffer[buf_pos++];
}

int main(void) {
    int ch;

    printf("Type something (Ctrl+D to end):\n");

    while ((ch = my_buffered_getchar()) != EOF) {
        putchar(ch);
    }

    printf("\n[EOF reached]\n");
    return 0;
}
