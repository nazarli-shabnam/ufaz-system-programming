#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/* Helper function to copy data from fdsrc to fddst */
void copy(int fdsrc, int fddst)
{
    char buffer[4096];
    ssize_t bytes_read;

    while ((bytes_read = read(fdsrc, buffer, sizeof(buffer))) > 0)
    {
        ssize_t bytes_written = 0;
        ssize_t total_written = 0;

        while (total_written < bytes_read)
        {
            bytes_written = write(fddst, buffer + total_written, bytes_read - total_written);
            if (bytes_written == -1)
            {
                perror("write");
                return;
            }
            total_written += bytes_written;
        }
    }

    if (bytes_read == -1)
    {
        perror("read");
    }
}

int main(void)
{
    int pipefd[2];

    /* Create a pipe */
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (pid == 0)
    {
        /* Child process: read from pipe and write to stdout */
        close(pipefd[1]); /* Close write end */

        copy(pipefd[0], STDOUT_FILENO);

        close(pipefd[0]);
        exit(0);
    }
    else
    {
        /* Parent process: read from stdin and write to pipe */
        close(pipefd[0]); /* Close read end */

        copy(STDIN_FILENO, pipefd[1]);

        close(pipefd[1]);

        /* Wait for child to terminate */
        int status;
        if (wait(&status) == -1)
        {
            perror("wait");
            return 1;
        }
    }

    return 0;
}

