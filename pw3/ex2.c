/* ex2_getchar.c
 * Reimplementation of getchar using read() system call.
 */

#include <unistd.h>
#include <stdio.h>  

int my_getchar(void) {
    unsigned char c;
    ssize_t n = read(STDIN_FILENO, &c, 1);
    if (n == 1)
        return c;
    else
        return EOF; // -1
}

int main(void) {
    int ch;

    printf("Type something (Ctrl+D to end):\n");

    while ((ch = my_getchar()) != EOF) {
        putchar(ch);
    }

    printf("\n[EOF reached]\n");
    return 0;
}
