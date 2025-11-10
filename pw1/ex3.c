#include <stdio.h>
#include <string.h>

/*
 * This program reads a string that may contain control or non-printable characters
 * and prints a new string showing its C representation.
 * Example: newline '\n' becomes "\\n" (two visible characters: backslash and n)
 */

int main(void) {
    char input[200];
    printf("Enter a string: ");
    fgets(input, sizeof(input), stdin);

    printf("\nC representation:\n");

    for (int i = 0; input[i] != '\0'; i++) {
        unsigned char c = input[i];  //to handle values >127 correctly

        if (c == '\n')       printf("\\n");
        else if (c == '\r')  printf("\\r");
        else if (c == '\t')  printf("\\t");
        else if (c == '\b')  printf("\\b");
        else if (c == '\\')  printf("\\\\");
        else if (c == '\"')  printf("\\\"");
        else if (c < 32 || c > 126)
            printf("\\%03o", c);  //non-printable
        else
            putchar(c);  //printable
    }

    printf("\n");
    return 0;
}
