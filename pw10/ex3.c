#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>

/* Signal handler that displays signal meaning and terminates */
void signal_handler(int sig)
{
    /* Display the meaning of the signal using psignal */
    psignal(sig, "Received signal");

    /* Terminate the program */
    exit(0);
}

int main(void)
{
    /* Install signal handler for all common signals */
    /* Note: Some signals cannot be caught (SIGKILL, SIGSTOP) */

    signal(SIGINT, signal_handler);   /* Interrupt (Ctrl+C) */
    signal(SIGQUIT, signal_handler); /* Quit (Ctrl+\) */
    signal(SIGILL, signal_handler);  /* Illegal instruction */
    signal(SIGABRT, signal_handler);  /* Abort */
    signal(SIGFPE, signal_handler);  /* Floating point exception */
    signal(SIGSEGV, signal_handler); /* Segmentation violation */
    signal(SIGTERM, signal_handler); /* Termination */
    signal(SIGHUP, signal_handler);  /* Hangup */
    signal(SIGUSR1, signal_handler); /* User-defined signal 1 */
    signal(SIGUSR2, signal_handler); /* User-defined signal 2 */
    signal(SIGALRM, signal_handler); /* Alarm */
    signal(SIGPIPE, signal_handler); /* Broken pipe */
    signal(SIGCHLD, signal_handler); /* Child process terminated */

    printf("Program started. This program will handle any signal and display its meaning.\n");
    printf("Send a signal to this process (e.g., kill -SIGINT %d or press Ctrl+C).\n", getpid());
    printf("The program will display the signal meaning and terminate.\n");

    /* Infinite loop - wait for signals */
    while (1)
    {
        pause();
    }

    return 0;
}

