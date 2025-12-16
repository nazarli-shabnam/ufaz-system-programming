#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *name;
    char pattern[256];

    /* Get name from argument or environment variable USER */
    if (argc >= 2)
    {
        name = argv[1];
    }
    else
    {
        name = getenv("USER");
        if (!name)
        {
            fprintf(stderr, "Error: No name provided and USER environment variable not set\n");
            return 1;
        }
    }

    snprintf(pattern, sizeof(pattern), "^%s", name);

    /* Create first pipe: ps -> grep */
    int pipe1[2];
    if (pipe(pipe1) == -1)
    {
        perror("pipe");
        return 1;
    }

    /* Create second pipe: grep -> wc */
    int pipe2[2];
    if (pipe(pipe2) == -1)
    {
        perror("pipe");
        close(pipe1[0]);
        close(pipe1[1]);
        return 1;
    }

    /* First child: ps eaux */
    pid_t pid1 = fork();

    if (pid1 == -1)
    {
        perror("fork");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        return 1;
    }

    if (pid1 == 0)
    {
        /* Child 1: ps eaux */
        close(pipe1[0]); /* Close read end */
        close(pipe2[0]); /* Close unused ends */
        close(pipe2[1]);

        if (dup2(pipe1[1], STDOUT_FILENO) == -1)
        {
            perror("dup2");
            exit(1);
        }
        close(pipe1[1]);

        execlp("ps", "ps", "eaux", (char *)NULL);
        perror("execlp");
        exit(1);
    }

    /* Second child: grep "^<name>" */
    pid_t pid2 = fork();

    if (pid2 == -1)
    {
        perror("fork");
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);
        return 1;
    }

    if (pid2 == 0)
    {
        /* Child 2: grep "^<name>" */
        close(pipe1[1]); /* Close write end of pipe1 */
        close(pipe2[0]); /* Close read end of pipe2 */

        if (dup2(pipe1[0], STDIN_FILENO) == -1)
        {
            perror("dup2 stdin");
            exit(1);
        }
        close(pipe1[0]);

        if (dup2(pipe2[1], STDOUT_FILENO) == -1)
        {
            perror("dup2 stdout");
            exit(1);
        }
        close(pipe2[1]);

        execlp("grep", "grep", pattern, (char *)NULL);
        perror("execlp");
        exit(1);
    }

    /* Parent: wc -l */
    close(pipe1[0]);
    close(pipe1[1]);
    close(pipe2[1]); /* Close write end */

    if (dup2(pipe2[0], STDIN_FILENO) == -1)
    {
        perror("dup2");
        close(pipe2[0]);
        return 1;
    }
    close(pipe2[0]);

    execlp("wc", "wc", "-l", (char *)NULL);
    perror("execlp");
    return 1;
}

