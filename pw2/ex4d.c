/* main.c
 * Asks the user for a radius and prints perimeter and area.
 */

#include <stdio.h>

/* function prototypes defined in other translation units */
double perimeter(double radius);
double area(double radius);

int main(void) {
    double r;

    printf("Enter radius: ");
    if (scanf("%lf", &r) != 1 || r < 0) {
        fprintf(stderr, "Invalid radius.\n");
        return 1;
    }

    printf("Perimeter: %.3f\n", perimeter(r));
    printf("Area     : %.3f\n", area(r));

    return 0;
}


/*
 to run in one shot - gcc main.c perimeter.c area.c -o circle
    or in multiple steps:
    gcc -c perimeter.c
    gcc -c area.c
    gcc -c main.c
*/