#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

static volatile sig_atomic_t counter = 0;
static volatile sig_atomic_t running = 1;
static volatile sig_atomic_t sigint_received = 0;
static volatile sig_atomic_t sigterm_received = 0;
static const char *filename = "counter_log.txt";

/* Signal handler for SIGINT - set flag only */
void sigint_handler(int sig)
{
    sigint_received = 1;
}

/* Signal handler for SIGTERM - set flag only */
void sigterm_handler(int sig)
{
    sigterm_received = 1;
    running = 0;
}

/* Function to write date and counter to file (called from main, not signal handler) */
void write_counter_to_file(void)
{
    time_t now;
    struct tm *timeinfo;
    char date_str[128];
    char buffer[256];
    int fd;

    time(&now);
    timeinfo = localtime(&now);
    strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("open");
        return;
    }

    snprintf(buffer, sizeof(buffer), "%s - Counter: %d\n", date_str, (int)counter);
    if (write(fd, buffer, strlen(buffer)) == -1)
    {
        perror("write");
    }

    close(fd);
}

/* Function to write "end" to file (called from main, not signal handler) */
void write_end_to_file(void)
{
    int fd;
    const char *end_msg = "end\n";

    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("open");
        return;
    }

    if (write(fd, end_msg, strlen(end_msg)) == -1)
    {
        perror("write");
    }

    close(fd);
}

int main(void)
{
    /* Install signal handlers */
    struct sigaction sa_int, sa_term;

    /* Setup SIGINT handler */
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    if (sigaction(SIGINT, &sa_int, NULL) == -1)
    {
        perror("sigaction SIGINT");
        return 1;
    }

    /* Setup SIGTERM handler */
    sa_term.sa_handler = sigterm_handler;
    sigemptyset(&sa_term.sa_mask);
    sa_term.sa_flags = 0;
    if (sigaction(SIGTERM, &sa_term, NULL) == -1)
    {
        perror("sigaction SIGTERM");
        return 1;
    }

    printf("Program started. Counter is incrementing.\n");
    printf("Press Ctrl+C (SIGINT) to record counter value.\n");
    printf("Send SIGTERM to write 'end' and terminate.\n");
    printf("PID: %d\n", getpid());

    /* Main loop - increment counter and check for signals */
    while (running)
    {
        counter++;
        usleep(100000); /* Sleep 0.1 seconds */

        /* Check for SIGINT and handle it outside signal handler */
        if (sigint_received)
        {
            sigint_received = 0;
            write_counter_to_file();
        }

        /* Check for SIGTERM and handle it outside signal handler */
        if (sigterm_received)
        {
            sigterm_received = 0;
            write_end_to_file();
            break;
        }
    }

    printf("Program terminating.\n");
    return 0;
}

