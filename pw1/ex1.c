#include <stdio.h>
#include <string.h>

#define MAX_LEN 100   /* maximum size of our strings */

int main(void) {
    char src[MAX_LEN];    /* original string read from stdin   */
    char dest[MAX_LEN];   /* copy of the string                */

    /* 1) Read a line from standard input using fgets */
    printf("Enter a string: ");
    if (fgets(src, MAX_LEN, stdin) == NULL) {
        /* if fgets fails (EOF or error), we just exit */
        printf("Error reading input.\n");
        return 1;
    }

    /* 2) Copy the string into dest using strncpy */
    strncpy(dest, src, MAX_LEN);
    dest[MAX_LEN - 1] = '\0';  /* make sure dest is null-terminated */

    /* 3) Display the copied string using puts */
    puts(dest);

    /* 4) Display its length using strlen and printf */
    printf("Length of the string: %zu\n", strlen(dest));

    return 0;
}
