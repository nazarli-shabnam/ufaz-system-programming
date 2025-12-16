#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <n>\n", argv[0]);
        return 1;
    }

    int n = atoi(argv[1]);
    if (n <= 0)
    {
        fprintf(stderr, "Error: n must be a positive integer\n");
        return 1;
    }

    pid_t *child_pids = (pid_t *)malloc(n * sizeof(pid_t));
    if (!child_pids)
    {
        perror("malloc");
        return 1;
    }

    for (int i = 0; i < n; i++)
    {
        pid_t pid = fork();

        if (pid == -1)
        {
            perror("fork");
            free(child_pids);
            return 1;
        }

        if (pid == 0)
        {
            exit(0);
        }
        else
        {
            child_pids[i] = pid;
        }
    }

    for (int i = 0; i < n; i++)
    {
        int status;
        pid_t terminated_pid = wait(&status);

        if (terminated_pid == -1)
        {
            perror("wait");
            free(child_pids);
            return 1;
        }

        if (WIFEXITED(status))
        {
            int exit_code = WEXITSTATUS(status);
            printf("Process %d terminated with return code %d\n", terminated_pid, exit_code);
        }
        else
        {
            printf("Process %d terminated abnormally\n", terminated_pid);
        }
    }

    free(child_pids);
    return 0;
}

