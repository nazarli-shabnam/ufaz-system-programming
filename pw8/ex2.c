#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/times.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s <directory>\n", argv[0]);
        return 1;
    }

    /* Get times before fork */
    struct tms start_times, end_times;
    clock_t start_clock, end_clock;

    start_clock = times(&start_times);
    if (start_clock == (clock_t)-1)
    {
        perror("times");
        return 1;
    }

    /* Fork to create child process */
    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return 1;
    }

    if (pid == 0)
    {
        /* Child process: redirect stdout to /dev/null */
        int dev_null = open("/dev/null", O_WRONLY);
        if (dev_null == -1)
        {
            perror("open /dev/null");
            exit(1);
        }

        if (dup2(dev_null, STDOUT_FILENO) == -1)
        {
            perror("dup2");
            close(dev_null);
            exit(1);
        }
        close(dev_null);

        /* Execute ls -R */
        execlp("ls", "ls", "-R", argv[1], (char *)NULL);
        perror("execlp");
        exit(1);
    }
    else
    {
        /* Parent process: wait for child and get times */

        int status;
        if (wait(&status) == -1)
        {
            perror("wait");
            return 1;
        }

        end_clock = times(&end_times);
        if (end_clock == (clock_t)-1)
        {
            perror("times");
            return 1;
        }

        /* Calculate processor time used by child */
        clock_t user_time = end_times.tms_cutime - start_times.tms_cutime;
        clock_t sys_time = end_times.tms_cstime - start_times.tms_cstime;
        clock_t total_time = user_time + sys_time;

        long clock_ticks_per_sec = sysconf(_SC_CLK_TCK);
        if (clock_ticks_per_sec == -1)
        {
            perror("sysconf");
            return 1;
        }

        double user_seconds = (double)user_time / clock_ticks_per_sec;
        double sys_seconds = (double)sys_time / clock_ticks_per_sec;
        double total_seconds = (double)total_time / clock_ticks_per_sec;

        printf("User time: %.6f seconds\n", user_seconds);
        printf("System time: %.6f seconds\n", sys_seconds);
        printf("Total processor time: %.6f seconds\n", total_seconds);
    }

    return 0;
}

