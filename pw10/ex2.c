/*
 * Exercise 2: Debugging with gdb and core files
 *
 * This exercise uses the program from ex1.c
 *
 * Steps to follow:
 *
 * 1. Enable core file generation:
 *    $ ulimit -c unlimited
 *
 * 2. Compile the program with debug symbols:
 *    $ gcc -g ex1.c -o ex1
 *
 * 3. Run the program:
 *    $ ./ex1
 *
 * 4. In another terminal or using Ctrl+\ (SIGQUIT), send SIGQUIT to the process:
 *    $ kill -QUIT <pid>
 *    OR press Ctrl+\ in the terminal running the program
 *
 * 5. This will generate a core file (usually named 'core' or 'core.<pid>')
 *
 * 6. Analyze the core file with gdb:
 *    $ gdb ./ex1 core
 *
 * 7. Use gdb commands to analyze:
 *    - (gdb) where          - Shows the call stack
 *    - (gdb) up             - Move up the call stack
 *    - (gdb) down           - Move down the call stack
 *    - (gdb) list           - Show source code around current location
 *    - (gdb) print counter  - Display the value of the counter variable
 *
 * Note: This is a demonstration program. The actual debugging exercise
 * should be performed manually using ex1.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static int counter = 0;

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
    if (signal(SIGINT, signal_handler) == SIG_ERR)
    {
        perror("signal");
        return 1;
    }

    printf("Program started. Press Ctrl+C to send SIGINT signal.\n");
    printf("Press Ctrl+\\ to send SIGQUIT and generate core file.\n");
    printf("Program will exit after receiving SIGINT 5 times.\n");

    while (1)
    {
        pause();
    }

    return 0;
}

