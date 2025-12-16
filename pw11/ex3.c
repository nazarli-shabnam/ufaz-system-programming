#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/wait.h>

static volatile sig_atomic_t bytes_written = 0;
static volatile sig_atomic_t signal_received = 0;

/* Signal handler - set flag to display pipe size */
void signal_handler(int sig)
{
    signal_received = 1;
}

int main(void)
{
    int pipefd[2];
    char buffer[1] = {'X'};
    pid_t pid;

    /* Create pipe */
    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        return 1;
    }

    /* Install signal handler */
    struct sigaction sa;
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGUSR1, &sa, NULL) == -1)
    {
        perror("sigaction");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    /* Fork to create writer process */
    pid = fork();

    if (pid == -1)
    {
        perror("fork");
        close(pipefd[0]);
        close(pipefd[1]);
        return 1;
    }

    if (pid == 0)
    {
        /* Child process: writer */
        close(pipefd[0]); /* Close read end - no reader */

        printf("Writer process (PID: %d) starting to write to pipe...\n", getpid());
        printf("Send SIGUSR1 signal to display pipe size: kill -USR1 %d\n", getpid());

        /* Write data until pipe is full and process blocks */
        while (1)
        {
            ssize_t n = write(pipefd[1], buffer, 1);
            
            if (n == -1)
            {
                /* Check if write was interrupted by signal */
                if (errno == EINTR)
                {
                    printf("\nSignal received while write() was blocked.\n");
                    printf("Bytes written to pipe: %d\n", (int)bytes_written);
                    printf("Pipe size: %d bytes\n", (int)bytes_written);
                    break;
                }
                else
                {
                    perror("write");
                    break;
                }
            }
            
            bytes_written++;

            /* Check if signal was received (unlikely while writing, but check anyway) */
            if (signal_received)
            {
                printf("\nSignal received.\n");
                printf("Bytes written to pipe: %d\n", (int)bytes_written);
                printf("Pipe size: %d bytes\n", (int)bytes_written);
                break;
            }
        }

        close(pipefd[1]);
        exit(0);
    }
    else
    {
        /* Parent process: reader (but doesn't actually read) */
        close(pipefd[1]); /* Close write end */
        /* Keep read end open but don't read from it */

        printf("Parent process (PID: %d) - pipe read end is open but not reading.\n", getpid());
        printf("Writer process PID: %d\n", pid);
        printf("Waiting for writer to fill pipe and block...\n");
        printf("Then send: kill -USR1 %d\n", pid);

        /* Wait for child to finish */
        int status;
        if (wait(&status) == -1)
        {
            perror("wait");
        }

        close(pipefd[0]);
    }

    return 0;
}

