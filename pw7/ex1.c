#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        pid_t child_pid = getpid();
        pid_t parent_pid = getppid();
        int exit_code = child_pid % 10;

        printf("Child process: my pid = %d, parent pid = %d\n", child_pid, parent_pid);
        exit(exit_code);
    }
    else
    {
        int status;
        pid_t child_pid = pid;

        printf("Parent process: child pid = %d\n", child_pid);

        if (wait(&status) == -1)
        {
            perror("wait");
            return 1;
        }

        if (WIFEXITED(status))
        {
            int exit_code = WEXITSTATUS(status);
            printf("Parent process: child %d terminated with return code %d\n", child_pid, exit_code);
        }
        else
        {
            printf("Parent process: child %d terminated abnormally\n", child_pid);
        }
    }

    return 0;
}

