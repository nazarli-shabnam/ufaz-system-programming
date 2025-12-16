#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    struct timeval start_time, end_time;
    long elapsed_seconds, elapsed_microseconds;

    /* Display start time */
    if (gettimeofday(&start_time, NULL) == -1)
    {
        perror("gettimeofday");
        return 1;
    }
    printf("Start time: %ld seconds, %ld microseconds\n",
           (long)start_time.tv_sec, (long)start_time.tv_usec);

    /* Fork to create child process */
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        /* Child process: execute ls -1 */
        execlp("ls", "ls", "-1", argv[1], (char *)NULL);
        perror("execlp");
        exit(1);
    }
    else
    {
        /* Parent process: wait for child to complete */
        int status;
        if (wait(&status) == -1)
        {
            perror("wait");
            return 1;
        }

        /* Display end time */
        if (gettimeofday(&end_time, NULL) == -1)
        {
            perror("gettimeofday");
            return 1;
        }
        printf("End time: %ld seconds, %ld microseconds\n",
               (long)end_time.tv_sec, (long)end_time.tv_usec);

        /* Calculate elapsed time */
        elapsed_seconds = end_time.tv_sec - start_time.tv_sec;
        elapsed_microseconds = end_time.tv_usec - start_time.tv_usec;

        if (elapsed_microseconds < 0)
        {
            elapsed_seconds--;
            elapsed_microseconds += 1000000;
        }

        printf("Time taken by ls command: %ld seconds, %ld microseconds\n",
               elapsed_seconds, elapsed_microseconds);
    }

    return 0;
}

