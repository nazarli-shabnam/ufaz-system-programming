#include <stdio.h>
#include <string.h>
#include <stdlib.h>

/*
 * This program reverses Exercise 3:
 * It reads a string like "Hello\\nUFAZ\\tRocks"
 * and transforms it into actual characters (newline, tab, etc.).
 */

int main(void)
{
    char input[200];
    char output[200];
    int i = 0, j = 0;

    printf("Enter a C-style string (e.g., Hello\\nUFAZ\\tRocks): ");
    fgets(input, sizeof(input), stdin);

    while (input[i] != '\0')
    {
        if (input[i] == '\\')
        { // backslash found
            i++;
            switch (input[i])
            {
            case 'n':
                output[j++] = '\n';
                break;
            case 't':
                output[j++] = '\t';
                break;
            case 'r':
                output[j++] = '\r';
                break;
            case 'b':
                output[j++] = '\b';
                break;
            case '\\':
                output[j++] = '\\';
                break;
            case '\"':
                output[j++] = '\"';
                break;
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            {
                int val = 0, k = 0;
                while (k < 3 && input[i] >= '0' && input[i] <= '7')
                {
                    val = val * 8 + (input[i] - '0');
                    i++;
                    k++;
                }
                i--; // adjust because outer loop will increment
                output[j++] = (char)val;
                break;
            }
            default:
                output[j++] = '\\';
                output[j++] = input[i];
                break;
            }
        }
        else
        {
            output[j++] = input[i];
        }
        i++;
    }

    output[j] = '\0';

    printf("\nTranslated string:\n");
    printf("%s\n", output);

    return 0;
}
