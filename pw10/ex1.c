#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static volatile sig_atomic_t counter = 0;

/* Signal handler function - increment and display counter */
void signal_handler(int sig)
{
    counter++;
    printf("Counter: %d\n", counter);

    if (counter >= 5)
    {
        printf("Received SIGINT 5 times. Exiting...\n");
        exit(0);
    }
}

int main(void)
{
    /* Install signal handler for SIGINT */
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        perror("signal");
        return 1;
    }

    printf("Program started. Press Ctrl+C to send SIGINT signal.\n");
    printf("Program will exit after receiving SIGINT 5 times.\n");

    /* Infinite loop - wait for signals */
    while (1)
    {
        pause();
    }

    return 0;
}

