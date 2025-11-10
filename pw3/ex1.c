/* ex1_syscall.c
 * Copies file "foo" into "bar" using only low-level system calls.
 */

#include <fcntl.h> 
#include <unistd.h>
#include <stdio.h>  
#include <stdlib.h> 

#define BUF_SIZE 1024

int main(void) {
    int fd_in, fd_out;
    ssize_t bytes_read, bytes_written;
    char buffer[BUF_SIZE];

    fd_in = open("foo", O_RDONLY);
    if (fd_in < 0) {
        perror("Error opening foo");
        exit(EXIT_FAILURE);
    }

    fd_out = open("bar", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd_out < 0) {
        perror("Error creating bar");
        close(fd_in);
        exit(EXIT_FAILURE);
    }

    while ((bytes_read = read(fd_in, buffer, BUF_SIZE)) > 0) {
        bytes_written = write(fd_out, buffer, bytes_read);
        if (bytes_written != bytes_read) {
            perror("Error writing to bar");
            close(fd_in);
            close(fd_out);
            exit(EXIT_FAILURE);
        }
    }

    if (bytes_read < 0)
        perror("Error reading foo");

    close(fd_in);
    close(fd_out);

    return 0;
}
