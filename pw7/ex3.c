#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
        return 1;
    }

    const char *filename = argv[1];

    int fd = open(filename, O_RDONLY);
    if (fd == -1)
    {
        perror("open");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        close(fd);
        return 1;
    }

    char buffer[1];
    int bytes_read = 0;
    int total_read = 0;

    if (pid == 0)
    {
        printf("[Child %d] Starting to read file...\n", getpid());

        while ((bytes_read = read(fd, buffer, 1)) > 0)
        {
            printf("[Child %d] Read: %c\n", getpid(), buffer[0]);
            total_read++;
        }

        if (bytes_read == -1)
        {
            perror("read (child)");
        }

        printf("[Child %d] Finished reading. Total characters read: %d\n", getpid(), total_read);
        close(fd);
        exit(0);
    }
    else
    {
        printf("[Parent %d] Starting to read file...\n", getpid());

        while ((bytes_read = read(fd, buffer, 1)) > 0)
        {
            printf("[Parent %d] Read: %c\n", getpid(), buffer[0]);
            total_read++;
        }

        if (bytes_read == -1)
        {
            perror("read (parent)");
        }

        printf("[Parent %d] Finished reading. Total characters read: %d\n", getpid(), total_read);

        int status;
        if (wait(&status) == -1)
        {
            perror("wait");
        }

        close(fd);

        struct stat st;
        if (stat(filename, &st) == 0)
        {
            printf("\nFile size: %ld bytes\n", (long)st.st_size);
            printf("Parent process read: %d characters\n", total_read);
            printf("Note: Both processes share the same file descriptor, so they read\n");
            printf("from the same file position. The total characters read by both\n");
            printf("processes should equal the file size (approximately %ld bytes),\n", (long)st.st_size);
            printf("since they are reading from the same position and competing for\n");
            printf("the same characters. Check the child's output for its count.\n");
        }
    }

    return 0;
}
