/* ex3.c
 * Reads up to 100 double values, sorts them and prints them.
 */

#include <stdio.h>

#define MAX_ELEM 100

/* Reads doubles into array a.
 * - max_elem: capacity of a
 * - nb_elem : out parameter with number of elements actually read
 * Returns 0 on success, -1 on error or if no value could be read.
 */
int read_array(double *a, int max_elem, int *nb_elem) {
    int i = 0;
    while (i < max_elem && scanf("%lf", &a[i]) == 1) {
        i++;
    }

    *nb_elem = i;

    if (i == 0) {          /*treat as error */
        return -1;
    }
    return 0;
}

/* Simple ascending sort (selection sort, easy to understand). */
void sort_array(double *a, int nb_elem) {
    for (int i = 0; i < nb_elem - 1; i++) {
        int min_idx = i;
        for (int j = i + 1; j < nb_elem; j++) {
            if (a[j] < a[min_idx]) {
                min_idx = j;
            }
        }
        /* swap a[i] and a[min_idx] */
        double tmp = a[i];
        a[i] = a[min_idx];
        a[min_idx] = tmp;
    }
}

/* Display the nb_elem elements of a, one per line. */
void display_array(double *a, int nb_elem) {
    for (int i = 0; i < nb_elem; i++) {
        printf("%g\n", a[i]);
    }
}

int main(void) {
    double a[MAX_ELEM];
    int nb_elem;

    printf("Enter some real numbers (Ctrl+D / Ctrl+Z to stop):\n");

    if (read_array(a, MAX_ELEM, &nb_elem) != 0) {
        fprintf(stderr, "No valid numbers were read.\n");
        return 1;
    }

    sort_array(a, nb_elem);
    printf("\nSorted values:\n");
    display_array(a, nb_elem);

    return 0;
}
