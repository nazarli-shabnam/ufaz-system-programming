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
static const char *filename = "counter_log.txt";

/* Signal handler for SIGINT - record date and counter value */
void sigint_handler(int sig)
{
    time_t now;
    struct tm *timeinfo;
    char date_str[128];
    char buffer[256];
    int fd;

    /* Get current date and time */
    time(&now);
    timeinfo = localtime(&now);
    strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M:%S", timeinfo);

    /* Open file for appending */
    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        /* Can't use perror in signal handler safely, but we'll try */
        return;
    }

    /* Write date and counter value */
    snprintf(buffer, sizeof(buffer), "%s - Counter: %d\n", date_str, (int)counter);
    write(fd, buffer, strlen(buffer));

    /* Close file */
    close(fd);
}

/* Signal handler for SIGTERM - write "end" and terminate */
void sigterm_handler(int sig)
{
    int fd;
    const char *end_msg = "end\n";

    /* Open file for appending */
    fd = open(filename, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        return;
    }

    /* Write "end" */
    write(fd, end_msg, strlen(end_msg));

    /* Close file */
    close(fd);

    /* Set flag to terminate */
    running = 0;
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

    /* Infinite loop - increment counter */
    while (running)
    {
        counter++;
        usleep(100000); /* Sleep 0.1 seconds to make counter visible */
    }

    printf("Program terminating.\n");
    return 0;
}

