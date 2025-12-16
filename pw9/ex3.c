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

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n < 2)
    {
        fprintf(stderr, "Error: n must be at least 2\n");
        return 1;
    }

    /* Create n-1 pipes */
    int **pipes = (int **)malloc((n - 1) * sizeof(int *));
    if (!pipes)
    {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < n - 1; i++)
    {
        pipes[i] = (int *)malloc(2 * sizeof(int));
        if (!pipes[i])
        {
            perror("malloc");
            for (int j = 0; j < i; j++)
            {
                free(pipes[j]);
            }
            free(pipes);
            return 1;
        }

        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            for (int j = 0; j < i; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
                free(pipes[j]);
            }
            free(pipes);
            return 1;
        }
    }

    /* Create n-1 child processes, all directly linked to parent */
    pid_t *child_pids = (pid_t *)malloc((n - 1) * sizeof(pid_t));
    if (!child_pids)
    {
        perror("malloc");
        for (int i = 0; i < n - 1; i++)
        {
            close(pipes[i][0]);
            close(pipes[i][1]);
            free(pipes[i]);
        }
        free(pipes);
        return 1;
    }

    for (int i = 0; i < n - 1; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            /* Clean up */
            for (int j = 0; j < n - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
                free(pipes[j]);
            }
            free(pipes);
            free(child_pids);
            return 1;
        }

        if (pid == 0)
        {
            /* Child process i+1 (1-indexed) */
            int input_fd, output_fd;

            /* Determine input: first child reads from stdin, others from previous pipe */
            if (i == 0)
            {
                input_fd = STDIN_FILENO;
            }
            else
            {
                input_fd = pipes[i - 1][0];
                close(pipes[i - 1][1]); /* Close write end of previous pipe */
            }

            /* Determine output: all children write to their pipe */
            output_fd = pipes[i][1];
            close(pipes[i][0]); /* Close read end of current pipe */

            /* Close all other pipes */
            for (int j = 0; j < n - 1; j++)
            {
                if (j != i && j != i - 1)
                {
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }
            }

            copy(input_fd, output_fd);

            close(output_fd);
            if (i > 0)
            {
                close(input_fd);
            }

            exit(0);
        }
        else
        {
            child_pids[i] = pid;
        }
    }

    /* Parent process (nth process): read from last pipe and write to stdout */
    close(pipes[n - 2][1]); /* Close write end of last pipe */

    /* Close all other pipe ends */
    for (int i = 0; i < n - 2; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    copy(pipes[n - 2][0], STDOUT_FILENO);

    close(pipes[n - 2][0]);

    /* Wait for all children to terminate */
    for (int i = 0; i < n - 1; i++)
    {
        int status;
        if (wait(&status) == -1)
        {
            perror("wait");
        }
    }

    /* Free allocated memory */
    for (int i = 0; i < n - 1; i++)
    {
        free(pipes[i]);
    }
    free(pipes);
    free(child_pids);

    return 0;
}

