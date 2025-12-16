#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <username>\n", argv[0]);
        return 1;
    }

    const char *username = argv[1];
    const char *temp_file = "foo";

    /* Step 1: Run "ps eaux > foo" */
    pid_t pid1 = fork();

    if (pid1 == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid1 == 0)
    {
        /* Child process 1: ps eaux > foo */
        int fd = open(temp_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (fd == -1)
        {
            perror("open");
            exit(1);
        }

        if (dup2(fd, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(fd);
            exit(1);
        }
        close(fd);

        execlp("ps", "ps", "eaux", (char *)NULL);
        perror("execlp");
        exit(1);
    }
    else
    {
        /* Parent: wait for ps to complete */
        int status;
        if (wait(&status) == -1)
        {
            perror("wait");
            return 1;
        }

        if (!WIFEXITED(status) || WEXITSTATUS(status) != 0)
        {
            fprintf(stderr, "ps command failed\n");
            return 1;
        }
    }

    /* Step 2: Run "grep \"^$1 \" < foo > /dev/null" */
    pid_t pid2 = fork();

    if (pid2 == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid2 == 0)
    {
        /* Child process 2: grep "^username " < foo > /dev/null */
        char pattern[256];
        snprintf(pattern, sizeof(pattern), "^%s ", username);

        /* Redirect stdin from foo */
        int fd_in = open(temp_file, O_RDONLY);
        if (fd_in == -1)
        {
            perror("open foo");
            exit(1);
        }

        if (dup2(fd_in, STDIN_FILENO) == -1)
        {
            perror("dup2 stdin");
            close(fd_in);
            exit(1);
        }
        close(fd_in);

        /* Redirect stdout to /dev/null */
        int fd_out = open("/dev/null", O_WRONLY);
        if (fd_out == -1)
        {
            perror("open /dev/null");
            exit(1);
        }

        if (dup2(fd_out, STDOUT_FILENO) == -1)
        {
            perror("dup2 stdout");
            close(fd_out);
            exit(1);
        }
        close(fd_out);

        execlp("grep", "grep", pattern, (char *)NULL);
        perror("execlp");
        exit(1);
    }
    else
    {
        /* Parent: wait for grep to complete */
        int status;
        if (wait(&status) == -1)
        {
            perror("wait");
            return 1;
        }

        /* Step 3: If grep succeeded (exit code 0), display result using write */
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0)
        {
            char message[256];
            int len = snprintf(message, sizeof(message), "%s is connected\n", username);

            if (write(STDOUT_FILENO, message, len) == -1)
            {
                perror("write");
                return 1;
            }
        }
    }

    /* Clean up temporary file */
    unlink(temp_file);

    return 0;
}

