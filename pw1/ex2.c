#include <stdio.h>
#include <string.h>
#include <ctype.h>

/* Structure that holds each month's name and number of days */
struct month {
    char name[10];  /* 9 + 1 characters as required */
    int days;
};

int main(void) {
    /* Constant array of 12 months */
    const struct month months[12] = {
        {"January", 31},
        {"February", 28},
        {"March", 31},
        {"April", 30},
        {"May", 31},
        {"June", 30},
        {"July", 31},
        {"August", 31},
        {"September", 30},
        {"October", 31},
        {"November", 30},
        {"December", 31}
    };

    char input[20];
    printf("Enter the name of a month: ");
    if (fgets(input, sizeof(input), stdin) == NULL) {
        printf("Error reading input.\n");
        return 1;
    }

    /* Remove trailing newline if present */
    input[strcspn(input, "\n")] = '\0';

    /* Normalize first letter uppercase, rest lowercase (for consistency) */
    if (strlen(input) > 0) {
        input[0] = toupper((unsigned char)input[0]);
        for (size_t i = 1; i < strlen(input); i++) {
            input[i] = tolower((unsigned char)input[i]);
        }
    }

    /* Search for the month */
    int found = 0;
    for (int i = 0; i < 12; i++) {
        if (strcmp(input, months[i].name) == 0) {
            printf("%s is month number %d and has %d days.\n",
                   months[i].name, i + 1, months[i].days);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("Error: '%s' is not a valid month name.\n", input);
    }

    return 0;
}
